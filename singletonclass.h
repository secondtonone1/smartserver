#ifndef _SINGLETON_CLASS_H_
#define _SINGLETON_CLASS_H_

template <class Type>
class Singleton
{
protected :
	Singleton(){}

public:
	static Type & getSingleton()
	{		
		return singleton;
	}

private:
		
	Singleton(const Singleton & temp){
		singleton = temp.singleton;
	}

private:
	static Type singleton;
};

template <class Type>
Type Singleton<Type>::singleton;

#endif