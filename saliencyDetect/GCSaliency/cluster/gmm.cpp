#include "gmm.h"

void CmGMM::View()
{
    cv::Mat_<cv::Vec3f> color3f(1, _K);
    cv::Mat_<double> weight1d(1, _K), cov1d(1, _K);
    for (int i = 0; i < _K; i++)
    {
        color3f(0, i) = getMean(i);
        weight1d(0, i) = getWeight(i);
        cov1d(0, i) = sqrt(this->_Guassians[i].det);
    }
}


double CmGMM::ViewFrgBkgProb(const CmGMM &fGMM, const CmGMM &bGMM)
{
    double Pf = fGMM.GetSumWeight(), Pb = bGMM.GetSumWeight(), sumN = Pf + Pb;
    Pf /= sumN, Pb /= sumN;
    printf("#ForeSample = %.2g%%, ", Pf);
    int n = 6 * 6 * 6;
    cv::Mat color3f(1, n, CV_64FC3);
    cv::Vec3f* colors = (cv::Vec3f*)(color3f.data);
    for (int i = 0; i < n; i++)
    {
        colors[i][0] = ((float)(i / 36) + 0.5f)/6.f;
        int idx = i % 36;
        colors[i][1] = ((float)(idx / 6) + 0.5f)/6.f;
        colors[i][2] = ((float)(idx % 6) + 0.5f)/6.f;
    }

    cv::Mat fCount1d(1, n, CV_64F), bCount1d(1, n, CV_64F), rCount1d;
    double *fCount = (double*)(fCount1d.data), *bCount = (double*)(bCount1d.data);
    for (int i = 0; i < n; i++)
        fCount[i] = fGMM.P(colors[i]), bCount[i] = bGMM.P(colors[i]);
    double fMax, fMin, bMax, bMin;
    cv::minMaxLoc(fCount1d, &fMin, &fMax);
    cv::minMaxLoc(bCount1d, &bMin, &bMax);
    printf("minF = %.2g, maxF = %.2g, minB = %.2g maxB = %.2g\n", fMin, fMax, bMin, bMax);
    double epsilon = std::min(fMax, bMax) * 1e-4;
    cv::Mat res = (fCount1d*Pf + 0.5*epsilon)/(fCount1d*Pf + bCount1d*Pb + epsilon) - 0.5;
    return epsilon;
}


// show GMM images
void CmGMM::Show(cv::Mat& components1i)
{
    cv::Mat pShow(components1i.size(), CV_32FC3);
    for (int y = 0; y < components1i.rows; y++)
    {
        float* show = pShow.ptr<float>(y);
        const int* comp = components1i.ptr<int>(y);
        for (int x = 0; x < components1i.cols; x++)
        {
            CV_Assert(comp[x] >= 0 && comp[x] < _K);
            show[0] = (float)_Guassians[comp[x]].mean[0];
            show[1] = (float)_Guassians[comp[x]].mean[1];
            show[2] = (float)_Guassians[comp[x]].mean[2];
            show += 3;
        }
    }
}


void CmGMM::reWeights(std::vector<double> &mulWs)
{
    double sumW = 0;
    std::vector<double> newW(_K);
    for (int i = 0; i < _K; i++)
    {
        newW[i] = _Guassians[i].w * mulWs[i];
        sumW += newW[i];
    }
    for (int i = 0; i < _K; i++)
        _Guassians[i].w = newW[i] / sumW;
}


