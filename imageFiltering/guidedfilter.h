/** GuidedFilter algorithm, described in:
 *
 * Kaiming He, Member, IEEE, Jian Sun, Member, IEEE, and Xiaoou Tang, Fellow, IEEE,
 * Guided Image Filtering,
 * IEEE transactions on pattern analysis and machine intelligence, VOL. 35, NO. 6, June 2013.
**/
#ifndef GUIDEDFILTER_H
#define GUIDEDFILTER_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

class GuidedFilterImpl
{
public:
    GuidedFilterImpl() {}
    virtual ~GuidedFilterImpl() {}

    cv::Mat filter(const cv::Mat &p, int depth);

protected:
    int Idepth;

private:
    virtual cv::Mat filterSingleChannel(const cv::Mat &p) const = 0;
};

class GuidedFilterMono : public GuidedFilterImpl
{
public:
    GuidedFilterMono(const cv::Mat &I, int radius, double eps);

private:
    virtual cv::Mat filterSingleChannel(const cv::Mat &p) const;

private:
    int radius;
    double eps;
    cv::Mat I, mean_I, var_I;
};

class GuidedFilterColor : public GuidedFilterImpl
{
public:
    GuidedFilterColor(const cv::Mat &I, int radius, double eps);

private:
    virtual cv::Mat filterSingleChannel(const cv::Mat &p) const;

private:
    std::vector<cv::Mat> Ichannels;
    int radius;
    double eps;
    cv::Mat mean_I_r, mean_I_g, mean_I_b;
    cv::Mat invrr, invrg, invrb, invgg, invgb, invbb;
};

class GuidedFilter
{
public:
    GuidedFilter(const cv::Mat &I, int radius, double eps);
    ~GuidedFilter();

    cv::Mat filter(const cv::Mat &p, int depth = -1) const;

private:
    GuidedFilterImpl *impl_;
};

#endif // GUIDEDFILTER_H
