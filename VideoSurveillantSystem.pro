#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T12:10:40
#
#-------------------------------------------------

QT       += core gui sql av avwidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

TARGET = VideoSurveillantSystem
TEMPLATE = app

include(colorConvert/colorConvert.pri)
include(imageFiltering/imageFiltering.pri)
include(algorithmBGS/algorithmBGS.pri)
include(shadowDetect/shadowDetect.pri)
include(multipletracking/multipletracking.pri)
include(algorithmClassify/algorithmClassify.pri)
include(saliencyDetect/saliencyDetect.pri)
include(baseAlgorithm/baseAlgorithm.pri)
include(videoAnalysis/videoAnalysis.pri)
include(applicationGUI/applicationGUI.pri)

SOURCES += main.cpp\
    simplecontrolwindow.cpp \
    mainwindow.cpp

HEADERS  += \
    simplecontrolwindow.h \
    mainwindow.h
	
RESOURCES += \
    images.qrc \
    style.qrc \
    QtAwesome.qrc

MOC_DIR = temp/moc
RCC_DIR = temp/rcc
UI_DIR  = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR = bin

RC_ICONS = appico.ico

INCLUDEPATH+= D:\opencv\build\include\
              D:\opencv\build\include\opencv\
              D:\opencv\build\include\opencv2


LIBS+=D:\opencv\build\x64\vc12\lib\opencv_world310d.lib
LIBS+=D:\opencv\build\x64\vc12\lib\opencv_world310.lib
