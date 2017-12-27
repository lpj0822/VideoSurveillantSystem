#include "PixelBasedAdaptiveSegmenter.h"
#include <iostream>

PixelBasedAdaptiveSegmenter::PixelBasedAdaptiveSegmenter()
{
    init();
    std::cout << "PixelBasedAdaptiveSegmenter()" << std::endl;
}

PixelBasedAdaptiveSegmenter::~PixelBasedAdaptiveSegmenter()
{
    std::cout << "~PixelBasedAdaptiveSegmenter()" << std::endl;
}

void PixelBasedAdaptiveSegmenter::process(const cv::Mat &img_input, cv::Mat &img_output,cv::Mat &img_bgmodel)
{
    if(img_input.empty())
        return;

    if(firstTime)
    {
        pbas.setAlpha(alpha);
        pbas.setBeta(beta);
        pbas.setN(N);
        pbas.setRaute_min(Raute_min);
        pbas.setR_incdec(R_incdec);
        pbas.setR_lower(R_lower);
        pbas.setR_scale(R_scale);
        pbas.setT_dec(T_dec);
        pbas.setT_inc(T_inc);
        pbas.setT_init(T_init);
        pbas.setT_lower(T_lower);
        pbas.setT_upper(T_upper);

        saveConfig();
        firstTime = false;
    }

    cv::Mat img_input_new;
    if (enableInputBlur)
		cv::GaussianBlur(img_input, img_input_new, cv::Size(5, 5), 1.5);
	else
		img_input.copyTo(img_input_new);

    pbas.process(&img_input_new, &img_foreground);
	
	if (enableOutputBlur)
		cv::medianBlur(img_foreground, img_foreground, 5);

    if (showOutput)
    {
        cv::imshow("PBAS", img_foreground);
    }

    img_bgmodel = cv::Mat::zeros(img_input.size(), img_input.type());
    img_foreground.copyTo(img_output);
}

void PixelBasedAdaptiveSegmenter::init()
{
    firstTime = true;
    loadConfig();
}

void PixelBasedAdaptiveSegmenter::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/PixelBasedAdaptiveSegmenter.xml", cv::FileStorage::WRITE);
	
	cv::write(fs, "enableInputBlur", enableInputBlur);
    cv::write(fs, "enableOutputBlur", enableOutputBlur);

    cv::write(fs, "alpha", alpha);
    cv::write(fs, "beta", beta);
    cv::write(fs, "N", N);
    cv::write(fs, "Raute_min", Raute_min);
    cv::write(fs, "R_incdec", R_incdec);
    cv::write(fs, "R_lower", R_lower);
    cv::write(fs, "R_scale", R_scale);
    cv::write(fs, "T_dec", T_dec);
    cv::write(fs, "T_inc", T_inc);
    cv::write(fs, "T_init", T_init);
    cv::write(fs, "T_lower", T_lower);
    cv::write(fs, "T_upper", T_upper);

    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void PixelBasedAdaptiveSegmenter::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/PixelBasedAdaptiveSegmenter.xml",cv::FileStorage::READ);
	
	cv::read(fs["enableInputBlur"], enableInputBlur, true);
    cv::read(fs["enableOutputBlur"], enableOutputBlur, true);
  
    cv::read(fs["alpha"], alpha, 7.0f);
    cv::read(fs["beta"], beta, 1.0f);
    cv::read(fs["N"], N, 20);
    cv::read(fs["Raute_min"], Raute_min, 2);
    cv::read(fs["R_incdec"], R_incdec, 0.05f);
    cv::read(fs["R_lower"], R_lower, 18);
    cv::read(fs["R_scale"], R_scale, 5);
    cv::read(fs["T_dec"], T_dec, 0.05f);
    cv::read(fs["T_inc"], T_inc, 1);
    cv::read(fs["T_init"], T_init, 18);
    cv::read(fs["T_lower"], T_lower, 2);
    cv::read(fs["T_upper"], T_upper, 200);

    cv::read(fs["showOutput"], showOutput ,true);

    fs.release();
}
