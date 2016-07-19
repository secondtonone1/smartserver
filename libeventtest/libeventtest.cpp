// libeventtest.cpp : Defines the entry point for the console application.
//

// LibeventTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#ifndef WIN32
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"

#include <WinSock2.h>
#include <string.h>
#include <iostream>
using namespace std;

static void
listener_read_cb(evutil_socket_t fd, short what, void *p)
{
	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(sockaddr_in));
	size_t addrlen = sizeof(sockaddr_in);
	int acceptres = accept(fd, (sockaddr *)&serveraddr, (int *)&addrlen);
	if(acceptres == -1)
	{
		/*int errorNum = WSAGetLastError();
		cout << "accept error!!!" <<errorNum<<endl;*/
	}
	event_base * eventbase =(event_base *) p;
	bufferevent_socket_new(eventbase,  fd, BEV_OPT_CLOSE_ON_FREE);
}


int main(int argc, char **argv)
{

#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
    event_base * myevent_base = event_base_new();
	evutil_socket_t  listenfd =  socket(AF_INET, SOCK_STREAM, 0);

	if(listenfd == -1)
	{
		return -1;
	}

	sockaddr_in listenAddr;
	memset(&listenAddr, 0, sizeof(listenAddr) );

	listenAddr.sin_addr.s_addr = inet_addr("192.168.1.99");
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(9995);
	evutil_make_socket_nonblocking(listenfd);
	int one = 1;  
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)); 

	int bindres = bind(listenfd, (sockaddr *)&listenAddr, sizeof(listenAddr));
	if(bindres == -1)
	{
		return -1;
	}

	

	listen(listenfd, 6);

	event* listenevent = event_new(myevent_base, listenfd, EV_READ|EV_PERSIST, listener_read_cb,myevent_base);
	event_add(listenevent, NULL);
	event_base_dispatch(myevent_base);

	event_base_free(myevent_base);
	event_free(listenevent);
	evutil_closesocket(listenfd);
	
	getchar();

    return 0;
}

