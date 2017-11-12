#ifndef SEEDSUPERPIXELS_H
#define SEEDSUPERPIXELS_H

#include "isuperpixelsextract.h"

class SEEDSuperpixels : public ISuperpixelsExtract
{
public:
    SEEDSuperpixels();
    ~SEEDSuperpixels();

    void run(const cv::Mat &src);
    cv::Mat getLabels();

private:

    cv::Mat labels;
    int countSuperpixels;
};

#endif // SEEDSUPERPIXELS_H
