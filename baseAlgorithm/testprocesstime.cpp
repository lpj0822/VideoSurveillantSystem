#include "testprocesstime.h"
#include <iomanip>
#include <iostream>

TestProcessTime::TestProcessTime()
{
    initData();
    std::cout << "TestProcessTime()" << std::endl;
}

TestProcessTime::~TestProcessTime()
{
    std::cout << "~TestProcessTime()" << std::endl;
}

//测试算法时间
void TestProcessTime::tic()
{
    duration = static_cast<double>(cv::getTickCount());
}

void TestProcessTime::toc()
{
    duration = (static_cast<double>(cv::getTickCount()) - duration) / cv::getTickFrequency();
    std::cout << processname << "\ttime(sec):" << std::fixed << std::setprecision(4) << duration << std::endl;
}

//设置算法名称和得到算法名称
void TestProcessTime::setProcessname(const std::string& name)
{
    this->processname = name;
}

std::string TestProcessTime::getProcessname()
{
    return processname;
}

void TestProcessTime::initData()
{
    processname = "";//算法名称
    duration = 0;//算法运行时间
}

