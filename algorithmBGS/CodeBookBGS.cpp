#include "CodeBookBGS.h"
#include <iostream>

CodeBookBGS::CodeBookBGS()
{
    init();
    std::cout << "CodeBookBGS()" << std::endl;
}

CodeBookBGS::~CodeBookBGS()
{
    if (cB)
    {
        delete [] cB;
        cB = NULL;
    }
    std::cout << "~CodeBookBGS()" << std::endl;
}

void CodeBookBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    if (img_input.empty())
        return;

    if (firstTime)
    {
        imageLen = img_input.cols * img_input.rows;
        cB = new CodeBook[imageLen];
        // 得到与图像像素数目长度一样的一组码本,以便对每个像素进行处理
        for (int i=0; i<imageLen; i++)
        {
            // 初始化每个码元数目为0
            cB[i].numEntries = 0;
        }
        for (int i=0; i<nChannels; i++)
        {
            cbBounds[i] = channelsThreshold;	// 用于确定码元各通道的阀值
            minMod[i]	= minLengthChannels;	// 用于背景差分函数中
            maxMod[i]	= maxLengthChannels;	// 调整其值以达到最好的分割
        }

        saveConfig();
        firstTime = false;
        img_foreground = cv::Mat::zeros(img_input.size(), CV_8UC1);
    }

    uchar* pColor = NULL;
    cv::Mat	yuvImage = cv::Mat::zeros(img_input.size(),CV_8UC3);

    // 色彩空间转换,将rawImage 转换到YUV色彩空间,输出到yuvImage,即使不转换效果依然很好
    cv::cvtColor(img_input, yuvImage, CV_BGR2YCrCb);

    if (historyNumber <= historyCount)//historyCount帧内进行背景学习
    {
        pColor = (uchar *)(yuvImage.data);
        // 指向yuvImage 图像的通道数据
        for (int c=0; c<imageLen; c++)
        {
            updateCodeBook(pColor, cB[c], cbBounds, nChannels);
            // 对每个像素,调用此函数,捕捉背景中相关变化图像
            pColor += nChannels;//nChannels通道图像, 指向下一个像素通道数据
        }
        if (historyNumber == historyCount)// 到historyCount帧时调用下面函数,删除码本中陈旧的码元
        {
            for (int c=0; c<imageLen; c++)
                clearStaleEntries(cB[c]);
        }

        historyNumber++;
    }
    else
    {
        uchar maskPixelCodeBook;
        pColor = (uchar *)(yuvImage.data); //3 channel yuv image
        uchar *pMask = (uchar *)(img_foreground.data); //1 channel image
        // 指向ImaskCodeBook 通道数据序列的首元素
        for(int c=0; c<imageLen; c++)
        {
            maskPixelCodeBook = backgroundDiff(pColor, cB[c], nChannels, minMod, maxMod);
            *pMask++ = maskPixelCodeBook;
            pColor += nChannels;// pColor 指向的是nChannels通道图像
        }

        if (showOutput)
        {
            cv::imshow("CodeBookBGS", img_foreground);
        }

        img_foreground.copyTo(img_output);
    }
}

/*
 * int updateCodeBook(uchar *p, CodeBook &c, unsigned cbBounds)
 * Updates the codebook entry with a new data point
 *
 * @param p			Pointer to a YUV pixel
 * @param c			Codebook for this pixel
 * @param cbBounds		Learning bounds for codebook (Rule of thumb: 10)
 * @param numChannels	Number of color channels we're learning
 *
 * NOTES:cvBounds must be of size cvBounds[numChannels]
 * Return codebook index
*/
int CodeBookBGS::updateCodeBook(uchar *p, CodeBook &c, unsigned int *cbBounds, int numChannels)
{
    if (c.numEntries == 0)
        c.t = 0;
    // 码本中码元为零时初始化时间为0
    c.t += 1;	// Record learning event
    // 每调用一次加一,即每一帧图像加一

    //SET HIGH AND LOW BOUNDS
    int n;
    unsigned int high[3],low[3];
    for (n=0; n<numChannels; n++)
    {
        high[n] = *(p+n) + *(cbBounds+n);
        // *(p+n) 和 p[n] 结果等价,经试验*(p+n) 速度更快
        if(high[n] > 255)
            high[n] = 255;
        low[n] = *(p+n) - *(cbBounds+n);
        if(low[n] < 0)
            low[n] = 0;
        // 用p 所指像素通道数据,加减cbBonds中数值,作为此像素阀值的上下限
    }

    //SEE IF THIS FITS AN EXISTING CODEWORD
    int matchChannel;
    int i;
    for (i=0; i<c.numEntries; i++)
    {
        // 遍历此码本每个码元,测试p像素是否满足其中之一
        matchChannel = 0;
        for (n=0; n<numChannels; n++)
            //遍历每个通道
        {
            if ((c.cb[i]->learnLow[n] <= *(p+n)) && (*(p+n) <= c.cb[i]->learnHigh[n])) //Found an entry for this channel
            // 如果p 像素通道数据在该码元阀值上下限之间
            {
                matchChannel++;
            }
        }
        if (matchChannel == numChannels)		// If an entry was found over all channels
            // 如果p 像素各通道都满足上面条件
        {
            c.cb[i]->t_last_update = c.t;
            // 更新该码元时间为当前时间
            // adjust this codeword for the first channel
            for (n=0; n<numChannels; n++)
                //调整该码元各通道最大最小值
            {
                if (c.cb[i]->max[n] < *(p+n))
                    c.cb[i]->max[n] = *(p+n);
                else if (c.cb[i]->min[n] > *(p+n))
                    c.cb[i]->min[n] = *(p+n);
            }
            break;
        }
    }

    // ENTER A NEW CODE WORD IF NEEDED
    if (i == c.numEntries)  // No existing code word found, make a new one
    // p 像素不满足此码本中任何一个码元,下面创建一个新码元
    {
        code_element **foo = new code_element* [c.numEntries+1];
        // 申请c.numEntries+1 个指向码元的指针
        for (int ii=0; ii<c.numEntries; ii++)
            // 将前c.numEntries 个指针指向已存在的每个码元
            foo[ii] = c.cb[ii];

        foo[c.numEntries] = new code_element;
        // 申请一个新的码元
        if (c.numEntries)
            delete [] c.cb;
        // 删除c.cb 指针数组
        c.cb = foo;
        // 把foo 头指针赋给c.cb
        for (n=0; n<numChannels; n++)
            // 更新新码元各通道数据
        {
            c.cb[c.numEntries]->learnHigh[n] = high[n];
            c.cb[c.numEntries]->learnLow[n] = low[n];
            c.cb[c.numEntries]->max[n] = *(p+n);
            c.cb[c.numEntries]->min[n] = *(p+n);
        }
        c.cb[c.numEntries]->t_last_update = c.t;
        c.cb[c.numEntries]->stale = 0;
        c.numEntries += 1;
    }

    // OVERHEAD TO TRACK POTENTIAL STALE ENTRIES
    for (int s=0; s<c.numEntries; s++)
    {
        // This garbage is to track which codebook entries are going stale
        int negRun = c.t - c.cb[s]->t_last_update;
        // 计算该码元的不更新时间
        if (c.cb[s]->stale < negRun)
            c.cb[s]->stale = negRun;
    }

    // SLOWLY ADJUST LEARNING BOUNDS
    for (n=0; n<numChannels; n++)
        // 如果像素通道数据在高低阀值范围内,但在码元阀值之外,则缓慢调整此码元学习界限
    {
        if (c.cb[i]->learnHigh[n] < high[n])
            c.cb[i]->learnHigh[n] += 1;
        if (c.cb[i]->learnLow[n] > low[n])
            c.cb[i]->learnLow[n] -= 1;
    }

    return(i);
}

/*
 * uchar cvbackgroundDiff(uchar *p, CodeBook &c, int minMod, int maxMod)
 * Given a pixel and a code book, determine if the pixel is covered by the codebook
 *
 * @param p		pixel pointer (YUV interleaved)
 * @param c		codebook reference
 * @param numChannels  Number of channels we are testing
 * @param maxMod	Add this (possibly negative) number onto max level when code_element determining if new pixel is foreground
 * @param minMod	Subract this (possible negative) number from min level code_element when determining if pixel is foreground
 *
 *  NOTES:minMod and maxMod must have length numChannels, e.g. 3 channels => minMod[3], maxMod[3].
 * Return 0 => background, 255 => foreground
*/
uchar CodeBookBGS::backgroundDiff(uchar *p, CodeBook &c, int numChannels, int *minMod, int *maxMod)
{
    // 下面步骤和背景学习中查找码元如出一辙
    int matchChannel;
    //SEE IF THIS FITS AN EXISTING CODEWORD
    int i;
    for (i=0; i<c.numEntries; i++)
    {
        matchChannel = 0;
        for (int n=0; n<numChannels; n++)
        {
            if ((c.cb[i]->min[n] - minMod[n] <= *(p+n)) && (*(p+n) <= c.cb[i]->max[n] + maxMod[n]))
                matchChannel++; //Found an entry for this channel
            else
                break;
        }
        if (matchChannel == numChannels)
            break; //Found an entry that matched all channels
    }
    if (i == c.numEntries)
        // p像素各通道值满足码本中其中一个码元,则返回白色
        return 255;

    return 0;
}


/*
 * int clearStaleEntries(CodeBook &c)
 * After you've learned for some period of time, periodically call this to clear out stale codebook entries
 *
 * @param c		Codebook to clean up
 *
 * Return number of entries cleared
*/
int CodeBookBGS::clearStaleEntries(CodeBook &c)
{
    int staleThresh = c.t >> 1;			// 设定刷新时间
    int *keep = new int [c.numEntries];	// 申请一个标记数组
    int keepCnt = 0;					// 记录不删除码元数目
    //SEE WHICH CODEBOOK ENTRIES ARE TOO STALE
    for (int i=0; i<c.numEntries; i++)
        // 遍历码本中每个码元
    {
        if (c.cb[i]->stale > staleThresh)
            // 如码元中的不更新时间大于设定的刷新时间,则标记为删除
            keep[i] = 0; //Mark for destruction
        else
        {
            keep[i] = 1; //Mark to keep
            keepCnt += 1;
        }
    }

    // KEEP ONLY THE GOOD
    c.t = 0;						//Full reset on stale tracking
    // 码本时间清零
    code_element **foo = new code_element* [keepCnt];
    // 申请大小为keepCnt 的码元指针数组
    int k=0;
    for (int ii=0; ii<c.numEntries; ii++)
    {
        if (keep[ii])
        {
            foo[k] = c.cb[ii];
            foo[k]->stale = 0;		//We have to refresh these entries for next clearStale
            foo[k]->t_last_update = 0;
            k++;
        }
    }
    //CLEAN UP
    delete [] keep;
    delete [] c.cb;
    c.cb = foo;
    // 把foo 头指针地址赋给c.cb
    int numCleared = c.numEntries - keepCnt;
    // 被清理的码元个数
    c.numEntries = keepCnt;
    // 剩余的码元地址
    return numCleared;
}

void CodeBookBGS::init()
{
    cB = NULL;
    imageLen = 0;
    nChannels = CHANNELS;
    historyNumber = 1;
    firstTime = true;

    loadConfig();
}

void CodeBookBGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/CodeBookBGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "historyCount", historyCount);
    cv::write(fs, "channelsThreshold", channelsThreshold);
    cv::write(fs, "minLengthChannels", minLengthChannels);
    cv::write(fs, "maxLengthChannels", maxLengthChannels);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void CodeBookBGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/CodeBookBGS.xml", cv::FileStorage::READ);

    cv::read(fs["historyCount"], historyCount, 20);
    cv::read(fs["channelsThreshold"], channelsThreshold, 10);
    cv::read(fs["minLengthChannels"], minLengthChannels, 20);
    cv::read(fs["maxLengthChannels"], maxLengthChannels, 20);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
