#include "picturesave.h"
#include <iostream>
#include <QDebug>

PictureSave::PictureSave(QObject *parent) : QObject(parent)
{
    init();
    std::cout << "PictureSave()" << std::endl;
}

PictureSave::~PictureSave()
{
    for(int loop=0; loop<MAXPICTURETHREAD; loop++)
    {
        if(saveThread[loop] != nullptr)
        {
            if(saveThread[loop]->isRunning())
            {
                saveThread[loop]->wait();
                delete saveThread[loop];
                saveThread[loop] = nullptr;
            }
            else
            {
                delete saveThread[loop];
                saveThread[loop] = nullptr;
            }
        }
    }
    std::cout << "~PictureSave()" << std::endl;
}

void PictureSave::savePictureData(QString savePath, QString saveFileName, const cv::Mat& frame)
{
    int errCode=0;
    if(savePath.isEmpty() || saveFileName.isEmpty())
    {
        qDebug() << "路径有误:" << savePath << endl << saveFileName;
        return;
    }
    if(numThread >= MAXPICTURETHREAD)
    {
        numThread = 0;
    }
    if(saveThread[numThread%MAXPICTURETHREAD] == nullptr)
    {
        saveThread[numThread%MAXPICTURETHREAD] = new PictureSaveThread();
        connect(saveThread[numThread%MAXPICTURETHREAD], &PictureSaveThread::signalPictureSaveFinish, this, &PictureSave::signalSaveFinish);
        errCode = saveThread[numThread%MAXPICTURETHREAD]->initData(savePath,saveFileName,frame);
        if(errCode == 0)
        {
            saveThread[numThread%MAXPICTURETHREAD]->start();
            numThread++;
        }
        else
        {
            qDebug() << errorCodeString.getErrCodeString(errCode);
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
        }

    }
    else
    {
        if(saveThread[numThread%MAXPICTURETHREAD]->isRunning())
        {
            saveThread[numThread%MAXPICTURETHREAD]->wait();
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
        }
        else
        {
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
            saveThread[numThread%MAXPICTURETHREAD] = new PictureSaveThread();
            connect(saveThread[numThread%MAXPICTURETHREAD],&PictureSaveThread::signalPictureSaveFinish,this,&PictureSave::signalSaveFinish);
            errCode=saveThread[numThread%MAXPICTURETHREAD]->initData(savePath,saveFileName,frame);
            if(errCode == 0)
            {
                saveThread[numThread%MAXPICTURETHREAD]->start();
                numThread++;
            }
            else
            {
                qDebug() << errorCodeString.getErrCodeString(errCode);
                delete saveThread[numThread%MAXPICTURETHREAD];
                saveThread[numThread%MAXPICTURETHREAD] = nullptr;
            }
        }
    }
}

void PictureSave::savePictureData(QString savePath, QString saveFileName, QImage image)
{
    int errCode=0;
    if(savePath.isEmpty() || saveFileName.isEmpty())
    {
        qDebug() <<"路径有误:" << savePath << endl << saveFileName;
        return;
    }
    if(numThread >= MAXPICTURETHREAD)
    {
        numThread = 0;
    }
    if(saveThread[numThread%MAXPICTURETHREAD] == nullptr)
    {
        saveThread[numThread%MAXPICTURETHREAD] = new PictureSaveThread();
        connect(saveThread[numThread%MAXPICTURETHREAD], &PictureSaveThread::signalPictureSaveFinish, this, &PictureSave::signalSaveFinish);
        errCode = saveThread[numThread%MAXPICTURETHREAD]->initData(savePath,saveFileName,image);
        if(errCode==0)
        {
            saveThread[numThread%MAXPICTURETHREAD]->start();
            numThread++;
        }
        else
        {
            qDebug() << errorCodeString.getErrCodeString(errCode);
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
        }

    }
    else
    {
        if(saveThread[numThread%MAXPICTURETHREAD]->isRunning())
        {
            saveThread[numThread%MAXPICTURETHREAD]->wait();
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
        }
        else
        {
            delete saveThread[numThread%MAXPICTURETHREAD];
            saveThread[numThread%MAXPICTURETHREAD] = nullptr;
            saveThread[numThread%MAXPICTURETHREAD] = new PictureSaveThread();
            connect(saveThread[numThread%MAXPICTURETHREAD], &PictureSaveThread::signalPictureSaveFinish, this, &PictureSave::signalSaveFinish);
            errCode = saveThread[numThread%MAXPICTURETHREAD]->initData(savePath,saveFileName,image);
            if(errCode == 0)
            {
                saveThread[numThread%MAXPICTURETHREAD]->start();
                numThread++;
            }
            else
            {
                qDebug() << errorCodeString.getErrCodeString(errCode);
                delete saveThread[numThread%MAXPICTURETHREAD];
                saveThread[numThread%MAXPICTURETHREAD] = nullptr;
            }
        }
    }
}


void PictureSave::init()
{
    numThread = 0;
    for(int loop=0; loop<MAXPICTURETHREAD; loop++)
    {
        saveThread[loop] = nullptr;
    }
}
