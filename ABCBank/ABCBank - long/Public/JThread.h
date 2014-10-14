#ifndef _JTHREAD_H_
#define _JTHREAD_H_

#define WIN32_LEAN_AND_MEAN	//这句的目的是指示编译器不要包含与MFC相关的操作否则会出现很多重定义
#include <windows.h>

namespace PUBLIC
{

class JThread
{
public:
	JThread();
	virtual ~JThread();

	virtual void Run() = 0;

	static unsigned __stdcall ThredFun(void* p);
	bool Start();

	void Wait(DWORD timeout = INFINITE);
	void SetAutoDel(bool autoDel);

private:
	HANDLE hThread_;
	unsigned threadId_;
	bool autoDel_;
};

}

#endif //_JTHREAD_H_