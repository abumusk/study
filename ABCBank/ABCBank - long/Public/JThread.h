#ifndef _JTHREAD_H_
#define _JTHREAD_H_

#define WIN32_LEAN_AND_MEAN	//����Ŀ����ָʾ��������Ҫ������MFC��صĲ����������ֺܶ��ض���
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