// libeventtest.cpp : Defines the entry point for the console application.
//

// LibeventTest.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include"../NetWorkSystem/networksystem.h"
int main(int argc, char **argv)
{
	bool initialRes = NetWorkSystem::getSingleton().initial();
	
	NetWorkSystem::getSingleton().run();
	
	getchar();

    return 0;
}

