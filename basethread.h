# ifndef _BASE_THREAD_H_
#define _BASE_THREAD_H_

#ifndef _WIN32
#include <pthread.h>
#endif

#include <Winsock2.h>
#include <process.h>
#include <structtype.h>

class BaseThread
{
public:
	BaseThread():m_nId(0),m_nShutDown(0){}
	virtual ~ BaseThread();
	void startup(unsigned int stackSize = 1024 * 1024 * 2);
	void join();
	void testFunc();
	virtual void threadWorkFunc(void)=0;
protected:
	UInt32 m_nId;

private:

#if defined _WIN32
	HANDLE m_hThread;
#endif

	
	UInt32 m_nShutDown;

};

#endif