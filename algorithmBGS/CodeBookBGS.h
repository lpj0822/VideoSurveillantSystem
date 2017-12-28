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

struct codeword
{
    float min;
    float max;
    float f;
    float l;
    int first;
    int last;
    bool isStale;
};

class CodeBookBGS : public IBGS
{
public:
    CodeBookBGS();
    ~CodeBookBGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    const int DEFAULT_ALPHA = 10;
    const float DEFAULT_BETA = 1.0f;
    const int DEFAULT_LEARNFRAMES = 10;

private:
    int t;
    int learningFrames;
    int alpha;
    float beta;

    bool firstTime;
    bool showOutput;

    std::vector<codeword> **cbMain;
    std::vector<codeword> **cbCache;

    cv::Mat img_foreground;

private:
    void initializeCodebook(int w, int h);
    void update_cb(const cv::Mat& frame);
    void fg_cb(const cv::Mat& frame, cv::Mat& fg);

private:
    void init();
    void saveConfig();
    void loadConfig();
};
#endif //CODEBOOKBGS_H
