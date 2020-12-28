#include "TestThread.h"
#include <iostream>

using namespace std;

typedef void (*CallbackFun)(double height, void* context);
CallbackFun m_callback = nullptr;

void TestThread::registerHeightCallback(CallbackFun callback, void* context)
{
	m_callback = callback;
	m_tt = (TestThread*)context;
	m_tt->testHeight();
}

void TestThread::creatThread()
{
    //myT =std::thread(&TestThread::myThread, this);
	std::thread t1(&TestThread::myThread, this);
	//t1.join();
	t1.detach();
}

void TestThread::myThread()
{
	int count = 0;
	while (count < INT8_MAX)
	{
		std::cout <<"sub thread runing..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		count++;
	}
}

void TestThread::testHeight()
{
	cout << "the method of instance." << endl;
}
void TestThread::setHeight(double height, void* context)
{
	cout << "height:" << height << endl;
}

void TestThread::registerCallback()
{
	registerHeightCallback(setHeight, this);
	m_callback(10, this);
}
