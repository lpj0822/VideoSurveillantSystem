#ifndef TESTPROCESSTIME_H
#define TESTPROCESSTIME_H

#include <opencv2/core.hpp>

class TestProcessTime
{
public:
    TestProcessTime();
    ~TestProcessTime();

    //测试算法时间
    void tic();
    void toc();

    //设置算法名称和得到算法名称
    void setProcessname(const std::string& name);
    std::string getProcessname();

private:

    std::string processname;//算法名称
    double duration;//算法运行时间

    void initData();
};

#endif // TESTPROCESSTIME_H
