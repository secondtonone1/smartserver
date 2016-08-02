#include "CLock.h"

Mutex::Mutex()
{
	#if defined _WIN32
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	#endif
}

Mutex::~ Mutex()
{
	#if defined _WIN32
		::CloseHandle(m_hMutex);
	#endif
}

void Mutex::lock() const
{
	#if defined _WIN32
	  DWORD d = WaitForSingleObject(m_hMutex, INFINITE);
	#endif
}

void Mutex::unlock() const
{
	#if defined _WIN32
	  ::ReleaseMutex(m_hMutex);
	#endif
}