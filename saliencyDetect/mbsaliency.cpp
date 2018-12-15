#include "mbsaliency.h"
#include <opencv2/imgproc.hpp>
#include <iostream>

#define MAX_IMG_DIM 300
#define FRAME_MAX 20
#define SOBEL_THRESH 0.4

#define USE_GEODESIC 0

static int findFrameMargin(const cv::Mat& img, bool reverse)
{
    cv::Mat edgeMap, edgeMapDil, edgeMask;
    cv::Sobel(img, edgeMap, CV_16SC1, 0, 1);
    edgeMap = cv::abs(edgeMap);
    edgeMap.convertTo(edgeMap, CV_8UC1);
    edgeMask = edgeMap < (SOBEL_THRESH * 255.0);
    cv::dilate(edgeMap, edgeMapDil, cv::Mat(), cv::Point(-1, -1), 2);
    edgeMap = edgeMap == edgeMapDil;
    edgeMap.setTo(cv::Scalar(0.0), edgeMask);


    if (!reverse)
    {
        for (int i = edgeMap.rows - 1; i >= 0; i--)
            if (cv::mean(edgeMap.row(i))[0] > 0.6*255.0)
                return i + 1;
    }
    else
    {
        for (int i = 0; i < edgeMap.rows; i++)
            if (cv::mean(edgeMap.row(i))[0] > 0.6*255.0)
                return edgeMap.rows - i;
    }

    return 0;
}

static bool removeFrame(const cv::Mat& src, cv::Mat& outImg, cv::Rect &roi)
{
    cv::Mat inImg = src.clone();
    if (inImg.rows < 2 * (FRAME_MAX + 3) || inImg.cols < 2 * (FRAME_MAX + 3))
    {
        roi = cv::Rect(0, 0, inImg.cols, inImg.rows);
        outImg = inImg;
        return false;
    }

    cv::Mat imgGray;
    cv::cvtColor(inImg, imgGray, cv::COLOR_BGR2GRAY);

    int up, dn, lf, rt;

    up = findFrameMargin(imgGray.rowRange(0, FRAME_MAX), false);
    dn = findFrameMargin(imgGray.rowRange(imgGray.rows - FRAME_MAX, imgGray.rows), true);
    lf = findFrameMargin(imgGray.colRange(0, FRAME_MAX).t(), false);
    rt = findFrameMargin(imgGray.colRange(imgGray.cols - FRAME_MAX, imgGray.cols).t(), true);

    int margin = MAX(up, MAX(dn, MAX(lf, rt)));
    if ( margin == 0 )
    {
        roi = cv::Rect(0, 0, imgGray.cols, imgGray.rows);
        outImg = inImg;
        return false;
    }

    int count = 0;
    count = up == 0 ? count : count + 1;
    count = dn == 0 ? count : count + 1;
    count = lf == 0 ? count : count + 1;
    count = rt == 0 ? count : count + 1;

    // cut four border region if at least 2 border frames are detected
    if (count > 1)
    {
        margin += 2;
        roi = cv::Rect(margin, margin, inImg.cols - 2*margin, inImg.rows - 2*margin);
        outImg = cv::Mat(inImg, roi);
        return true;
    }

    // otherwise, cut only one border
    up = up == 0 ? up : up + 2;
    dn = dn == 0 ? dn : dn + 2;
    lf = lf == 0 ? lf : lf + 2;
    rt = rt == 0 ? rt : rt + 2;

    roi = cv::Rect(lf, up, inImg.cols - lf - rt, inImg.rows - up - dn);
    outImg = cv::Mat(inImg, roi);

    return true;

}

MBSaliency::MBSaliency()
{
    std::cout << "MBSaliency()" << std::endl;
}

MBSaliency::~MBSaliency()
{
    std::cout << "~MBSaliency()" << std::endl;
}

cv::Mat MBSaliency::getSaliency(const cv::Mat &src)
{
    cv::Mat result = cv::Mat::zeros(src.size(), CV_32FC1);
    cv::Mat srcRoi;
    cv::Rect rect;
    // detect and remove the artifical frame of the image
#if USE_GEODESIC == 1
    srcRoi = src;
    rect = cv::Rect(0, 0, src.cols, src.rows);
#else
    removeFrame(src, srcRoi, rect);
#endif

#if USE_GEODESIC == 0
    cv::cvtColor(srcRoi, srcRoi, cv::COLOR_BGR2Lab);
#endif

    cv::Mat saliencyMap = cv::Mat::zeros(srcRoi.size(), CV_32FC1);;
    mFeatureMaps.clear();
    cv::split(srcRoi, mFeatureMaps);
    for (int i = 0; i < mFeatureMaps.size(); i++)
    {
        cv::medianBlur(mFeatureMaps[i], mFeatureMaps[i], 5);
    }
#if USE_GEODESIC == 1
    saliencyMap = fastGeodesic(mFeatureMaps);
#else
    saliencyMap = fastMBS(mFeatureMaps);
#endif
    cv::normalize(saliencyMap, cv::Mat(result, rect), 1.0, 0.0, cv::NORM_MINMAX);
    return result;
}

cv::Mat MBSaliency::fastGeodesic(const std::vector<cv::Mat> featureMaps)
{
    assert(featureMaps[0].type() == CV_8UC1);

    cv::Size sz = featureMaps[0].size();
    cv::Mat ret = cv::Mat::zeros(sz, CV_32FC1);
    if (sz.width < 3 || sz.height < 3)
        return ret;

    for (int i = 0; i < featureMaps.size(); i++)
    {
        // determines the threshold for clipping
        float thresh = getThreshForGeo(featureMaps[i]);
        //cout << thresh << endl;
        cv::Mat map = cv::Mat::zeros(sz, CV_32FC1);
        cv::Mat mapROI(map, cv::Rect(1, 1, sz.width - 2, sz.height - 2));
        mapROI.setTo(cv::Scalar(1000000000));

        rasterScanGeo(featureMaps[i], map, thresh);
        invRasterScanGeo(featureMaps[i], map, thresh);
        rasterScanGeo(featureMaps[i], map, thresh);

        ret += map;
    }

    return ret;
}

cv::Mat MBSaliency::fastMBS(const std::vector<cv::Mat> featureMaps)
{
    assert(featureMaps[0].type() == CV_8UC1);

    cv::Size sz = featureMaps[0].size();
    cv::Mat ret = cv::Mat::zeros(sz, CV_32FC1);
    if (sz.width < 3 || sz.height < 3)
        return ret;

    for (int i = 0; i < featureMaps.size(); i++)
    {
        cv::Mat map = cv::Mat::zeros(sz, CV_32FC1);
        cv::Mat mapROI(map, cv::Rect(1, 1, sz.width - 2, sz.height - 2));
        mapROI.setTo(cv::Scalar(100000));
        cv::Mat lb = featureMaps[i].clone();
        cv::Mat ub = featureMaps[i].clone();

        rasterScan(featureMaps[i], map, lb, ub);
        invRasterScan(featureMaps[i], map, lb, ub);
        rasterScan(featureMaps[i], map, lb, ub);

        ret += map;
    }

    return ret;
}


void MBSaliency::rasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub)
{
    cv::Size sz = featMap.size();
    float *pMapup = (float*)map.data + 1;
    float *pMap = pMapup + sz.width;
    uchar *pFeatup = featMap.data + 1;
    uchar *pFeat = pFeatup + sz.width;
    uchar *pLBup = lb.data + 1;
    uchar *pLB = pLBup + sz.width;
    uchar *pUBup = ub.data + 1;
    uchar *pUB = pUBup + sz.width;

    float mapPrev;
    float featPrev;
    uchar lbPrev, ubPrev;

    float lfV, upV;
    int flag;
    for (int r = 1; r < sz.height - 1; r++)
    {
        mapPrev = *(pMap - 1);
        featPrev = *(pFeat - 1);
        lbPrev = *(pLB - 1);
        ubPrev = *(pUB - 1);


        for (int c = 1; c < sz.width - 1; c++)
        {
            lfV = (float)(MAX(*pFeat, ubPrev) - MIN(*pFeat, lbPrev));//(*pFeat >= lbPrev && *pFeat <= ubPrev) ? mapPrev : mapPrev + abs((float)(*pFeat) - featPrev);
            upV = (float)(MAX(*pFeat, *pUBup) - MIN(*pFeat, *pLBup));//(*pFeat >= *pLBup && *pFeat <= *pUBup) ? *pMapup : *pMapup + abs((float)(*pFeat) - (float)(*pFeatup));

            flag = 0;
            if (lfV < *pMap)
            {
                *pMap = lfV;
                flag = 1;
            }
            if (upV < *pMap)
            {
                *pMap = upV;
                flag = 2;
            }

            switch (flag)
            {
            case 0:		// no update
                break;
            case 1:		// update from left
                *pLB = MIN(*pFeat, lbPrev);
                *pUB = MAX(*pFeat, ubPrev);
                break;
            case 2:		// update from up
                *pLB = MIN(*pFeat, *pLBup);
                *pUB = MAX(*pFeat, *pUBup);
                break;
            default:
                break;
            }

            mapPrev = *pMap;
            pMap++;
            pMapup++;
            featPrev = *pFeat;
            pFeat++;
            pFeatup++;
            lbPrev = *pLB;
            pLB++;
            pLBup++;
            ubPrev = *pUB;
            pUB++;
            pUBup++;
        }
        pMapup += 2;
        pMap += 2;
        pFeat += 2;
        pFeatup += 2;
        pLBup += 2;
        pLB += 2;
        pUBup += 2;
        pUB += 2;
    }
}

void MBSaliency::invRasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub)
{
    cv::Size sz = featMap.size();
    int datalen = sz.width*sz.height;
    float *pMapdn = (float*)map.data + datalen - 2;
    float *pMap = pMapdn - sz.width;
    uchar *pFeatdn = featMap.data + datalen - 2;
    uchar *pFeat = pFeatdn - sz.width;
    uchar *pLBdn = lb.data + datalen - 2;
    uchar *pLB = pLBdn - sz.width;
    uchar *pUBdn = ub.data + datalen - 2;
    uchar *pUB = pUBdn - sz.width;

    float mapPrev;
    float featPrev;
    uchar lbPrev, ubPrev;

    float rtV, dnV;
    int flag;
    for (int r = 1; r < sz.height - 1; r++)
    {
        mapPrev = *(pMap + 1);
        featPrev = *(pFeat + 1);
        lbPrev = *(pLB + 1);
        ubPrev = *(pUB + 1);

        for (int c = 1; c < sz.width - 1; c++)
        {
            rtV = (float)(MAX(*pFeat, ubPrev) - MIN(*pFeat, lbPrev));//(*pFeat >= lbPrev && *pFeat <= ubPrev) ? mapPrev : mapPrev + abs((float)(*pFeat) - featPrev);
            dnV = (float)(MAX(*pFeat, *pUBdn) - MIN(*pFeat, *pLBdn));//(*pFeat >= *pLBdn && *pFeat <= *pUBdn) ? *pMapdn : *pMapdn + abs((float)(*pFeat) - (float)(*pFeatdn));

            flag = 0;
            if (rtV < *pMap)
            {
                *pMap = rtV;
                flag = 1;
            }
            if (dnV < *pMap)
            {
                *pMap = dnV;
                flag = 2;
            }

            switch (flag)
            {
            case 0:		// no update
                break;
            case 1:		// update from right
                *pLB = MIN(*pFeat, lbPrev);
                *pUB = MAX(*pFeat, ubPrev);
                break;
            case 2:		// update from down
                *pLB = MIN(*pFeat, *pLBdn);
                *pUB = MAX(*pFeat, *pUBdn);
                break;
            default:
                break;
            }

            mapPrev = *pMap;
            pMap--;
            pMapdn--;
            featPrev = *pFeat;
            pFeat--;
            pFeatdn--;
            lbPrev = *pLB;
            pLB--;
            pLBdn--;
            ubPrev = *pUB;
            pUB--;
            pUBdn--;
        }

        pMapdn -= 2;
        pMap -= 2;
        pFeatdn -= 2;
        pFeat -= 2;
        pLBdn -= 2;
        pLB -= 2;
        pUBdn -= 2;
        pUB -= 2;
    }
}

float MBSaliency::getThreshForGeo(const cv::Mat& src)
{
    float ret=0.0f;
    cv::Size sz = src.size();

    uchar *pFeatup = src.data + 1;
    uchar *pFeat = pFeatup + sz.width;
    uchar *pfeatdn = pFeat + sz.width;

    float featPrev;

    for (int r = 1; r < sz.height - 1; r++)
    {
        featPrev = *(pFeat - 1);

        for (int c = 1; c < sz.width - 1; c++)
        {
            float temp = MIN(abs(*pFeat-featPrev),abs(*pFeat-*(pFeat+1)));
            temp = MIN(temp,abs(*pFeat-*pFeatup));
            temp = MIN(temp,abs(*pFeat-*pfeatdn));
            ret += temp;

            featPrev = *pFeat;
            pFeat++; pFeatup++; pfeatdn++;
        }
        pFeat += 2; pFeatup += 2; pfeatdn += 2;
    }
    return ret / ((sz.width - 2)*(sz.height - 2));
}

void MBSaliency::rasterScanGeo(const cv::Mat& featMap, cv::Mat& map, float thresh)
{
    cv::Size sz = featMap.size();
    float *pMapup = (float*)map.data + 1;
    float *pMap = pMapup + sz.width;
    uchar *pFeatup = featMap.data + 1;
    uchar *pFeat = pFeatup + sz.width;

    float mapPrev;
    float featPrev;

    float lfV, upV;
    for (int r = 1; r < sz.height - 1; r++)
    {
        mapPrev = *(pMap - 1);
        featPrev = *(pFeat - 1);


        for (int c = 1; c < sz.width - 1; c++)
        {
            lfV = (abs(featPrev - *pFeat)>thresh ? abs(featPrev - *pFeat):0.0f) + mapPrev;
            upV = (abs(*pFeatup - *pFeat)>thresh ? abs(*pFeatup - *pFeat):0.0f) + *pMapup;

            if (lfV < *pMap)
            {
                *pMap = lfV;
            }
            if (upV < *pMap)
            {
                *pMap = upV;
            }

            mapPrev = *pMap;
            pMap++; pMapup++;
            featPrev = *pFeat;
            pFeat++; pFeatup++;
        }
        pMapup += 2;
        pMap += 2;
        pFeat += 2;
        pFeatup += 2;
    }
}

void MBSaliency::invRasterScanGeo(const cv::Mat& featMap, cv::Mat& map, float thresh)
{
    cv::Size sz = featMap.size();
    int datalen = sz.width*sz.height;
    float *pMapdn = (float*)map.data + datalen - 2;
    float *pMap = pMapdn - sz.width;
    uchar *pFeatdn = featMap.data + datalen - 2;
    uchar *pFeat = pFeatdn - sz.width;

    float mapPrev;
    float featPrev;

    float rtV, dnV;
    for (int r = 1; r < sz.height - 1; r++)
    {
        mapPrev = *(pMap + 1);
        featPrev = *(pFeat + 1);

        for (int c = 1; c < sz.width - 1; c++)
        {
            rtV = (abs(featPrev - *pFeat)>thresh ? abs(featPrev - *pFeat):0.0f) + mapPrev;
            dnV = (abs(*pFeatdn - *pFeat)>thresh ? abs(*pFeatdn - *pFeat):0.0f) + *pMapdn;

            if (rtV < *pMap)
            {
                *pMap = rtV;
            }
            if (dnV < *pMap)
            {
                *pMap = dnV;
            }

            mapPrev = *pMap;
            pMap--; pMapdn--;
            featPrev = *pFeat;
            pFeat--; pFeatdn--;
        }


        pMapdn -= 2; pMap -= 2;
        pFeatdn -= 2; pFeat -= 2;
    }
}
