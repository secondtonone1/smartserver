#include "networksystem.h"

#ifdef _linux_
#include<arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"WS2_32.LIB")
#endif


void NetWorkSystem::tcpread_cb(struct bufferevent *bev, void *ctx)
{
	getSingleton().dealReadEvent(bev, ctx);
}

void NetWorkSystem::dealReadEvent(struct bufferevent *bev, void *ctx)
{

	//
	evutil_socket_t  bufferfd = bufferevent_getfd(bev);
	std::map<evutil_socket_t, TcpHandler *>::iterator tcpHandlerIter = m_mapTcpHandlers.find(bufferfd);
	if(tcpHandlerIter != m_mapTcpHandlers.end())
	{
		tcpHandlerIter->second->dealReadEvent();
	}
}

void
NetWorkSystem::listener_read_cb(evutil_socket_t fd, short what, void *p)
{
	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(sockaddr_in));
	size_t addrlen = sizeof(sockaddr_in);
	evutil_socket_t acceptres = accept(fd, (sockaddr *)&serveraddr, (int *)&addrlen);
	if(acceptres == -1)
	{
		cout << "accept failed !" <<endl;
	}

	event_base * eventbase =(event_base *) p;
	bufferevent * buffersocket = bufferevent_socket_new(eventbase,  acceptres, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(buffersocket, NetWorkSystem::tcpread_cb, NetWorkSystem::tcpwrite_cb, NetWorkSystem::tcperror_cb, eventbase);
	bufferevent_enable(buffersocket, EV_READ);
	
	
	cout << "new connection arrived: "<< inet_ntoa(serveraddr.sin_addr) << endl;
 

	getSingleton().addConnection(acceptres, buffersocket);

	

}

void NetWorkSystem::tcpwrite_cb(struct bufferevent *bev, void *ctx)
{

}

void NetWorkSystem::tcperror_cb(struct bufferevent *bev, short what, void *ctx)
{

}


bool NetWorkSystem::initial()
{
	#ifdef WIN32
		WSADATA wsa_data;
		WSAStartup(0x0201, &wsa_data);
	#endif
	m_pEvent_base = event_base_new();
	m_nListenfd =  socket(AF_INET, SOCK_STREAM, 0);

	if(m_nListenfd == -1)
	{
		return false;
	}

	sockaddr_in listenAddr;
	memset(&listenAddr, 0, sizeof(listenAddr) );

	listenAddr.sin_addr.s_addr = inet_addr("192.168.1.99");
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(9995);
	evutil_make_socket_nonblocking(m_nListenfd);
	int one = 1;  
	setsockopt(m_nListenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)); 

	int bindres = bind(m_nListenfd, (sockaddr *)&listenAddr, sizeof(listenAddr));
	if(bindres == -1)
	{
		return false;
	}



	listen(m_nListenfd, 128);


	m_pListenevent = event_new(m_pEvent_base, m_nListenfd, EV_READ|EV_PERSIST, NetWorkSystem::listener_read_cb,m_pEvent_base);
	event_add(m_pListenevent, NULL);

	return true;
}


void NetWorkSystem::run()
{
	event_base_dispatch(m_pEvent_base);
}

void NetWorkSystem::release()
{
	event_base_free(m_pEvent_base);
	event_free(m_pListenevent);
	evutil_closesocket(m_nListenfd);
}

void NetWorkSystem::addConnection(evutil_socket_t fd, bufferevent * bev)
{
	m_nConnId ++;
	TcpHandler * tcpHandler = new TcpHandler(m_nConnId, bev);
	if(!tcpHandler)
	{
		free(bev);
		bev = NULL;
	}

	m_mapTcpHandlers.insert(pair<evutil_socket_t,  TcpHandler*>(fd, tcpHandler));
	
	cout <<"connectionid is:  " <<  m_nConnId << endl;
}