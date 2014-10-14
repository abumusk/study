#include "JApplication.h"

#include "JEvent.h"
#include "JWindow.h"

#include <conio.h>

using namespace JFC;

JApplication* JApplication::self_ = 0;

JApplication::JApplication()
{
	if (self_)
		return;
	
	self_ = this;

	SetConsoleOutputCP(936);		//设置控制台支持中文字符
	COORD size = {80, 25};
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);	//设置控制台大小 (80列25行) 未设置大小直接运行控制台的大小是不止25行的

	HideCursor();
	root_ = new JWindow(0, 0, 80, 25, 0);
	root_->Show();

}

JApplication::~JApplication()
{
	delete root_;
}

//显示光标
void JApplication::ShowCursor() const
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);	//获取标出输出句柄
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hout, &cci);		//获取光标
	cci.bVisible = true;
	SetConsoleCursorInfo(hout, &cci);		//设置光标
}

//隐藏光标
void JApplication::HideCursor() const
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hout, &cci);
	cci.bVisible = false;
	SetConsoleCursorInfo(hout, &cci);
}

int JApplication::Exec()
{
	while (1)
	{
		int key = _getch();
		if (key == 224)			//特殊按键		上、 下、 左、 右、 F11、 F12、 home、 pg up、 pg down、 end、 delete、 insert
		{
			key = (224 << 8) + _getch();
		}
		else if (key == 0)		//特殊键	F1~F10	这两种特殊按键需要在次调用_getch()函数来接收走事件
		{
			key = -1 * _getch();
		}

		if (current_)
		{
			JEvent e(key, current_);
			current_->OnKeyEvent(&e);
		}
	}
	return 0;
}