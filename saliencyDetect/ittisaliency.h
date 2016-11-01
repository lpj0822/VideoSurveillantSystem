/*
 * L. Itti, C. Koch, E. Niebur,
 * A Model of Saliency-Based Visual Attention for Rapid Scene Analysis,
 * IEEE Transactions on Pattern Analysis and Machine Intelligence,
 * Vol. 20, No. 11, pp. 1254-1259, Nov 1998.
 */
#ifndef ITTISALIENCY_H
#define ITTISALIENCY_H

#include <opencv2/imgproc.hpp>

#include "isaliency.h"

#define PYRLEVEL 9

//高斯金字塔结构体
typedef struct GaussPyr
{
    cv::Mat lev[PYRLEVEL];
}GaussPyr;

class IttiSaliency: public ISaliency
{
public:
    IttiSaliency();
    ~IttiSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

private:
    //初始化金字塔结构体
    void initPyr(GaussPyr *pyr);
    //根据层数，求第i层的尺寸
    int downsample(int x, int level);
    //计算并产生一幅图的高斯金字塔
    void Gscale(GaussPyr *pyr, cv::Mat data, int level, double sigma);
    //c-s过程中用到的跨尺度相减
    void overScaleSub(cv::Mat s1, cv::Mat s2, cv::Mat &dst);
    //求图像的局部最大值
    void getLocalMaxima(cv::Mat src, double thresh, double *lm_sum, int *lm_num, double*lm_avg);
    //N操作，包括下采样到第5层的尺寸
    void N_operation(cv::Mat src, cv::Mat &dst);
    //C_S过程，金字塔最底层为第0层(实际上（c,s）=(3,6),(3,7),(4,7),(4,8),(5,8),(5,9),)
    void CS_operation(GaussPyr *p1, GaussPyr *p2, cv::Mat &dst);
    //金字塔相减，在求颜色特征时使用
    void PyrSub(GaussPyr *s1, GaussPyr *s2, GaussPyr *dst);
    //计算Gabor滤波
    void cvGabor(cv::Mat src, cv::Mat &dst, int width, double lamda, double theta);

private:
    //金字塔尺寸
    cv::Size pyrSize[PYRLEVEL];

};

#endif // ITTISALIENCY_H
