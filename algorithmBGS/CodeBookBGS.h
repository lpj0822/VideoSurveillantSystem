/** CodeBook is a background subtraction algorithm, described in:
 *
 * Kim,
 * Real-time foreground–background segmentation using codebook model,
 * Real-Time Imaging, 2005.
**/
#ifndef CODEBOOKBGS_H
#define CODEBOOKBGS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "IBGS.h"

#define CHANNELS 3 // 设置处理的图像通道数,要求小于等于图像本身的通道数

// 下面为码本码元的数据结构
// 处理图像时每个像素对应一个码本,每个码本中可有若干个码元
typedef struct ce {
    uchar learnHigh[CHANNELS];	// High side threshold for learning
    // 此码元各通道的阀值上限(学习界限)
    uchar learnLow[CHANNELS];		// Low side threshold for learning
    // 此码元各通道的阀值下限
    // 学习过程中如果一个新像素各通道值x[i],均有 learnLow[i]<=x[i]<=learnHigh[i],则该像素可合并于此码元
    uchar max[CHANNELS];			// High side of box boundary
    // 属于此码元的像素中各通道的最大值
    uchar min[CHANNELS];			// Low side of box boundary
    // 属于此码元的像素中各通道的最小值
    int	t_last_update;			// This is book keeping to allow us to kill stale entries
    // 此码元最后一次更新的时间,每一帧为一个单位时间,用于计算stale
    int	stale;					// max negative run (biggest period of inactivity)
    // 此码元最长不更新时间,用于删除规定时间不更新的码元,精简码本
} code_element;						// 码元的数据结构

typedef struct code_book {
    code_element **cb;
    // 码元的二维指针,理解为指向码元指针数组的指针,使得添加码元时不需要来回复制码元,只需要简单的指针赋值即可
    int	numEntries;
    // 此码本中码元的数目
    int	t;				// count every access
    // 此码本现在的时间,一帧为一个时间单位
} CodeBook;							// 码本的数据结构

class CodeBookBGS : public IBGS
{
public:
    CodeBookBGS();
    ~CodeBookBGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    bool firstTime;
    cv::Mat img_foreground;

    CodeBook* cB;
    unsigned int cbBounds[CHANNELS];
    int imageLen;
    int	nChannels;
    int	minMod[CHANNELS];
    int	maxMod[CHANNELS];
    int historyNumber;

    int historyCount;//历史背景学习帧数
    int channelsThreshold;// 用于确定码元各通道的阀值
    int minLengthChannels;// 用于背景差分函数中,调整其值以达到最好的分割
    int maxLengthChannels;
    bool showOutput;

private:
    int updateCodeBook(uchar *p, CodeBook &c, unsigned int *cbBounds, int numChannels);//更新码本模型
    uchar backgroundDiff(uchar *p, CodeBook &c, int numChannels, int *minMod, int *maxMod);//根据码本模型决定此像素是前景还是背景
    int clearStaleEntries(CodeBook &c);//定期清除陈旧的码本

    void init();
    void saveConfig();
    void loadConfig();
};
#endif //CODEBOOKBGS_H
