#ifndef _JAPPLICATION_H_
#define _JAPPLICATION_H_

namespace JFC
{

class JWindow;

//一种特殊的单例模式，模仿QT
class JApplication			//应用程序类
{
public:
	JApplication();
	~JApplication();

	static JApplication* Instance() { return self_; }

	//显示光标
	void ShowCursor() const;
	//隐藏光标
	void HideCursor() const;

	JWindow* GetCurrent() const { return current_; }
	void SetCurrent(JWindow* win) { current_ = win; }
	JWindow* Root() const { return root_; }

	int Exec();			//消息循环

private:
	static JApplication* self_; 
	JWindow* current_;	//当前接收按键的窗口
	JWindow* root_;		//根
	
};

#define jApp JApplication::Instance()

}


#endif //_JAPPLICATION_H_
