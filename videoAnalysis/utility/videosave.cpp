#include "videosave.h"
#include <iostream>
#include <QDebug>

VideoSave::VideoSave(QObject *parent) : QObject(parent)
{
    init();
    std::cout<<"VideoSave()"<<std::endl;
}

VideoSave::~VideoSave()
{
    for(int loop=0;loop<MAXVIDEOTHREAD;loop++)
    {
        if(saveThread[loop])
        {
            if(saveThread[loop]->isRunning())
            {
                saveThread[loop]->stopThread();
                saveThread[loop]->wait();
                delete saveThread[loop];
                saveThread[loop]=NULL;
            }
            else
            {
                delete saveThread[loop];
                saveThread[loop]=NULL;
            }
        }
    }
    std::cout<<"~VideoSave()"<<std::endl;
}

void VideoSave::saveVideoData(QString videoPath, QString savePath, QString saveFileName, int stopPos, int intervalSec)
{
    int errCode;
    if(videoPath.isEmpty()||savePath.isEmpty()||saveFileName.isEmpty())
    {
        qDebug()<<"路径有误:"<<videoPath<<endl<<savePath<<endl<<saveFileName;
        return;
    }
    if(numThread>=MAXVIDEOTHREAD)
    {
        numThread=0;
    }
    if(saveThread[numThread%MAXVIDEOTHREAD]==NULL)
    {
        saveThread[numThread%MAXVIDEOTHREAD]=new VideoSaveThread();
        connect(saveThread[numThread%MAXVIDEOTHREAD],&VideoSaveThread::signalVideoSaveFinish,this,&VideoSave::signalSaveFinish);
        errCode=saveThread[numThread%MAXVIDEOTHREAD]->openVideo(videoPath);
        if(errCode==0)
        {
            errCode=saveThread[numThread%MAXVIDEOTHREAD]->initSaveVideoData(savePath,saveFileName, stopPos, intervalSec,true);
            if(errCode==0)
            {
                saveThread[numThread%MAXVIDEOTHREAD]->startThread();
                saveThread[numThread%MAXVIDEOTHREAD]->start();
                numThread++;
            }
            else
            {
                qDebug()<<errorCodeString.getErrCodeString(errCode);
                delete saveThread[numThread%MAXVIDEOTHREAD];
                saveThread[numThread%MAXVIDEOTHREAD]=NULL;
            }
        }
        else
        {
            qDebug()<<errorCodeString.getErrCodeString(errCode);
            delete saveThread[numThread%MAXVIDEOTHREAD];
            saveThread[numThread%MAXVIDEOTHREAD]=NULL;
        }

    }
    else
    {
        if(saveThread[numThread%MAXVIDEOTHREAD]->isRunning())
        {
            saveThread[numThread%MAXVIDEOTHREAD]->wait();
            delete saveThread[numThread%MAXVIDEOTHREAD];
            saveThread[numThread%MAXVIDEOTHREAD]=NULL;
        }
        else
        {
            delete saveThread[numThread%MAXVIDEOTHREAD];
            saveThread[numThread%MAXVIDEOTHREAD]=NULL;
            saveThread[numThread%MAXVIDEOTHREAD]=new VideoSaveThread();
            connect(saveThread[numThread%MAXVIDEOTHREAD],&VideoSaveThread::signalVideoSaveFinish,this,&VideoSave::signalSaveFinish);
            errCode=saveThread[numThread%MAXVIDEOTHREAD]->openVideo(videoPath);
            if(errCode==0)
            {
                errCode=saveThread[numThread%MAXVIDEOTHREAD]->initSaveVideoData(savePath,saveFileName,stopPos,intervalSec,true);
                if(errCode==0)
                {
                    saveThread[numThread%MAXVIDEOTHREAD]->startThread();
                    saveThread[numThread%MAXVIDEOTHREAD]->start();
                    numThread++;
                }
                else
                {
                    qDebug()<<errorCodeString.getErrCodeString(errCode);
                    delete saveThread[numThread%MAXVIDEOTHREAD];
                    saveThread[numThread%MAXVIDEOTHREAD]=NULL;
                }
            }
            else
            {
                qDebug()<<errorCodeString.getErrCodeString(errCode);
                delete saveThread[numThread%MAXVIDEOTHREAD];
                saveThread[numThread%MAXVIDEOTHREAD]=NULL;
            }
        }
    }
}

void VideoSave::init()
{
    numThread=0;
    for(int loop=0;loop<MAXVIDEOTHREAD;loop++)
    {
        saveThread[loop] = NULL;
    }
}
