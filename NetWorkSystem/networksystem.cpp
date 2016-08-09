#include "networksystem.h"

#ifdef _linux_
#include<arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"WS2_32.LIB")
#endif


NetWorkSystem::NetWorkSystem(){

	m_nListenfd = 0;
	m_pEvent_base = 0;
	m_nConnId = 0;
	for(int i = 0; i < 2; i++)
	{
		m_pNetWorkers[i] = new NetWorker();	
		m_pNetWorkers[i]->startup();
	}
}

NetWorkSystem::~NetWorkSystem()
{
	m_nListenfd = 0;
	m_pEvent_base = 0;
	m_nConnId = 0;
	for(int i = 0; i < 2; i++)
	{
		delete m_pNetWorkers[i];
		m_pNetWorkers[i] = NULL;
	}


}

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
		//调用应用层消息处理
		const UInt64 &connId  =  tcpHandlerIter->second->getConnId();
		m_pNetWorkers[connId%2]->pushNodeInStream(tcpHandlerIter->second);
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
	cout << "write success !!!" <<endl;
}

//出现连接错误的时候要清除数据，释放tcphandler
void NetWorkSystem::tcperror_cb(struct bufferevent *bev, short what, void *ctx)
{
	
	getSingleton().eraseConnection(bev);
}

void NetWorkSystem::eraseConnection( bufferevent * bev)
{
	evutil_socket_t  bufferfd = bufferevent_getfd(bev);
	std::map<evutil_socket_t, TcpHandler *>::iterator tcpHandlerIter = m_mapTcpHandlers.find(bufferfd);
	if(tcpHandlerIter != m_mapTcpHandlers.end())
	{
		if(tcpHandlerIter->second)
		{
				UInt64 connId = tcpHandlerIter->second->getConnId();
				m_mapConHandlers.erase(connId);
				delete(tcpHandlerIter->second);
				tcpHandlerIter->second = NULL;

				cout << "connection id : "<< connId <<" erase!" << endl;
		}
	
		m_mapTcpHandlers.erase(tcpHandlerIter);	
	}

	
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
	for(std::map<evutil_socket_t, TcpHandler *>::iterator mapIter = m_mapTcpHandlers.begin();
		mapIter != m_mapTcpHandlers.end();  mapIter ++)
	{
		if(mapIter->second)
		{
			delete(mapIter->second);
			mapIter->second = NULL;
		}
		
	}

	m_mapTcpHandlers.clear();
	
	m_mapConHandlers.clear();
}

TcpHandler * NetWorkSystem::getHandlerByConnId(UInt64 connId)
{
	std::map<UInt64, TcpHandler *>::iterator  findIter = m_mapConHandlers.find(connId);
	if(findIter == m_mapConHandlers.end())
	{
		return NULL;
	}

	return findIter->second;
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

	m_mapConHandlers.insert(pair<UInt64, TcpHandler *>(m_nConnId, tcpHandler));

	cout <<"connectionid is:  " <<  m_nConnId << endl;
}