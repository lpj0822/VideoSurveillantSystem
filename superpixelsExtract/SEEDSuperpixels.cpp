#include "SEEDSuperpixels.h"
#include "seeds.h"

#include <iostream>

#define SEEDS_WIDTH 2
#define SEEDS_HEIGHT 2
#define NR_LEVELS 4

SEEDSuperpixels::SEEDSuperpixels()
{
    std::cout << "SEEDSuperpixels()" << std::endl;
}

SEEDSuperpixels::~SEEDSuperpixels()
{
    std::cout << "~SEEDSuperpixels()" << std::endl;
}

void SEEDSuperpixels::run(const cv::Mat &src)
{
    CV_Assert(!src.empty());
    CV_Assert(src.type() == CV_8UC3);

    const int NR_BINS = 5; // Number of bins in each histogram channel
    int width = src.cols;
    int height = src.rows;
    int count = width * height;

    SEEDS seeds(width, height, 3, NR_BINS);

    seeds.initialize(SEEDS_WIDTH, SEEDS_HEIGHT, NR_LEVELS);

    std::vector<unsigned int> ubuff(count);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            cv::Vec3b c = src.at<cv::Vec3b>(y, x);
            //image is assumed to have data in BGR order
            unsigned int b = c[0];
            unsigned int g = c[1];
            unsigned int r = c[2];

            ubuff[x + y * width] = b | (g << 8) | (r << 16);
        }
    }

    seeds.update_image_ycbcr(&ubuff[0]);
    seeds.iterate();

    unsigned int *slabels = seeds.get_labels();

    unsigned int maxElem = *std::max_element(slabels, slabels + count);

    std::vector<int> counts(maxElem + 1, 0);
    for (int i = 0; i < count; ++i)
        counts[slabels[i]]++;

    std::vector<int> deltas(maxElem + 1);
    int delta = 0;
    for (size_t i = 0; i < counts.size(); ++i)
    {
        if (counts[i] == 0)
            delta++;
        deltas[i] = delta;
    }

    for (int i = 0; i < count; ++i)
        slabels[i] -= deltas[slabels[i]];

    labels.create(height, width, CV_32SC1);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            labels.at<int>(y, x) = slabels[x + y * width];

    countSuperpixels = maxElem - deltas[maxElem] + 1;
}

cv::Mat SEEDSuperpixels::getLabels()
{
    return this->labels;
}
