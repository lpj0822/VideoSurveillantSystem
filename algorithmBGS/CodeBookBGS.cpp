#include "CodeBookBGS.h"
#include <iostream>

static const int Tdel = 200;
static const int Tadd = 150;
static const int Th = 200;

CodeBookBGS::CodeBookBGS(): t(0), learningFrames(DEFAULT_LEARNFRAMES), alpha(DEFAULT_ALPHA), beta(DEFAULT_BETA)
{
    init();
    std::cout << "CodeBookBGS()" << std::endl;
}

CodeBookBGS::~CodeBookBGS()
{
    std::cout << "~CodeBookBGS()" << std::endl;
}

void CodeBookBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    cv::Mat img_input_gray;
    if (img_input.empty())
        return;

    img_output = cv::Mat::zeros(img_input.size(), CV_8UC1);
    img_bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);

    if(firstTime)
    {
        img_foreground = cv::Mat::zeros(img_input.size(), CV_8UC1);
        initializeCodebook(img_input.rows, img_input.cols);

        saveConfig();
        firstTime = false;
    }

    cv::cvtColor(img_input, img_input_gray, cv::COLOR_BGR2GRAY);
    fg_cb(img_input_gray, img_foreground);

    if (showOutput)
    {
        cv::imshow("Codebook", img_foreground);
    }

    img_foreground.copyTo(img_output);
}

void CodeBookBGS::initializeCodebook(int w, int h)
{
    cbMain = new std::vector<codeword>*[w];
    for (int i = 0; i < w; ++i)
        cbMain[i] = new std::vector<codeword>[h];

    cbCache = new std::vector<codeword>*[w];
    for (int i = 0; i < w; ++i)
        cbCache[i] = new std::vector<codeword>[h];
}

void CodeBookBGS::update_cb(const cv::Mat& frame)
{
    if (t > learningFrames)
        return;

    for (int i = 0; i < frame.rows; i++)
    {
        for (int j = 0; j < frame.cols; j++)
        {
            int pix = frame.at<uchar>(i, j);
            std::vector<codeword>& cm = cbMain[i][j];
            bool found = false;
            for (int k = 0; k<cm.size(); k++)
            {
                if (cm[k].min <= pix && pix <= cm[k].max && !found)
                {
                    found = true;
                    cm[k].min = ((pix - alpha) + (cm[k].f*cm[k].min)) / (cm[k].f + 1);
                    cm[k].max = ((pix + alpha) + (cm[k].f*cm[k].max)) / (cm[k].f + 1);
                    cm[k].l = 0;
                    cm[k].last = t;
                    cm[k].f++;
                }
                else
                {
                    cm[k].l++;
                }
            }
            if (!found)
            {
                codeword n = {};
                n.min = (float)std::max(0, pix - alpha);
                n.max = (float)std::min(255, pix + alpha);
                n.f = 1;
                n.l = 0;
                n.first = t;
                n.last = t;
                cm.push_back(n);
            }
        }
    }
    t++;
}

void CodeBookBGS::fg_cb(const cv::Mat& frame, cv::Mat& fg)
{
    if (t <= learningFrames)
    {
        update_cb(frame);
        return;
    }

    for (int i = 0; i<frame.rows; i++)
    {
        for (int j = 0; j<frame.cols; j++)
        {
            int pix = frame.at<uchar>(i, j);
            std::vector<codeword>& cm = cbMain[i][j];
            bool found = false;
            for (int k = 0; k<cm.size(); k++)
            {
                if (cm[k].min <= pix && pix <= cm[k].max && !found)
                {
                    cm[k].min = ((1 - beta)*(pix - alpha)) + (beta*cm[k].min);
                    cm[k].max = ((1 - beta)*(pix + alpha)) + (beta*cm[k].max);
                    cm[k].l = 0;
                    cm[k].first = t;
                    cm[k].f++;
                    found = true;
                }
                else
                    cm[k].l++;
            }
            cm.erase(remove_if(cm.begin(), cm.end(), [](codeword& c) { return c.l >= Tdel; }), cm.end());
            fg.at<uchar>(i, j) = found ? 0 : 255;
            if (found) continue;
            found = false;
            std::vector<codeword>& cc = cbCache[i][j];
            for (int k = 0; k<cc.size(); k++)
            {
                if (cc[k].min <= pix && pix <= cc[k].max && !found)
                {
                    cc[k].min = ((1 - beta)*(pix - alpha)) + (beta*cc[k].min);
                    cc[k].max = ((1 - beta)*(pix + alpha)) + (beta*cc[k].max);
                    cc[k].l = 0;
                    cc[k].first = t;
                    cc[k].f++;
                    found = true;
                }
                else
                    cc[k].l++;
            }

            if (!found)
            {
                codeword n = {};
                n.min = (float)std::max(0, pix - alpha);
                n.max = (float)std::min(255, pix + alpha);
                n.f = 1;
                n.l = 0;
                n.first = t;
                n.last = t;
                cc.push_back(n);
            }

            cc.erase(remove_if(cc.begin(), cc.end(), [](codeword& c) { return c.l >= Th; }), cc.end());

            for (std::vector<codeword>::iterator it = cc.begin(); it != cc.end(); it++)
                if (it->f > Tadd)
                    cm.push_back(*it);

            cc.erase(remove_if(cc.begin(), cc.end(), [](codeword& c) { return c.f > Tadd; }), cc.end());
        }
    }
}

void CodeBookBGS::init()
{
    firstTime = true;
    loadConfig();
}

void CodeBookBGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/CodeBookBGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "alpha", alpha);
    cv::write(fs, "beta", beta);
    cv::write(fs, "learningFrames", learningFrames);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void CodeBookBGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/CodeBookBGS.xml", cv::FileStorage::READ);

    cv::read(fs["alpha"] ,alpha, DEFAULT_ALPHA);
    cv::read(fs["beta"] ,beta, DEFAULT_BETA);
    cv::read(fs["learningFrames"], learningFrames, DEFAULT_LEARNFRAMES);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
