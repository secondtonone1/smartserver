#include "basethread.h"
#include "assert.h"

#if defined _WIN32
#include <process.h>
unsigned _stdcall threadFunc (void * param)
{
	Sleep(1000);
	assert(param);
	BaseThread * baseThread = static_cast<BaseThread *>(param);
	assert(baseThread);
	baseThread->threadWorkFunc();
	//cout << "hello world!" <<endl;
#if defined _WIN32
	_endthreadex(0);
#endif
	return 0;
}

#endif



void BaseThread::startup(UInt32 stackSize)
{
	assert(m_nId == 0);
	#if defined _WIN32
	m_hThread =(HANDLE) _beginthreadex(NULL,0,threadFunc, this, 0, &m_nId);
	//cout << this <<endl;
	 ::SetThreadPriority(::GetCurrentThread(), 2);
	//让线程跑起来后再退出函数
	// Sleep(1000);
	
	#endif
}

BaseThread::~BaseThread()
{
	m_nShutDown = 1;
	join();
}

void BaseThread::join()
{
	#if defined _WIN32
	 DWORD exitCode;
	 while(1)
	 {
		if(GetExitCodeThread(m_hThread, &exitCode) != 0)
		{
			if(exitCode != STILL_ACTIVE)
			{
				break;

			}
			else
			{
				// wait之前， 需要唤起线程， 防止线程处于挂起状态导致死等
				ResumeThread(m_hThread);
				WaitForSingleObject(m_hThread, INFINITE);
			}
		}
		else
		{
			break;
		}
	 }
	
	 CloseHandle(m_hThread);
	#endif

	 m_nId = 0;
}


void BaseThread::testFunc()
{
	this->threadWorkFunc();
}
