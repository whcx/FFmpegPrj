#pragma once

#include <thread>
#include <iostream>
#include <stdio.h>

typedef void (*CallbackFun)(double height, void* context);

class TestThread
{
public:
    void myThread();
    void creatThread();

    static void setHeight(double height, void* context);
    void testHeight();
    void registerHeightCallback(CallbackFun callback, void* context);
    void registerCallback();
    
private:
    TestThread* m_tt;
    std::thread myT;
};

