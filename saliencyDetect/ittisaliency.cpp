#include "ittisaliency.h"
#include <iostream>

IttiSaliency::IttiSaliency()
{
    std::cout<<"IttiSaliency()"<<std::endl;
}

IttiSaliency::~IttiSaliency()
{
    std::cout<<"~IttiSaliency()"<<std::endl;
}

cv::Mat IttiSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels()==3);

    cv::Mat saliencyMap;
    //将原图插值为两倍
    cv::Size newSize;
    newSize.height = 2*src.rows;
    newSize.width =  2*src.cols;
    cv::Mat originx2=cv::Mat(newSize,src.type());
    cv::resize(src, originx2 , originx2.size(),0,0,cv::INTER_LINEAR);
    //初始化高斯金字塔尺寸
    for(int i = 0; i < PYRLEVEL; i++)
    {
        pyrSize[i].height = downsample(newSize.height, i);
        pyrSize[i].width = downsample(newSize.width, i);
    }
    //分离bgr分量
    std::vector<cv::Mat> rgb;
    cv::split(originx2,rgb);
    //计算亮度I和R、G、B、Y分量
    cv::Mat Intensity=cv::Mat(newSize,CV_64FC1);
    cv::Mat R=cv::Mat(newSize,CV_64FC1);
    cv::Mat G=cv::Mat(newSize,CV_64FC1);
    cv::Mat B=cv::Mat(newSize,CV_64FC1);
    cv::Mat Y=cv::Mat(newSize,CV_64FC1);

    for(int i=0; i < newSize.height; i++)
    {
        uchar* data_r=rgb[0].ptr<uchar>(i);
        uchar* data_g=rgb[1].ptr<uchar>(i);
        uchar* data_b=rgb[2].ptr<uchar>(i);
        double* data_Intensity=Intensity.ptr<double>(i);
        double* data_R=R.ptr<double>(i);
        double* data_G=G.ptr<double>(i);
        double* data_B=B.ptr<double>(i);
        double* data_Y=Y.ptr<double>(i);
        for(int j=0; j < newSize.width; j++)
        {
            data_Intensity[j]=(double)(data_r[j]+data_g[j]+data_b[j])/3;
            data_R[j]=data_r[j]-(double)(data_g[j]+data_b[j])/2;
            data_G[j]=data_g[j]-(double)(data_r[j]+data_b[j])/2;
            data_B[j]=data_b[j]-(double)(data_r[j]+data_g[j])/2;
            data_Y[j]=(double)(data_r[j]+data_g[j])/2-(double)abs(data_r[j]-data_g[j])/2-(double)data_b[j];
        }
    }

    //亮度特征提取
    //计算I金字塔
    GaussPyr Pyr_I;
    initPyr(&Pyr_I);
    Gscale(&Pyr_I, Intensity, PYRLEVEL, 1.6);

    //I的CS过程
    cv::Mat I_mean=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    CS_operation(&Pyr_I, &Pyr_I, I_mean);
    N_operation(I_mean, I_mean);

    //方向特征提取
    //计算O金字塔
    cv::Mat orient[4];
    for(int i = 0; i < 4; i++)
        orient[i] = cv::Mat(Intensity.size(),CV_64FC1);

    cvGabor(Intensity, orient[0], 11, 5, 0);
    cvGabor(Intensity, orient[1], 11, 5, CV_PI/4);
    cvGabor(Intensity, orient[2], 11, 5, CV_PI/2);
    cvGabor(Intensity, orient[3], 11, 5, 3*CV_PI/4);

    GaussPyr Pyr_0, Pyr_45, Pyr_90, Pyr_135;

    initPyr(&Pyr_0);
    initPyr(&Pyr_45);
    initPyr(&Pyr_90);
    initPyr(&Pyr_135);

    Gscale(&Pyr_0, orient[0], PYRLEVEL, 0.5);
    Gscale(&Pyr_45, orient[1], PYRLEVEL, 0.5);
    Gscale(&Pyr_90, orient[2], PYRLEVEL, 0.5);
    Gscale(&Pyr_135, orient[3], PYRLEVEL, 0.5);

    //O的CS过程
    cv::Mat O_mean=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    cv::Mat O_1=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    cv::Mat O_2=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    cv::Mat O_3=cv::Mat::zeros(pyrSize[4],CV_64FC1);

    CS_operation(&Pyr_0, &Pyr_0, O_mean);
    CS_operation(&Pyr_45, &Pyr_45, O_1);
    CS_operation(&Pyr_90, &Pyr_90, O_2);
    CS_operation(&Pyr_135, &Pyr_135, O_3);


    N_operation(O_mean, O_mean);
    N_operation(O_1, O_1);
    N_operation(O_2, O_2);
    N_operation(O_3, O_3);

    cv::add(O_mean, O_1, O_mean);
    cv::add(O_mean, O_2, O_mean);
    cv::add(O_mean, O_3, O_mean);

    N_operation(O_mean, O_mean);

    //颜色特征提取
    //计算RGBY金字塔
    GaussPyr Pyr_R, Pyr_G, Pyr_B, Pyr_Y;

    initPyr(&Pyr_R);
    initPyr(&Pyr_G);
    initPyr(&Pyr_B);
    initPyr(&Pyr_Y);

    Gscale(&Pyr_R, R, PYRLEVEL, 0.5);
    Gscale(&Pyr_G, G, PYRLEVEL, 0.5);
    Gscale(&Pyr_B, B, PYRLEVEL, 0.5);
    Gscale(&Pyr_Y, Y, PYRLEVEL, 0.5);

    //计算RG—BY金字塔
    GaussPyr Pyr_RG, Pyr_GR, Pyr_BY, Pyr_YB;

    initPyr(&Pyr_RG);
    initPyr(&Pyr_GR);
    initPyr(&Pyr_BY);
    initPyr(&Pyr_YB);

    PyrSub(&Pyr_R, &Pyr_G, &Pyr_RG);
    PyrSub(&Pyr_G, &Pyr_R, &Pyr_GR);
    PyrSub(&Pyr_B, &Pyr_Y, &Pyr_BY);
    PyrSub(&Pyr_Y, &Pyr_B, &Pyr_YB);

    //C的CS过程
    cv::Mat C_mean=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    cv::Mat C_1=cv::Mat::zeros(pyrSize[4],CV_64FC1);

    CS_operation(&Pyr_RG, &Pyr_GR, C_mean);
    CS_operation(&Pyr_BY, &Pyr_YB, C_1);
    cv::add(C_mean,C_1,C_mean);

    N_operation(C_mean, C_mean);

    //整合所有特征，生成显著性图
    cv::Mat all=cv::Mat::zeros(pyrSize[4],CV_64FC1);
    cv::add(I_mean,C_mean,all);
    cv::add(all,O_mean,saliencyMap);

    saliencyMap.convertTo(saliencyMap,saliencyMap.type(),0.33333,0);
    cv::normalize(saliencyMap,saliencyMap,1.0, 0.0, cv::NORM_MINMAX);
    cv::resize(saliencyMap,saliencyMap,src.size(), 0,0, cv::INTER_LINEAR);
    return saliencyMap;
}

//初始化金字塔结构体
void IttiSaliency::initPyr(GaussPyr *pyr)
{
    for(int i = 0; i < PYRLEVEL; i++)
    {
        pyr->lev[i].create(pyrSize[i],CV_64FC1);
    }
}


//根据层数，求第i层的尺寸
int IttiSaliency::downsample(int x, int level)
{
    while(level>0)
    {
        if(x%2==0)
            x=x/2;
        else
            x=(x+1)/2;
        level--;
    }
    return x;
}


//计算并产生一幅图的高斯金字塔
void IttiSaliency::Gscale(GaussPyr *pyr, cv::Mat data, int level, double sigma)
{
    for(int i = 0; i<level; i++)
    {
        if(i == 0)
        {
            cv::GaussianBlur(data,pyr->lev[0],cv::Size(5,5),sigma,sigma,0);
        }
        else
        {
            cv::pyrDown(pyr->lev[i-1],pyr->lev[i],pyrSize[i]);
        }
    }
}


//c-s过程中用到的跨尺度相减
void IttiSaliency::overScaleSub(cv::Mat s1, cv::Mat s2, cv::Mat &dst)
{
    cv::resize(s2,dst,dst.size());
    cv::absdiff(s1, dst, dst);
}


//求图像的局部最大值
void IttiSaliency::getLocalMaxima(cv::Mat src, double thresh, double *lm_sum, int *lm_num, double*lm_avg)
{
    *lm_sum = 0.0;
    *lm_num = 0;
    *lm_avg = 0.0;
    int count = 0;

    for(int a = 1; a < (src.rows - 1); a++)
    {
        double* data1=src.ptr<double>(a-1);
        double* data2=src.ptr<double>(a);
        double* data3=src.ptr<double>(a+1);
        for(int b = 1; b< (src.cols - 1); b++)
        {
            double val = data2[b];
            if((val >= thresh) &&
                (val >= data1[b]) &&
                (val >= data3[b]) &&
                (val >= data2[b-1]) &&
                (val >= data2[b+1]))
            {
                if(val == 10)
                    count++;
                *lm_sum += val;
                (*lm_num) ++;
            }
        }
    }

    if(*lm_num > count)
    {
        *lm_sum = *lm_sum - 10*count;
        *lm_num = *lm_num - count;
        if(*lm_num > 0)
            *lm_avg = *lm_sum / *lm_num;
        else
            *lm_avg = 0;
    }
    else
        *lm_avg = 0;
}


//N操作，包括下采样到第5层的尺寸
void IttiSaliency::N_operation(cv::Mat src, cv::Mat &dst)
{
    int lm_num=0;
    double lm_sum=0.0;
    double lm_avg=0.0;
    if(src.empty()||dst.empty())
    {
        return;
    }
    cv::normalize(src,src,1.0,0.0,cv::NORM_MINMAX);
    src.convertTo(src,src.type(),10,0);
    getLocalMaxima(src ,1, &lm_sum, &lm_num, &lm_avg);
    if(lm_num > 0)
        src.convertTo(src,src.type(),(10-lm_avg)*(10-lm_avg), 0);


    //下采样，先判断目前的层数
    if( src.rows == pyrSize[2].height )//scr在第三层
    {
        for(int a = 0; a < pyrSize[4].height; a++)
        {
            double* dataDst=dst.ptr<double>(a);
            double* dataSrc=src.ptr<double>(4*a);
            for(int b = 0; b < pyrSize[4].width; b++)
            {
                dataDst[b] = dataSrc[4*b];
            }
        }
    }
    else if( src.rows == pyrSize[3].height )//scr在第四层
    {
        for(int a = 0; a < pyrSize[4].height; a++)
        {
            double* dataDst=dst.ptr<double>(a);
            double* dataSrc=src.ptr<double>(2*a);
            for(int b = 0; b < pyrSize[4].width; b++)
            {
                dataDst[b] = dataSrc[2*b];
            }
        }
    }
    else //scr在第五层
    {
        for(int a = 0; a < pyrSize[4].height; a++)
        {
            double* dataDst=dst.ptr<double>(a);
            double* dataSrc=src.ptr<double>(a);
            for(int b = 0; b < pyrSize[4].width; b++)
            {
                dataDst[b] = dataSrc[b];
            }
        }
    }
}


//C_S过程，金字塔最底层为第0层(实际上（c,s）=(3,6),(3,7),(4,7),(4,8),(5,8),(5,9),)
void IttiSaliency::CS_operation(GaussPyr *p1, GaussPyr *p2, cv::Mat &dst)
{
    for(int c = 2; c < 5; c++)
    {
        for(int delta = 3, s = c + delta; delta < 5; delta++, s = c + delta)
        {
            cv::Mat tempMat=cv::Mat(pyrSize[c],CV_64FC1);
            overScaleSub(p1->lev[c], p2->lev[s], tempMat);
            cv::Mat temp5Lev=cv::Mat(pyrSize[4],CV_64FC1);
            N_operation(tempMat, temp5Lev);
            cv::add(temp5Lev,dst,dst);
        }
    }
}


//金字塔相减，在求颜色特征时使用
void IttiSaliency::PyrSub(GaussPyr *s1, GaussPyr *s2, GaussPyr *dst)
{
    for(int i = 0; i<PYRLEVEL; i++)
    {
        cv::subtract(s1->lev[i], s2->lev[i], dst->lev[i]);
    }
}


//计算Gabor滤波
void IttiSaliency::cvGabor(cv::Mat src, cv::Mat &dst, int width, double lamda, double theta)
{
    cv::Mat gaborKernel=cv::Mat(width,width,CV_32FC1);
    double tmp1,tmp2,xtmp,ytmp,re;
    int i,j,x,y;
    for(i = 0; i < width; i++)
    {
        float* data=gaborKernel.ptr<float>(i);
        for(j = 0; j < width; j++)
        {
            x = (i*16/(width - 1)) - 8;
            y = (j*16/(width - 1)) - 8;
            xtmp = (float)x*cos(theta) + (float)y*sin(theta);
            ytmp = (float)(-x)*sin(theta) + (float)y*cos(theta);

            tmp1 = (1/(CV_PI*pow(lamda,2)))*exp(-((pow(xtmp,2) + pow(ytmp,2))/pow(lamda,2)));
            tmp2 = cos(2*CV_PI*xtmp/lamda);
            re   = tmp1*tmp2;
            data[j]=(float)re;
        }
    }

    cv::filter2D(src,dst,src.depth(),gaborKernel);
    cv::abs(dst);
    double max = 0;
    double min= 0;
    cv::minMaxLoc(dst,&min,&max);
    dst.convertTo(dst,dst.type(),1/max,0);
}
