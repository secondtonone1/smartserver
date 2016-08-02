// libeventtest.cpp : Defines the entry point for the console application.
//

// LibeventTest.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include"../NetWorkSystem/networksystem.h"
#include <fstream>
#include "../ProtoBuf/smart.msg.pb.h"
#include "../basethread.h"
#include "../NetWorkSystem/networker.h"

#pragma comment(lib, "libprotobuf.lib")  
#pragma comment(lib, "libprotoc.lib")

int main(int argc, char **argv)
{
	bool initialRes = NetWorkSystem::getSingleton().initial();
	
	smart::test msg1; 
	msg1.set_age(101); 
	msg1.set_name("nice to meet you!");
	std::fstream out("User.pb", std::ios::out | std::ios::binary | std::ios::trunc);  
	msg1.SerializeToOstream(&out);  
	out.close(); 

	smart::test msg2;  
	std::fstream in("User.pb", std::ios::in | std::ios::binary);  
	if (!msg2.ParseFromIstream(&in)) {  
		std::cerr << "Failed to parse User.pb." << std::endl;  
		exit(1);  
	}  

	std::cout << msg2.age() << std::endl;  
	std::cout << msg2.name() << std::endl; 
	std::cout << msg2.email() << std::endl;
	std::cout << "adfdf" <<std::endl;


	NetWorkSystem::getSingleton().run();
	
	
	getchar();

    return 0;
}

