#ifndef _JEVENT_H_
#define _JEVENT_H_

namespace JFC
{

class JWindow;

class JEvent	//事件类
{
public:
	JEvent(int code, JWindow* sender)
		: code_(code),
		  sender_(sender),
		  done_(false)
	{

	}

	JWindow* GetSender() const { return sender_; }
	int GetEventCode() const { return code_; }
	void Done() { done_ = true; }
	bool IsDone() const { return done_ == true; }

private:	
	int code_;				//按键
	JWindow* sender_;		//当前窗口
	bool done_;				//是否处理完事件
};

}

#endif //_JEVENT_H_