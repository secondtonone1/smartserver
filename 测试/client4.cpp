#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#define BUFFLEN 1024
#define SERVER_PORT 9995

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include <iostream>
#include <fstream>
#include "smart.msg.pb.h"

using namespace smart;

using namespace std;

struct PacketHead
{
	int packetID;
	int packetLen;
};

void *create(void *arg)
{
    int s;

	char buff[BUFFLEN];

	int n = 0;

	//建立套接字
	s = socket(AF_INET, SOCK_STREAM, 0);


	//初始化服务器地址
	struct sockaddr_in server;

	memset(&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	//server.sin_addr.s_addr = inet_addr("192.168.1.40");
	server.sin_addr.s_addr = inet_addr("192.168.1.99");


	cout << "begin connect" <<endl;

	//连接服务器

	int conRes = connect(s,(struct sockaddr *)&server, sizeof(server));

	if(conRes < 0)
	{
		cout << "connect error !!" << endl;
		cout << "errorno is :  " << errno << endl;
		return 0;
	}

	cout << "connect res: " << conRes << endl;

	memset(buff, 0, BUFFLEN);

	strcpy(buff, "TIME");

	
	
	
	struct PacketHead packetHead;
	packetHead.packetID = 0;
	
	smart::test msg1; 
	msg1.set_age(101); 
	msg1.set_name("nice to meet you!");
	msg1.set_email("1017234");

	std::string bufStr;
	msg1.SerializeToString(&bufStr);
        packetHead.packetLen = bufStr.size();
	char * sendData = (char *)malloc(packetHead.packetLen + 8);
	
		

	memcpy(sendData, &packetHead, 8);
	//cout << "now head is: " << sendData << endl;
    	memcpy(sendData + 8, bufStr.c_str(), packetHead.packetLen);
	cout << "msg len is : " << packetHead.packetLen << endl;
	//发送数据

	int sendLen = send(s, sendData, sizeof(packetHead) + packetHead.packetLen, 0);

	cout << "send datalen: " <<  sendLen << endl;
    //cout << "send data is : " << sendData << endl;	
	
 	
	
	

	
	return NULL;
}


	
int main(int argc, char * argv[])
{

	//Linux一个进程最多开辟1024线程
	for(int i = 0; i < 1000; i++)
	{
		pthread_t tid;
		int error = pthread_create(&tid, NULL, create, NULL);
		if(error!=0)
		{
			printf("pthread_create is created is not created ... ");
			return -1;
		}
		
		//sleep(1);
		usleep(500000);
		//pthread_join(tid, NULL);
	}
	
	
	
	
	//cout << "send data is : " << sendData2 << endl;
	   
	
	getchar();
	
	

	return 0;	
}
