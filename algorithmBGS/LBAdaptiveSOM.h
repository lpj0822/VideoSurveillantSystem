#ifndef LBADAPTIVESOM_H
#define LBADAPTIVESOM_H

#include "lb/BGModelSom.h"
#include "IBGS.h"

using namespace lb_library;
using namespace lb_library::AdaptiveSOM;

class LBAdaptiveSOM : public IBGS
{
public:
    LBAdaptiveSOM();
    ~LBAdaptiveSOM();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:

    BGModel* m_pBGModel;

    int sensitivity;
    int trainingSensitivity;
    int learningRate;
    int trainingLearningRate;
    int trainingSteps;

    bool showOutput;
    bool firstTime;

    cv::Mat img_background;
    cv::Mat img_foreground;

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // LBADAPTIVESOM_H
