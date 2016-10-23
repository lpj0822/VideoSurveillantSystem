#include "rtcprgb2gray.h"
#include <iostream>

RTCPrgb2gray::RTCPrgb2gray()
{
    std::cout<<"RTCPrgb2gray()"<<std::endl;
}

RTCPrgb2gray::~RTCPrgb2gray()
{
    std::cout<<"~RTCPrgb2gray()"<<std::endl;
}

cv::Mat RTCPrgb2gray::BGR2Gray(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3);

    float s = 64.f / sqrt((float)(src.rows * src.cols));
    int cols = cvRound(s * src.cols);
    int rows = cvRound(s * src.rows);

    const float sigma = 0.05f;
    std::vector<cv::Vec3f> W = weight();

    std::vector<cv::Vec3f> P;
    std::vector<float> det;

    std::vector<cv::Point> pos0;
    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            int x = (int)((i + 0.5f) * src.cols / cols);
            int y = (int)((j + 0.5f) * src.rows / rows);
            pos0.push_back(cv::Point(x, y));
        }
    }

    std::vector<cv::Point> pos1 = pos0;
    std::random_shuffle(pos1.begin(), pos1.end());

    for (std::size_t i = 0; i < pos0.size(); ++i)
    {
        cv::Vec3b c0 = src.at<cv::Vec3b>(pos0[i].y, pos0[i].x);
        cv::Vec3b c1 = src.at<cv::Vec3b>(pos1[i].y, pos1[i].x);

        add(c0, c1, P, det);
    }

    cols /= 2;
    rows /= 2;

    for (int i = 0; i < cols - 1; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            int x0 = (int)((i + 0.5f) * src.cols / cols);
            int x1 = (int)((i + 1.5f) * src.cols / cols);
            int y = (int)((j + 0.5f) * src.rows / rows);

            cv::Vec3b c0 = src.at<cv::Vec3b>(y, x0);
            cv::Vec3b c1 = src.at<cv::Vec3b>(y, x1);

            add(c0, c1, P, det);
        }
    }

    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows - 1; ++j)
        {
            int x = (int)((i + 0.5f) * src.cols / cols);
            int y0 = (int)((j + 0.5f) * src.rows / rows);
            int y1 = (int)((j + 1.5f) * src.rows / rows);

            cv::Vec3b c0 = src.at<cv::Vec3b>(y0, x);
            cv::Vec3b c1 = src.at<cv::Vec3b>(y1, x);

            add(c0, c1, P, det);
        }
    }

    float maxEs = -FLT_MAX;
    int bw=0;
    for (std::size_t i = 0; i < W.size(); ++i)
    {
        float Es = 0;
        for (std::size_t j = 0; j < P.size(); ++j)
        {
            float L = P[j][0] * W[i][0] + P[j][1] * W[i][1] + P[j][2] * W[i][2];
            float detM = det[j];

            float a = (L + detM) / sigma;
            float b = (L - detM) / sigma;

            Es += log(exp(-a * a) + exp(-b * b));
        }
        Es /= P.size();

        if (Es > maxEs)
        {
            maxEs = Es;
            bw = (int)i;
        }
    }

    std::vector<cv::Mat> c;
    cv::split(src, c);

    cv::Mat result;
    cv::addWeighted(c[2], W[bw][0], c[1], W[bw][1], 0.0, result);
    cv::addWeighted(c[0], W[bw][2], result, 1.0, 0.0, result);

    return result;
}

std::vector<cv::Vec3f> RTCPrgb2gray::weight()
{
    std::vector<cv::Vec3f> W;

    for (int i = 0; i <= 10; ++i)
	{
        for (int j = 0; j <= (10 - i); ++j)
        {
            int k = 10 - (i + j);
            W.push_back(cv::Vec3f(i/10.f, j/10.f, k/10.f));
        }
	}

    return W;
}

void RTCPrgb2gray::add(const cv::Vec3b &c0, const cv::Vec3b &c1, std::vector<cv::Vec3f> &P, std::vector<float> &det)
{
    float Rx = c0[2]/255.f - c1[2]/255.f;
    float Gx = c0[1]/255.f - c1[1]/255.f;
    float Bx = c0[0]/255.f - c1[0]/255.f;

    float d = sqrt(Rx * Rx + Gx * Gx + Bx * Bx) / 1.41f;

    if (d >= 0.05f)
    {
        P.push_back(cv::Vec3f(Rx, Gx, Bx));
        det.push_back(d);
    }
}
