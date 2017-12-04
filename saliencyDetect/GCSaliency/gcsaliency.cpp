#include "gcsaliency.h"
#include "utility.h"

double GCSaliency::dummyD=0.0;

GCSaliency::GCSaliency(bool isCSD):gmm(DEFAULT_GMM_NUM)
{
    this->isGetCSD=isCSD;
}

GCSaliency::~GCSaliency()
{

}

cv::Mat GCSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels()==3);
    std::vector<double> cd;
    cv::Mat saliencyMapCSD;
    cv::Mat saliencyMapGU;
    histgramGMMs(src);
    if (isGetCSD)
    {
        getCSD(colorSpatialDistribution, cd);
        saliencyMapCSD = getSaliencyFromGMMs(colorSpatialDistribution);
    }

    getGU(globalContrast, cd, 0.4, 15);
    saliencyMapGU = getSaliencyFromGMMs(globalContrast);

    if (isGetCSD)
    {
        return spatialVar(saliencyMapCSD) < spatialVar(saliencyMapGU) ? saliencyMapCSD : saliencyMapGU;
    }
    else
    {
        return saliencyMapGU;
    }
}

void GCSaliency::histgramGMMs(const cv::Mat &src)
{
    // Color quantization
    cv::Mat colorNumsFC1;
    cv::Mat binBGRFC3;
    ColorQuantize quantize;
    src.convertTo(imgFC3,CV_32FC3, 1.0/255);
    quantize.D_Quantize(imgFC3, histBinIndexIC1, binBGRFC3, colorNumsFC1);
    colorNumsFC1.convertTo(colorNumsFC1, CV_32FC1);

    // Train GMMs
    cv::Mat gmmIdx1i;
    gmm.BuildGMMs(binBGRFC3, gmmIdx1i, colorNumsFC1);
    NUM = gmm.RefineGMMs(binBGRFC3, gmmIdx1i, colorNumsFC1);
    pixelSaliencyComponentFC1.resize(NUM);
    histBinSaliencyComponentFC1.resize(NUM);
    gmmColors.resize(NUM);
    gmmWeight.resize(NUM);
    colorSpatialDistribution.resize(NUM);
    globalContrast.resize(NUM);
    finalSaliency.resize(NUM);

    // Assign GMM color means and weights
    for (int i = 0; i < NUM; i++)
    {
        gmmColors[i] = gmm.getMean(i);
        gmmWeight[i] = gmm.getWeight(i);
    }

    // Assign GMMs for each components
    gmm.GetProbs(binBGRFC3, histBinSaliencyComponentFC1);
#pragma omp parallel for
    for (int c = 0; c < NUM; c++)
    {
        pixelSaliencyComponentFC1[c].create(imgFC3.size(), CV_32FC1);
        float *prob = (float*)(histBinSaliencyComponentFC1[c].data);
        for (int y = 0; y < histBinIndexIC1.rows; y++)
        {
            const int *index = histBinIndexIC1.ptr<int>(y);
            float* probCI = pixelSaliencyComponentFC1[c].ptr<float>(y);
            for (int x = 0; x < histBinIndexIC1.cols; x++)
                probCI[x] = prob[index[x]];
        }
        cv::blur(pixelSaliencyComponentFC1[c], pixelSaliencyComponentFC1[c], cv::Size(3, 3));
    }
}

cv::Mat GCSaliency::getSaliencyFromGMMs(std::vector<double> &val, bool isNormlize)
{
    reScale(val);
    cv::Mat binSal = cv::Mat::zeros(histBinSaliencyComponentFC1[0].size(), CV_32FC1);
    int num = (int)histBinSaliencyComponentFC1.size(), type = histBinSaliencyComponentFC1[0].type();
    for (int c = 0; c < num; c++)
    {
        cv::Mat tmp;
        histBinSaliencyComponentFC1[c].convertTo(tmp, type, val[c]);
        cv::add(binSal, tmp, binSal);
    }
    cv::normalize(binSal, binSal, -4, 4, cv::NORM_MINMAX);
    float *bVal = binSal.ptr<float>(0);
    for(int i = 0; i < binSal.cols; i++)
        bVal[i] = 1 / (1 + exp(-bVal[i])); // Sigmoid
    if (isNormlize)
        cv::normalize(binSal, binSal, 1.0, 0, cv::NORM_MINMAX);
    cv::Mat salMat(histBinIndexIC1.size(), CV_32FC1);
    float* binSalVal = (float*)(binSal.data);
#pragma omp parallel for
    for (int r = 0; r < histBinIndexIC1.rows; r++)
    {
        const int* idx = histBinIndexIC1.ptr<int>(r);
        float* sal = salMat.ptr<float>(r);
        for (int c = 0; c < histBinIndexIC1.cols; c++)
            sal[c] = binSalVal[idx[c]];
    }
    cv::blur(salMat, salMat, cv::Size(3, 3));
    cv::normalize(salMat, salMat, 1.0, 0, cv::NORM_MINMAX);
    return salMat;
}

void GCSaliency::getCSD(std::vector<double> &csd, std::vector<double> &cD)
{
    mergeGMMs();
    std::vector<double> tempCSD, tempcD;
    int num = (int)probabilityGMMComponentsFC1.size(); // Number of Gaussian
    cv::Size imgSz = probabilityGMMComponentsFC1[0].size();

    tempcD.resize(num);
    std::vector<double> V(num), D(num);
#pragma omp parallel for
    for (int i = 0; i < num; i++)
        V[i] = spatialVar(probabilityGMMComponentsFC1[i], tempcD[i]);
    cv::normalize(V, V, 1.0, 0, cv::NORM_MINMAX);
    cv::normalize(tempcD, D, 1.0, 0,cv::NORM_MINMAX);
    tempCSD.resize(num);
    for (int i = 0; i < num; i++)
        tempCSD[i] = (1-V[i])*(1-D[i]);
    cv::normalize(tempCSD, tempCSD, 1.0, 0,cv::NORM_MINMAX);
    csd.resize(NUM);
    cD.resize(NUM);
    for (int i = 0; i < NUM; i++)
    {
        csd[i] = tempCSD[clusteredIndex[i]];
        cD[i] = tempcD[clusteredIndex[i]];
    }
}

void GCSaliency::getGU(std::vector<double>& gc, std::vector<double> &cD, double sigmaDist, double dominate)
{
    if (cD.size() == 0)
    {
        cD.resize(NUM);
#pragma omp parallel for
        for (int i = 0; i < NUM; i++)
            spatialVar(pixelSaliencyComponentFC1[i], cD[i]);
    }

    cv::Mat_<double> clrDist;
    clrDist = cv::Mat_<double>::zeros(NUM, NUM);
    std::vector<cv::Vec3f> tempGmmClrs(NUM);
    cv::cvtColor(gmmColors, tempGmmClrs, cv::COLOR_BGR2Lab);
    std::vector<double> maxDist(NUM);
    for (int i = 0; i < NUM; i++) for (int j = 0; j < i; j++)
    {
        double dCrnt = vecDist(tempGmmClrs[i], tempGmmClrs[j]);
        clrDist(i, j) = clrDist(j, i) = dCrnt;
        maxDist[i] = std::max(maxDist[i], dCrnt);
        maxDist[j] = std::max(maxDist[j], dCrnt);
    }

    for (int i = 0; i < NUM; i++)
    {
        gc[i] = 0;
        for (int j = 0; j < NUM; j++)
            gc[i] += gmmWeight[j] * clrDist(i, j) / maxDist[i]; //min(clrDist(i, j), dominate);
    }

    for (int i = 0; i < NUM; i++)
        globalContrast[i] = globalContrast[i] * exp(-9.0 *cD[i]*cD[i]);
    cv::normalize(globalContrast, globalContrast, 1.0, 0, cv::NORM_MINMAX);
}

void GCSaliency::reScale(std::vector<double>& saliencyVals, double minRatio)
{
    while (1)
    {
        double sumW = 0;
		double maxCrnt = 0;
        for (int i = 0; i < NUM; i++)
            if (saliencyVals[i] > 0.95)
                sumW += gmmWeight[i];
            else
                maxCrnt = std::max(maxCrnt, saliencyVals[i]);
        if (sumW < minRatio && maxCrnt > EPS)
            for (int i = 0; i < NUM; i++)
                saliencyVals[i] = std::min(saliencyVals[i]/maxCrnt, 1.0);
        else
            break;
    }
}

void GCSaliency::mergeGMMs()
{
    clusteredIndex.resize(NUM);
    for (int i = 0; i < NUM; i++)
        clusteredIndex[i] = i;

    APCluster apCluter(NUM);

    cv::Mat_<double> cor = cv::Mat_<double>::zeros(NUM, NUM);
    for (int y = 0; y < imgFC3.rows; y++)
    {
        for (int x = 0; x < imgFC3.cols; x++)
        {
            std::vector<CostfIdx> pIdx;
            pIdx.reserve(NUM);
            for (int c = 0;c < NUM; c++)
                pIdx.push_back(std::make_pair(pixelSaliencyComponentFC1[c].at<float>(y, x), c));
            std::sort(pIdx.begin(), pIdx.end(), std::greater<CostfIdx>());
            int i1 = pIdx[0].second, i2 = pIdx[1].second;
            float cost = std::min(pIdx[0].first, pIdx[1].first);
            cor(i1, i2) += cost;
            cor(i2, i1) += cost;
        }
    }

    double scale = imgFC3.rows*imgFC3.cols * 7 / 1e4; // Scale values for better illustration
    for (int i = 0; i < NUM; i++)
        for (int j = 0; j < i; j++)
            cor(i, j) = cor(j, i) = log(1 + cor(i, j) / (scale * std::min(gmmWeight[i], gmmWeight[j]) + 1E-4)) - 1;
    double preference = 4*cv::sum(cor).val[0]/(cor.cols*cor.rows);
    for (int i = 0; i < NUM; i++)
        cor(i, i) = preference; // Preference for apcluster

    clusteredIndex=apCluter.affinityPropagation(cor);
    NUM_Merged = apCluter.reMapIdx(clusteredIndex);
    probabilityGMMComponentsFC1.resize(NUM_Merged);

    for (int i = 0; i < NUM_Merged; i++)
        probabilityGMMComponentsFC1[i] = cv::Mat::zeros(imgFC3.size(), CV_32FC1);
    for (int i = 0; i < NUM; i++)
        probabilityGMMComponentsFC1[clusteredIndex[i]] += pixelSaliencyComponentFC1[i];
}

void GCSaliency::MergeGMMs()
{
    clusteredIndex.resize(NUM);
    for (int i = 0; i < NUM; i++)
        clusteredIndex[i] = i;

    CmAPCluster apCluter;
    CmAPCluster::apcluster32 apFun = apCluter.GetApFun();
    unsigned int N = NUM *NUM;
    if (apFun != NULL)
    {
        cv::Mat_<double> cor = cv::Mat_<double>::zeros(NUM, NUM);
        for (int y = 0; y < imgFC3.rows; y++)
        {
            for (int x = 0; x < imgFC3.cols; x++)
            {
                std::vector<CostfIdx> pIdx;
                pIdx.reserve(NUM);
                for (int c = 0;c < NUM; c++)
                    pIdx.push_back(std::make_pair(pixelSaliencyComponentFC1[c].at<float>(y, x), c));
                std::sort(pIdx.begin(), pIdx.end(), std::greater<CostfIdx>());
                int i1 = pIdx[0].second, i2 = pIdx[1].second;
                float cost = std::min(pIdx[0].first, pIdx[1].first);
                cor(i1, i2) += cost;
                cor(i2, i1) += cost;
            }
        }

        double scale = imgFC3.rows*imgFC3.cols * 7 / 1e4; // Scale values for better illustration
        for (int i = 0; i < NUM; i++)
            for (int j = 0; j < i; j++)
                cor(i, j) = cor(j, i) = log(1 + cor(i, j) / (scale * std::min(gmmWeight[i], gmmWeight[j]) + 1E-4)) - 1;
        //CmShow::Correlation(_gmmClrs, cor, _nameNE + "_MCOR.png", 20);
        double preference = 4*cv::sum(cor).val[0]/(cor.cols*cor.rows);
        for (int i = 0; i < NUM; i++)
            cor(i, i) = preference; // Preference for apcluster
        double netSim = 0;
        apFun(cor.ptr<double>(0), NULL, NULL, N, &clusteredIndex[0], &netSim, &apCluter.apoptions);
    }
    NUM_Merged = CmAPCluster::ReMapIdx(clusteredIndex);
    probabilityGMMComponentsFC1.resize(NUM_Merged);

    if (apFun != NULL)
    {
        for (int i = 0; i < NUM_Merged; i++)
            probabilityGMMComponentsFC1[i] = cv::Mat::zeros(imgFC3.size(), CV_32FC1);
        for (int i = 0; i < NUM; i++)
            probabilityGMMComponentsFC1[clusteredIndex[i]] += pixelSaliencyComponentFC1[i];
    }
    else
    {
        for (int i = 0; i < NUM; i++)
            probabilityGMMComponentsFC1[i] = pixelSaliencyComponentFC1[i];
    }
}

double GCSaliency::spatialVar(cv::Mat& map1f, double &cD)
{
    // Find centroid of the map
    const int h = map1f.rows, w = map1f.cols;
    const double H = h, W = w;
    double xM = 0, yM = 0, sumP = EPS;
    for (int y = 0; y < h; y++)
    {
        const float* prob = map1f.ptr<float>(y);
        for (int x = 0; x < w; x++)
        {
            sumP += prob[x];
            xM += prob[x] * x;
            yM += prob[x] * y;
        }
    }
    xM /= sumP, yM /= sumP;

    // Find variance of X and Y direction
    double vX = 0, vY = 0;
    cD = 0; // average center distance
    for (int y = 0; y < h; y++)
    {
        const float* prob = map1f.ptr<float>(y);
        for (int x = 0; x < w; x++)
        {
            double p = prob[x];
            vX += p * (x - xM)*(x - xM);
			vY += p * (y - yM)*(y - yM);
            cD += p * sqrt((x/W - 0.5)*(x/W - 0.5) + (y/H - 0.5)*(y/H - 0.5));
        }
    }
    cD /= sumP;
    return vX/sumP + vY/sumP;
}
