#ifndef PIXELBASEDADAPTIVESEGMENTER_H
#define PIXELBASEDADAPTIVESEGMENTER_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "PBAS/PBAS.h"
#include "IBGS.h"

class PixelBasedAdaptiveSegmenter : public IBGS
{
public:
    PixelBasedAdaptiveSegmenter();
    ~PixelBasedAdaptiveSegmenter();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    PBAS pbas;
    cv::Mat img_foreground;

    bool firstTime;
    bool showOutput;
	
	bool enableInputBlur;
    bool enableOutputBlur;

    float alpha;
    float beta;
    int N;
    int Raute_min;
    float R_incdec;
    int R_lower;
    int R_scale;
    float T_dec;
    int T_inc;
    int T_init;
    int T_lower;
    int T_upper;

private:
    void init();
    void saveConfig();
    void loadConfig();
};
#endif //PIXELBASEDADAPTIVESEGMENTER_H
