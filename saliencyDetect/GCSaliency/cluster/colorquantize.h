#ifndef COLORQUANTIZE_H
#define COLORQUANTIZE_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

#define S_Q_NUM 3
#define Q_NUM 4

typedef int (*S_QUANTIZE_FUNC)(const cv::Vec3f &c);
typedef void (*S_RECOVER_FUNC)(int idx, cv::Vec3f &c);

class ColorQuantize
{
public:
    ColorQuantize();
    ~ColorQuantize();

    enum {S_BGR, S_HSV, S_LAB, D_BGR};
    static const int binNum[Q_NUM];
    static const S_QUANTIZE_FUNC sqFuns[S_Q_NUM];
    static const S_RECOVER_FUNC srFuns[S_Q_NUM];

    // Static quantization and recover without prior color statistics, img3f: BGR image
    void S_Quantize(cv::Mat& img3f, cv::Mat &idx1i, int method = S_BGR);
    void S_Recover(cv::Mat& idx1i, cv::Mat& img3f, int method = 1, cv::Mat &src3f = cv::Mat()); // img3f and src3f are BGR.

    int D_Quantize(cv::Mat& img3f, cv::Mat &idx1i, cv::Mat &_color3f, cv::Mat &_colorNum1i, double ratio = 0.95, const int colorNums[3] = DefaultNums);
    void D_Recover(cv::Mat& idx1i, cv::Mat &img3f, cv::Mat &color3f);

    // src3f are BGR, color3f are 1xBinDim matrix represent color fore each histogram bin
    int S_BinInf(cv::Mat& idx1i, cv::Mat &color3f, std::vector<int> &colorNum, int method = 1, cv::Mat &src3f = cv::Mat());

    // Static quantization and recover without prior color statistics
    static int SQ_BGR(const cv::Vec3f &c);		// B,G,R[0,1] --> [0, 215]
    static void SR_BGR(int idx, cv::Vec3f &c);	// [0,215] --> B,G,R[0,1]
    static int SQ_HSV(const cv::Vec3f &c);		// H[0,360], S,V[0,1] ---> [0, 255]
    static void SR_HSV(int idx, cv::Vec3f &c); // [0,255] ---> H[0,360], S,V[0,1]
    static int SQ_Lab(const cv::Vec3f &c);		// L[0, 100], a, b [-127, 127] ---> [0, 244]
    static void SR_Lab(int idx, cv::Vec3f &c); // [0, 244] ---> L[0, 100], a, b [-127, 127]

private:
    static const int DefaultNums[3];
};

#endif // COLORQUANTIZE_H
