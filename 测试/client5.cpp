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

int recvCount = 0;

struct PacketHead
{
	int packetID;
	int packetLen;
};

void *sendf(void *arg)
{
    
	int * pSocket = (int *) arg;
	int s = *pSocket;
	
	while(1)
	{
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
//	cout << "msg len is : " << packetHead.packetLen << endl;
	//发送数据

	
	int sendLen = send(s, sendData, sizeof(packetHead) + packetHead.packetLen, 0);

//	cout << "send datalen: " <<  sendLen << endl;
	usleep(500000);
	}
	
    //cout << "send data is : " << sendData << endl;	
	
 	
	return NULL;
}

void * recvf(void *arg)
{
	int * pSocket = (int *) arg;
	int s = *pSocket;
	char packethead[8]={0};

	cout << "........................."<<endl;
	while(1)
	{
		int offset = 0;
		int recvlen = recv(s,packethead,8-offset,0);
		if(recvlen == -1 || recvlen < 0)
		{
			cout << "error!!!" <<endl;
			continue;
		}
		
		
		while(recvlen < 8-offset)
		{
			offset += recvlen;
			recvlen = recv(s,packethead,8-offset,0);
		}

		cout << "offset is : " << offset <<endl;
		cout << "recvlen is : " << recvlen <<endl;
		cout << "recv head len is : " << recvlen + offset <<endl;
		
		struct PacketHead packetHead;
		memcpy(&packetHead, &packethead, 8);
		cout << "packet ID: " << packetHead.packetID << endl;
		cout << "packet Len: " << packetHead.packetLen << endl;
		
		offset = 0;
		char * recvData = (char *)malloc((packetHead.packetLen+1) * sizeof(char));

		int recvpacketlen = recv(s, recvData, packetHead.packetLen, 0);

		while(recvpacketlen < 0)
		{
			recvpacketlen = recv(s, recvData, packetHead.packetLen, 0);
		}


		while(recvpacketlen < packetHead.packetLen - offset)
		{
						
			offset += recvpacketlen;
			recvpacketlen = recv(s, recvData, packetHead.packetLen - offset, 0);
			
		}

		cout << "+++++++++++++++++++++++++++++++++++++++"<<endl;
		recvData[packetHead.packetLen] = '\0';
		std::string bufStr(recvData);
		smart::test msg1; 
		msg1.ParseFromString(bufStr);
		
		cout <<"age is : " << msg1.age() <<endl;
		cout << "name is : " << msg1.name() << endl;
		cout << "email is : " << msg1.email() << endl;
		
		free(recvData);

		cout << "recvCount is : " << ++recvCount <<endl;
		
	}	
		
		
}

	
int main(int argc, char * argv[])
{

	int s;

	char buff[BUFFLEN];

	int n = 0;

	//建立套接字
	s = socket(AF_INET, SOCK_STREAM, 0);


	//初始化服务器地址
	struct sockaddr_in server;

	memset(&server, -1, sizeof(server));

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


	
	//Linux一个进程最多开辟1024线程
	cout << "begin tid " <<endl;	
		pthread_t tid;
		int error = pthread_create(&tid, NULL, sendf, &s);
		if(error!=0)
		{
			printf("pthread_create is created is not created ... ");
			return -1;
		}
		

		
		
		cout << "begin tid2" <<endl;
		pthread_t tid2;
		int error2 = pthread_create(&tid2, NULL, recvf, &s);
		if(error2!=0)
		{
			printf("pthread_create is created is not created ... ");
			return -1;
		}
		

		pthread_join(tid, NULL);
		pthread_join(tid2, NULL);
		
	
	
	
	
	//cout << "send data is : " << sendData2 << endl;
	   
	
	getchar();
	
	

	return 0;	
}
