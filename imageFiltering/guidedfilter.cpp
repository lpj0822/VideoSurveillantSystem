#include "guidedfilter.h"
#include <iostream>

cv::Mat GuidedFilterImpl::filter(const cv::Mat &p, int depth)
{
    cv::Mat p2;
    p.convertTo(p2, Idepth);

    cv::Mat tempResult;
    cv::Mat result;
    if (p.channels() == 1)
    {
        tempResult = filterSingleChannel(p2);
    }
    else
    {
        std::vector<cv::Mat> pc;
        cv::split(p2, pc);

        for (std::size_t i = 0; i < pc.size(); ++i)
            pc[i] = filterSingleChannel(pc[i]);

        cv::merge(pc, tempResult);
    }
    tempResult.convertTo(result,depth == -1 ? p.depth() : depth);
    return result;
}

GuidedFilterMono::GuidedFilterMono(const cv::Mat &origI, int radius, double eps) : radius(radius), eps(eps)
{
    if (origI.depth() == CV_32F || origI.depth() == CV_64F)
        I = origI.clone();
    else
        origI.convertTo(I,CV_32F);

    Idepth = I.depth();

    cv::Mat mean_II;
    cv::blur(I,mean_I,cv::Size(radius,radius));
    cv::blur(I.mul(I),mean_II,cv::Size(radius,radius));
    var_I = mean_II - mean_I.mul(mean_I);
}

cv::Mat GuidedFilterMono::filterSingleChannel(const cv::Mat &p) const
{
    cv::Mat q;
    cv::Mat mean_p;
    cv::Mat mean_Ip;
    cv::Mat cov_Ip;
    cv::blur(p,mean_p,cv::Size(radius,radius));
    cv::blur(I.mul(p),mean_Ip,cv::Size(radius,radius));
    cov_Ip = mean_Ip - mean_I.mul(mean_p); // this is the covariance of (I, p) in each local patch.

    cv::Mat a = cov_Ip / (var_I + eps); // Eqn. (5) in the paper;
    cv::Mat b = mean_p - a.mul(mean_I); // Eqn. (6) in the paper;

    cv::Mat mean_a;
    cv::Mat mean_b;
    cv::blur(a,mean_a,cv::Size(radius,radius));
    cv::blur(b,mean_b,cv::Size(radius,radius));

    q=mean_a.mul(I) + mean_b;

    return q;
}

GuidedFilterColor::GuidedFilterColor(const cv::Mat &origI, int radius, double eps) : radius(radius), eps(eps)
{
    cv::Mat I;
    cv::Mat mean_I_rr;
    cv::Mat mean_I_rg;
    cv::Mat mean_I_rb;
    cv::Mat mean_I_gg;
    cv::Mat mean_I_gb;
    cv::Mat mean_I_bb;
    if (origI.depth() == CV_32F || origI.depth() == CV_64F)
        I = origI.clone();
    else
        origI.convertTo(I,CV_32F);

    Idepth = I.depth();

    cv::split(I, Ichannels);

    cv::blur(Ichannels[0],mean_I_b,cv::Size(radius,radius));
    cv::blur(Ichannels[1],mean_I_g,cv::Size(radius,radius));
    cv::blur(Ichannels[2],mean_I_r,cv::Size(radius,radius));

    cv::blur(Ichannels[0].mul(Ichannels[0]),mean_I_bb,cv::Size(radius,radius));
    cv::blur(Ichannels[1].mul(Ichannels[1]),mean_I_gg,cv::Size(radius,radius));
    cv::blur(Ichannels[2].mul(Ichannels[2]),mean_I_rr,cv::Size(radius,radius));
    cv::blur(Ichannels[2].mul(Ichannels[1]),mean_I_rg,cv::Size(radius,radius));
    cv::blur(Ichannels[2].mul(Ichannels[0]),mean_I_rb,cv::Size(radius,radius));
    cv::blur(Ichannels[1].mul(Ichannels[0]),mean_I_gb,cv::Size(radius,radius));

    // variance of I in each local patch: the matrix Sigma in Eqn (14).
    // Note the variance in each local patch is a 3x3 symmetric matrix:
    //           rr, rg, rb
    //   Sigma = rg, gg, gb
    //           rb, gb, bb
    cv::Mat var_I_rr = mean_I_rr - mean_I_r.mul(mean_I_r) + eps;
    cv::Mat var_I_rg = mean_I_rg - mean_I_r.mul(mean_I_g);
    cv::Mat var_I_rb = mean_I_rb - mean_I_r.mul(mean_I_b);
    cv::Mat var_I_gg = mean_I_gg - mean_I_g.mul(mean_I_g) + eps;
    cv::Mat var_I_gb = mean_I_gb - mean_I_g.mul(mean_I_b);
    cv::Mat var_I_bb = mean_I_bb - mean_I_b.mul(mean_I_b) + eps;

    // Inverse of Sigma + eps * I
    invrr = var_I_gg.mul(var_I_bb) - var_I_gb.mul(var_I_gb);
    invrg = var_I_gb.mul(var_I_rb) - var_I_rg.mul(var_I_bb);
    invrb = var_I_rg.mul(var_I_gb) - var_I_gg.mul(var_I_rb);
    invgg = var_I_rr.mul(var_I_bb) - var_I_rb.mul(var_I_rb);
    invgb = var_I_rb.mul(var_I_rg) - var_I_rr.mul(var_I_gb);
    invbb = var_I_rr.mul(var_I_gg) - var_I_rg.mul(var_I_rg);

    cv::Mat covDet = invrr.mul(var_I_rr) + invrg.mul(var_I_rg) + invrb.mul(var_I_rb);

    invrr /= covDet;
    invrg /= covDet;
    invrb /= covDet;
    invgg /= covDet;
    invgb /= covDet;
    invbb /= covDet;
}

cv::Mat GuidedFilterColor::filterSingleChannel(const cv::Mat &p) const
{
    cv::Mat q;
    cv::Mat mean_p;
    cv::blur(p,mean_p,cv::Size(radius,radius));

    cv::Mat mean_Ip_r;
    cv::Mat mean_Ip_g;
    cv::Mat mean_Ip_b;
    cv::blur(Ichannels[2].mul(p),mean_Ip_r,cv::Size(radius,radius));
    cv::blur(Ichannels[1].mul(p),mean_Ip_g,cv::Size(radius,radius));
    cv::blur(Ichannels[0].mul(p),mean_Ip_b,cv::Size(radius,radius));

    // covariance of (I, p) in each local patch.
    cv::Mat cov_Ip_r = mean_Ip_r - mean_I_r.mul(mean_p);
    cv::Mat cov_Ip_g = mean_Ip_g - mean_I_g.mul(mean_p);
    cv::Mat cov_Ip_b = mean_Ip_b - mean_I_b.mul(mean_p);

    cv::Mat a_r = invrr.mul(cov_Ip_r) + invrg.mul(cov_Ip_g) + invrb.mul(cov_Ip_b);
    cv::Mat a_g = invrg.mul(cov_Ip_r) + invgg.mul(cov_Ip_g) + invgb.mul(cov_Ip_b);
    cv::Mat a_b = invrb.mul(cov_Ip_r) + invgb.mul(cov_Ip_g) + invbb.mul(cov_Ip_b);

    cv::Mat b = mean_p - a_r.mul(mean_I_r) - a_g.mul(mean_I_g) - a_b.mul(mean_I_b); // Eqn. (15) in the paper;

    cv::Mat mean_a_r;
    cv::Mat mean_a_g;
    cv::Mat mean_a_b;
    cv::Mat mean_b;
    cv::blur(a_r,mean_a_r,cv::Size(radius,radius));
    cv::blur(a_g,mean_a_g,cv::Size(radius,radius));
    cv::blur(a_b,mean_a_b,cv::Size(radius,radius));
    cv::blur(b,mean_b,cv::Size(radius,radius));

    // Eqn. (16) in the paper;
    q=mean_a_r.mul(Ichannels[2])+mean_a_g.mul(Ichannels[1])+mean_a_b.mul(Ichannels[0])+mean_b;

    return q;
}

GuidedFilter::GuidedFilter(const cv::Mat &I, int radius, double eps)
{
    CV_Assert(I.channels() == 1 || I.channels() == 3);

    if (I.channels() == 1)
    {
        impl_ = new GuidedFilterMono(I, 2 * radius + 1, eps);
    }
    else
    {
        impl_ = new GuidedFilterColor(I, 2 * radius + 1, eps);
    }

    std::cout<<"GuidedFilter()"<<std::endl;
}

GuidedFilter::~GuidedFilter()
{
    if(impl_)
    {
        delete impl_;
        impl_=NULL;
    }
    std::cout<<"~GuidedFilter()"<<std::endl;
}

cv::Mat GuidedFilter::filter(const cv::Mat &p, int depth) const
{
    return impl_->filter(p, depth);
}
