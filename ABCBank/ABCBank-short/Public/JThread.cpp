#include "JThread.h"

#include <process.h>

using namespace PUBLIC;

JThread::JThread() : hThread_(NULL), threadId_(0), autoDel_(false)
{
}

JThread::~JThread()
{
	if (hThread_ != NULL)
		CloseHandle(hThread_);
}

void JThread::SetAutoDel(bool autoDel)
{
	autoDel_ = autoDel;
}

unsigned __stdcall JThread::ThredFun(void* p)
{
	JThread* jtp = (JThread*)p;
	jtp->Run();
	if (jtp->autoDel_)
	{
		delete jtp;
	}
	return 0;
}

bool JThread::Start()
{
	hThread_ = (HANDLE)_beginthreadex(NULL, 0, ThredFun, this, 0, &threadId_);
	return hThread_ != NULL;
}

void JThread::Wait(DWORD timeout)
{
	WaitForSingleObject(hThread_, timeout);
}