#include "JButton.h"

using namespace JFC;

JButton::JButton()
	:JWindow(0, 0, 0, 0,0)
{
	SetCanFocus(true);
}

JButton::JButton(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& text, 
		JWindow* parent)
		: JWindow(x, y, width, height, parent),
		text_(text)
{
	SetCanFocus(true);
}

JButton::~JButton()
{
}

void JButton::Draw()
{
	if (IsCurrent())			//如果是当前窗口设置焦点，
	{
		SetTextColor(FCOLOR_WHITE);
		SetBkColor(BCOLOR_RED);
		jApp->HideCursor();
	}
	else						//如果不是当前窗品设置焦点，就是按钮的选中和非选中的区别
	{
		SetTextColor(FCOLOR_BLUE);
		SetBkColor(BCOLOR_GRAY);
	}

	JRECT rect = { 0, 0, Width()-1, Height()-1 };
	FillRect(rect);		//绘制背景
	DrawText(text_);	//绘制文字
}