# ifndef _BASELOCK_H_
# define _BASELOCK_H_

#include <windows.h>

class BaseLock
{
public:
	BaseLock(){}
	virtual ~BaseLock(){}
	virtual void lock() const = 0 ;
	virtual void unlock() const = 0 ;
};

class Mutex :public BaseLock
{
public:
	Mutex();
	~Mutex();
	virtual void lock() const;
	virtual void unlock() const;
private:
#if defined _WIN32
	HANDLE m_hMutex;
#endif
};

//Îö¹¹º¯Êý
class CLock
{
public:
	CLock(const BaseLock & baseLock):m_cBaseLock(baseLock){
		m_cBaseLock.lock();
	}
	~CLock(){
		m_cBaseLock.unlock();
	}
private:
	const BaseLock& m_cBaseLock;
};







#endif 