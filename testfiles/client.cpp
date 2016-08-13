#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#define BUFFLEN 1024
#define SERVER_PORT 9995

#include <iostream>
using namespace std;

struct PacketHead
{
	int packetID;
	int packetLen;
};


int main(int argc, char * argv[])
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
		return 0;
	}

	cout << "connect res: " << conRes << endl;

	memset(buff, 0, BUFFLEN);

	strcpy(buff, "TIME");

	char sendData[19] = {0};
	
	struct PacketHead packetHead;
	packetHead.packetID = 55;
        packetHead.packetLen = 10;
		
	char msg[] = "1234567890";
	memcpy(sendData, &packetHead, 8);
	//cout << "now head is: " << sendData << endl;
        memcpy(sendData + 8, msg, strlen(msg));
	cout << "msg len is : " << strlen(msg) << endl;
	//发送数据

	int sendLen = send(s, sendData, sizeof(packetHead) + strlen(msg), 0);

	cout << "send datalen: " <<  sendLen << endl;
        //cout << "send data is : " << sendData << endl;	
	struct PacketHead packetHead2;
	packetHead2.packetID = 77;
        packetHead2.packetLen = 15;
	char sendData2[24] = {0};
        memcpy(sendData2, &packetHead2, 8);
        char msg2[] = "abcdefghijklmno";
 	memcpy(sendData2 + 8, msg2, strlen(msg2));
	cout << "msg len is: " << strlen(msg2) << endl;
	int sendLen2 = send(s, sendData2, sizeof(packetHead) + strlen(msg2), 0);
	cout << "send data len 2: " << sendLen2 << endl;
	//cout << "send data is : " << sendData2 << endl;
        memset(buff, 0, sizeof(buff));
	n = recv(s, buff, BUFFLEN, 0);
	if(n > 0)
	{
		printf("Time: %s", buff);
        }
	close(s);

	return 0;	
}
