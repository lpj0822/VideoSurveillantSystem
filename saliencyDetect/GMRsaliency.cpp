#include "GMRsaliency.h"

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <limits>
#include "superpixelsExtract/SLICSuperpixels.h"

GMRsaliency::GMRsaliency()
{
    alpha = 0.99f;
    delta = 0.1f;
    spcounta = 225;
    std::cout << "GMRsaliency()" << std::endl;
}

GMRsaliency::~GMRsaliency()
{
    std::cout << "~GMRsaliency()" << std::endl;
}

cv::Mat GMRsaliency::getSaliency(const cv::Mat &src)
{
    cv::Mat copySrc;
    int wcut[6];
    copySrc = removeFrame(src, wcut);

    cv::Mat suplabel(src.size(),CV_16U);
    suplabel = getSuperpixels(copySrc);

    cv::Mat adj(cv::Size(spcounta,spcounta),CV_16U);
    adj=getAdjLoop(suplabel);

    cv::Mat tImg;
    copySrc.convertTo(tImg, CV_32FC3, 1.0/255);
    cv::cvtColor(tImg, tImg, cv::COLOR_BGR2Lab);

    cv::Mat W(adj.size(),CV_32F);
    W=getWeight(tImg,suplabel,adj);

    cv::Mat optAff(W.size(),CV_32F);
    optAff=getOptAff(W);

    cv::Mat salt,sald,sall,salr;
    cv::Mat bdy;

    bdy=getBdQuery(suplabel,1);
    salt=optAff*bdy;
    cv::normalize(salt, salt, 0, 1, cv::NORM_MINMAX);
    salt=1-salt;

    bdy=getBdQuery(suplabel,2);
    sald=optAff*bdy;
    cv::normalize(sald, sald, 0, 1, cv::NORM_MINMAX);
    sald=1-sald;

    bdy=getBdQuery(suplabel,3);
    sall=optAff*bdy;
    cv::normalize(sall, sall, 0, 1, cv::NORM_MINMAX);
    sall=1-sall;

    bdy=getBdQuery(suplabel,4);
    salr=optAff*bdy;
    cv::normalize(salr, salr, 0, 1, cv::NORM_MINMAX);
    salr=1-salr;

    cv::Mat salb;
    salb=salt;
    salb=salb.mul(sald);
    salb=salb.mul(sall);
    salb=salb.mul(salr);

    double thr=mean(salb)[0];
    cv::Mat fgy;
    cv::threshold(salb,fgy,thr,1, cv::THRESH_BINARY);

    cv::Mat salf;
    salf=optAff*fgy;

    cv::Mat salMap(src.size(),CV_32F);
    for(int i=0;i<salMap.rows;i++)
    {
        for(int j=0;j<salMap.cols;j++)
        {
            salMap.at<float>(i,j)=salf.at<float>(suplabel.at<ushort>(i,j));
        }
    }

    cv::normalize(salMap, salMap, 0, 1, cv::NORM_MINMAX);

    cv::Mat outMap(cv::Size(wcut[1], wcut[0]), CV_32F, cv::Scalar(0));
    cv::Mat subMap = outMap(cv::Range(wcut[2],wcut[3]), cv::Range(wcut[4],wcut[5]));
    salMap.convertTo(subMap, subMap.type());
    return outMap;

}


cv::Mat GMRsaliency::getSuperpixels(const cv::Mat &image)
{
    cv::Mat labels;
    cv::Mat tempLabels;
    SLICSuperpixels superpixels;
    superpixels.run(image);
    tempLabels = superpixels.getLabels();
    tempLabels.convertTo(labels, CV_16U);

	return labels;
}

cv::Mat GMRsaliency::getAdjLoop(const cv::Mat &supLab)
{
    cv::Mat adj(cv::Size(spcounta, spcounta), CV_16U, cv::Scalar(0));
	for(int i=0;i<supLab.rows-1;i++)
	{
		for(int j=0;j<supLab.cols-1;j++)
		{
			if(supLab.at<ushort>(i,j)!=supLab.at<ushort>(i+1,j))
			{
				adj.at<ushort>(supLab.at<ushort>(i,j),supLab.at<ushort>(i+1,j))=1;
				adj.at<ushort>(supLab.at<ushort>(i+1,j),supLab.at<ushort>(i,j))=1;
			}
			if(supLab.at<ushort>(i,j)!=supLab.at<ushort>(i,j+1))
			{
				adj.at<ushort>(supLab.at<ushort>(i,j),supLab.at<ushort>(i,j+1))=1;
				adj.at<ushort>(supLab.at<ushort>(i,j+1),supLab.at<ushort>(i,j))=1;
			}
			if(supLab.at<ushort>(i,j)!=supLab.at<ushort>(i+1,j+1))
			{
				adj.at<ushort>(supLab.at<ushort>(i,j),supLab.at<ushort>(i+1,j+1))=1;
				adj.at<ushort>(supLab.at<ushort>(i+1,j+1),supLab.at<ushort>(i,j))=1;
			}
			if(supLab.at<ushort>(i+1,j)!=supLab.at<ushort>(i,j+1))
			{
				adj.at<ushort>(supLab.at<ushort>(i+1,j),supLab.at<ushort>(i,j+1))=1;
				adj.at<ushort>(supLab.at<ushort>(i,j+1),supLab.at<ushort>(i+1,j))=1;
			}
		}
	}

    std::vector<ushort> bd;
    std::vector<ushort>::iterator result;
	for(int i=0;i<supLab.cols;i++)
	{
        result = std::find(bd.begin(),bd.end(),supLab.at<ushort>(0,i));
		if(result==bd.end())
			bd.push_back(supLab.at<ushort>(0,i));
	}
	for(int i=0;i<supLab.cols;i++)
	{
        result = std::find(bd.begin(),bd.end(),supLab.at<ushort>(supLab.rows-1,i));
		if(result==bd.end())
			bd.push_back(supLab.at<ushort>(supLab.rows-1,i));
	}
	for(int i=0;i<supLab.rows;i++)
	{
        result = std::find(bd.begin(),bd.end(),supLab.at<ushort>(i,0));
		if(result==bd.end())
			bd.push_back(supLab.at<ushort>(i,0));
	}
	for(int i=0;i<supLab.rows;i++)
	{
        result = std::find(bd.begin(),bd.end(),supLab.at<ushort>(i,supLab.cols-1));
		if(result==bd.end())
			bd.push_back(supLab.at<ushort>(i,supLab.cols-1));
	}
    std::vector<ushort>::iterator bdi=bd.begin();
    std::vector<ushort>::iterator bdj;
	for(;bdi!=bd.end();bdi++)
	{
		for(bdj=bdi+1;bdj!=bd.end();bdj++)
		{
			adj.at<ushort>(*bdi,*bdj)=1;
			adj.at<ushort>(*bdj,*bdi)=1;
		}
	}

	return adj;

}

cv::Mat GMRsaliency::getWeight(const cv::Mat &img,const cv::Mat &supLab,const cv::Mat &adj)
{
    std::vector<float> supL(spcounta,0);
    std::vector<float> supa(spcounta,0);
    std::vector<float> supb(spcounta,0);
    std::vector<float> pcount(spcounta,0);
	for(int i=0;i<img.rows;i++)
	{
		const float *labp=img.ptr<float>(i);
		for(int j=0;j<img.cols;j++,labp+=3)
		{
			supL[supLab.at<ushort>(i,j)]+=labp[0];
			supa[supLab.at<ushort>(i,j)]+=labp[1];
			supb[supLab.at<ushort>(i,j)]+=labp[2];
			pcount[supLab.at<ushort>(i,j)]+=1.;
		}
	}
	for(int i=0;i<spcounta;i++)
	{
		supL[i]/=pcount[i];
		supa[i]/=pcount[i];
		supb[i]/=pcount[i];
	}
    cv::Mat w(adj.size(),CV_32F, cv::Scalar(-1));
    float minw=(float)std::numeric_limits<float>::max(),maxw=(float)std::numeric_limits<float>::min();
	for(int i=0;i<spcounta;i++)
	{
		for(int j=0;j<spcounta;j++)
		{
			if(adj.at<ushort>(i,j)==1)
			{
                float dist=std::sqrt(std::pow((supL[i]-supL[j]),2)+ std::pow((supa[i]-supa[j]),2)+ std::pow((supb[i]-supb[j]),2));
				w.at<float>(i,j)=dist;
				if(minw>dist)
					minw=dist;
				if(maxw<dist)
					maxw=dist;
				for(int k=0;k<spcounta;k++)
				{
					if(adj.at<ushort>(j,k)==1&&k!=i)
					{
                        float dist=std::sqrt(std::pow((supL[i]-supL[k]),2)+ std::pow((supa[i]-supa[k]),2)+ std::pow((supb[i]-supb[k]),2));
						w.at<float>(i,k)=dist;
						if(minw>dist)
							minw=dist;
						if(maxw<dist)
							maxw=dist;
					}
						
				}
			}
		}
	}
	for(int i=0;i<spcounta;i++)
	{
		for(int j=0;j<spcounta;j++)
		{
			if(w.at<float>(i,j)>-1)
                w.at<float>(i,j)=std::exp(-(w.at<float>(i,j)-minw)/((maxw-minw)*delta));
			else
				w.at<float>(i,j)=0;
		}
	}
	return w;
}

cv::Mat GMRsaliency::getOptAff(const cv::Mat &W)
{

    cv::Mat dd(cv::Size(W.rows,1), CV_32F);
    cv::reduce(W,dd,1, cv::REDUCE_SUM);
    cv::Mat D(W.size(), CV_32F);
    D = cv::Mat::diag(dd);
    cv::Mat optAff(W.size(), CV_32F);
    optAff = (D - alpha*W);
    optAff = optAff.inv();
    cv::Mat B = cv::Mat::ones(optAff.size(), CV_32F)- cv::Mat::eye(optAff.size(),CV_32F);
    optAff = optAff.mul(B);
	return optAff;

}

cv::Mat GMRsaliency::getBdQuery(const cv::Mat &supLab,int type)
{
    cv::Mat y(cv::Size(1,spcounta), CV_32F, cv::Scalar(0));
	switch(type)
	{
	case 1:
		for(int i=0;i<supLab.cols;i++)
			y.at<float>(supLab.at<ushort>(0,i))=1;
		break;
	case 2:
		for(int i=0;i<supLab.cols;i++)
			y.at<float>(supLab.at<ushort>(supLab.rows-1,i))=1;
		break;
	case 3:
		for(int i=0;i<supLab.rows;i++)
			y.at<float>(supLab.at<ushort>(i,0))=1;
		break;
	case 4:
		for(int i=0;i<supLab.rows;i++)
			y.at<float>(supLab.at<ushort>(i,supLab.cols-1))=1;
		break;
	default:
		printf("error");
	}
	return y;
		
}

cv::Mat GMRsaliency::removeFrame(const cv::Mat &img,int *wcut)
{
	double thr=0.6;
    cv::Mat grayimg;
    cv::cvtColor(img, grayimg, cv::COLOR_BGR2GRAY);
    cv::Mat edge;
    cv::Canny(grayimg,edge,150*0.4,150);

	int flagt=0;
	int flagd=0;
	int flagr=0;
	int flagl=0;
	int t=0;
	int d=0;
	int l=0;
	int r=0;

	int m=grayimg.rows;
	int n=grayimg.cols;

	int i=0;
	while(i<30)
	{
        float pbt=(float)cv::mean(edge(cv::Range(i,i+1), cv::Range::all()))[0];
        float pbd=(float)cv::mean(edge(cv::Range(m-i-1,m-i), cv::Range::all()))[0];
        float pbl=(float)cv::mean(edge(cv::Range::all(), cv::Range(i,i+1)))[0];
        float pbr=(float)cv::mean(edge(cv::Range::all(), cv::Range(n-i-1,n-i)))[0];
		if(pbt/255>thr)
		{
			t=i;
			flagt=1;
		}
		if(pbd/255>thr)
		{
			d=i;
			flagd=1;
		}
		if(pbl/255>thr)
		{
			l=i;
			flagl=1;
		}
		if(pbr/255>thr)
		{
			r=i;
			flagr=1;
		}
		i++;
	}
	int flagrm=flagt+flagd+flagl+flagr;
    cv::Mat outimg;
	if(flagrm>1)
	{
		int maxwidth;
		maxwidth=(t>d)?t:d;
		maxwidth=(maxwidth>l)?maxwidth:l;
		maxwidth=(maxwidth>r)?maxwidth:r;
		if(t==0)
			t=maxwidth;
		if(d==0)
			d=maxwidth;
		if(l==0)
			l=maxwidth;
		if(r==0)
			r=maxwidth;
        outimg=img(cv::Range(t,m-d),cv::Range(l,n-r));
		wcut[0]=m;
		wcut[1]=n;
		wcut[2]=t;
		wcut[3]=m-d;
		wcut[4]=l;
		wcut[5]=n-r;
	}
	else
	{
		wcut[0]=m;
		wcut[1]=n;
		wcut[2]=0;
		wcut[3]=m;
		wcut[4]=0;
		wcut[5]=n;
		outimg=img;
	}
	return outimg;

}
