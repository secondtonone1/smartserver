#include "CLock.h"

//在构造函数里创建锁
Mutex::Mutex()
{
	#if defined _WIN32
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	#endif
}

//析构函数里销毁锁
Mutex::~ Mutex()
{
	#if defined _WIN32
		::CloseHandle(m_hMutex);
	#endif
}

//互斥锁上锁
void Mutex::lock() const
{
	#if defined _WIN32
	  DWORD d = WaitForSingleObject(m_hMutex, INFINITE);
	#endif
}

//互斥锁解锁
void Mutex::unlock() const
{
	#if defined _WIN32
	  ::ReleaseMutex(m_hMutex);
	#endif
}