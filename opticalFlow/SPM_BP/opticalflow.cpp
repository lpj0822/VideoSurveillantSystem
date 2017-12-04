#include "opticalflow.h"
#include "colorcode.h"
#include "FGS_Header.h"
#include "spmbp.h"

OpticalFlow::OpticalFlow()
{
    spmbp_params_default(&params);
}

OpticalFlow::~OpticalFlow()
{

}

cv::Mat OpticalFlow::runOpticalFlowEstimator(const cv::Mat &frame1, const cv::Mat frame2)
{
    cv::Mat result;
    cv::Mat input1;
    cv::Mat input2;

    cv::GaussianBlur(frame1, input1, cv::Size(0, 0), 0.9);
    cv::GaussianBlur(frame2, input2, cv::Size(0, 0), 0.9);

    height1 = input1.rows;
    width1 = input1.cols;
    height2 = input2.rows;
    width2 = input2.cols;

    flow12.create(height1, width1);
    flow21.create(height2, width2);
    occMap.create(height1, width1);

    //optical flow from frame 1 to frame 2
    opticalFlowEst(input1, input2, flow12, &params);
    //optical flow from frame 2 to frame 1
    opticalFlowEst(input2, input1, flow21, &params);

    //left-right consistancy check (occlusion estimation)
    occMap.create(height1, width1);
    occMapEst(flow12, flow21, occMap);

    //post processing (occlusion region filling)
    opticalFlowRefine(flow12, occMap, input1, flow_refined);

    MotionToColor(flow_refined, result, -1);
    return result;
}

void OpticalFlow::opticalFlowEst(cv::Mat& img1, cv::Mat& img2, cv::Mat_<cv::Vec2f>& flow, spm_bp_params* params)
{
    spm_bp* estimator = new spm_bp();
    estimator->loadPairs(img1, img2);

    // load params
    estimator->setParameters(params);

    // preparare data
    estimator->preProcessing();

    // spm-bp main start
    cv::Mat_<cv::Vec2f> flowResult;
    flowResult.create(height1, width1);
    estimator->runspm_bp(flowResult);

    flow = flowResult.clone();
    delete estimator;
}

void OpticalFlow::occMapEst(cv::Mat_<cv::Vec2f>& flow12, cv::Mat_<cv::Vec2f>& flow21, cv::Mat_<uchar>& occMap)
{
    int iy, ix;

    const float FLOW_PIXEL_THRESH = 2;

    occMap.setTo(255);
    for (iy = 0; iy < height1; ++iy)
    {
        for (ix = 0; ix < width1; ++ix)
        {
            cv::Vec2f fFlow = flow12[iy][ix];
            int ny, nx;

            ny = floor(iy + fFlow[1] + 0.5);
            nx = floor(ix + fFlow[0] + 0.5);

            if (ny >= 0 && ny < height1 && nx >= 0 && nx < width1)
            {
                cv::Vec2f bFlow = flow21[ny][nx];
                if (fabs(bFlow[1] + ny - iy) < FLOW_PIXEL_THRESH && fabs(bFlow[0] + nx - ix) < FLOW_PIXEL_THRESH)
                {
                    continue;
                }
            }
            occMap[iy][ix] = 0;
        }
    }

    cv::Mat bw = occMap;
    cv::Mat labelImage(occMap.size(), CV_32S);
    int nLabels = connectedComponents(bw, labelImage, 8);

    std::vector<int> hist(nLabels, 0);
    for (iy = 0; iy < height1; ++iy)
        for (ix = 0; ix < width1; ++ix)
            hist[labelImage.at<int>(iy, ix)]++;
    std::vector<int> rmv_list;
    rmv_list.reserve(20);
    for (int i = 0; i < nLabels; ++i)
    {
        if (hist[i] < 50)
            rmv_list.push_back(i);
    }
    for (iy = 0; iy < height1; ++iy)
    {
        for (ix = 0; ix < width1; ++ix)
        {
            for (int r = 0; r < rmv_list.size(); ++r)
                if (labelImage.at<int>(iy, ix) == rmv_list[r])
                    occMap[iy][ix] = 0;
        }
    }
}

void OpticalFlow::opticalFlowRefine(cv::Mat_<cv::Vec2f>& flow_in, cv::Mat_<uchar>& occMap, const cv::Mat_<cv::Vec3b>& weightColorImg, cv::Mat_<cv::Vec2f>& flow_refined)
{
    cv::Mat_<float> flow_in_single[2];
    cv::split(flow_in, flow_in_single);
    cv::Mat_<float> flow_out_single[2];
    cv::Mat_<float> occ_fgs;
    occMap.convertTo(occ_fgs, CV_32FC1);
    occMap = occMap;
    cv::multiply(flow_in_single[0], occ_fgs, flow_in_single[0]);
    cv::multiply(flow_in_single[1], occ_fgs, flow_in_single[1]);
    FGS(flow_in_single[0], weightColorImg, flow_out_single[0], 0.01, 100);
    FGS(flow_in_single[1], weightColorImg, flow_out_single[1], 0.01, 100);
    FGS(occMap, weightColorImg, occ_fgs, 0.01, 100);
    cv::divide(flow_out_single[0], occ_fgs, flow_out_single[0]);
    cv::divide(flow_out_single[1], occ_fgs, flow_out_single[1]);

    cv::merge(flow_out_single, 2, flow_refined);
}
