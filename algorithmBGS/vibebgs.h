/** ViBe is a background subtraction algorithm, described in:
 *
 * Olivier Barnich and Marc Van Droogenbroeck,
 * ViBe: A Universal Background Subtraction Algorithm for Video Sequences,
 * IEEE Transactions on Image Processing, Vol. 20, No. 6, June 2011.
**/
#ifndef VIBEBGS_H
#define VIBEBGS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <cmath>

#include "IBGS.h"


class ViBeBGS: public IBGS
{
public:
    ViBeBGS();
    ~ViBeBGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    //初始化模型
    void initBg(const cv::Mat& image);
    //提取前景, 更新模型
    void update(const cv::Mat& image, cv::Mat& foregroundImage);
    void fill8UC3(const cv::Mat& image);
    void fill8UC1(const cv::Mat& image);
    void proc8UC3(const cv::Mat& image, cv::Mat& foreImage);
    void proc8UC1(const cv::Mat& image, cv::Mat& foreImage);

    void init();

private:
    int imageWidth;                         //处理图片的宽度
    int imageHeight;                        // 处理图片的高度
    int imageChannels;                      // 处理图片的通道数, 支持 1 和 3
    int imageType;                          // 处理图片的类型, 支持 CV_8UC1 和 CV_8UC3

    cv::Mat img_foreground;                 //前景
    cv::Mat samples;                        // 背景模型
    std::vector<uchar*> rowSamples;         // 样本的行首地址, 使用 vector 方便管理内存
    uchar** ptrSamples;                     // &rowSamples[0], 使用数组的下标而不是 vector 的 [] 运算符, 加快程序运行速度

    int numOfSamples;                       //每个像素保存的样本数量
    int minMatchDist;                       //判定前景背景的距离
    int minNumOfMatchCount;                 //判定为背景的最小匹配成功次数
    int subSampleInterval;                  //它的倒数等于更新保存像素值的概率
    bool showOutput;                        //是否显示

    bool firstTime;

private:
    void saveConfig();
    void loadConfig();
};

#endif // VIBEBGS_H
