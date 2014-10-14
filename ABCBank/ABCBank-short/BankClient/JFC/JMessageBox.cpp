#include "JMessageBox.h"
#include "JButton.h"
#include "JApplication.h"
#include <conio.h>


using namespace JFC;

int JMessageBox::Show(const std::string& title,
				const std::string& text,
				std::vector<std::string>& buttons)
{
	short btn_distance = static_cast<short>(buttons.size()) - 1;	//计算按纽之间的间隔

	for (int i=0; i<static_cast<int>(buttons.size()); ++i)
		btn_distance += buttons[i].length() + 4;		//按纽的宽度=文本长度+4

	short width = 4 + btn_distance;		//最小左边距=2，最小右边距=2

	if (width < 40)
		width = 40;			//MessageBox最小宽度为40

	//每行字符个数width-6
	//计算得出文体行数
	short lines = (text.length() + width-6 -1) / (width-6);// +width-6 -1可以得出零头的一行
	//short lines = (text.length() % (width -6)) ? (text.length() / (width-6)+1 : (text.length() / (width-6)));

	short height = lines + 9;	//得出消息框的高度

	short x = (80 - width) / 2;		//计算消息框的居中位置坐标
	short y = (25 - height) / 2;

	JMessageBoxImpl jmbl(x, y, width, height, title, text, buttons, btn_distance, lines);
	JWindow* win = jApp->GetCurrent();	//保存当前按纽

	JRECT rect = {x, y, x+width-1, y+height-1};
	ScreenSaver sc(rect);
	sc.Dump();			//保存被消息框遮盖部分的屏幕数据
	jmbl.Show();
	int ret = jmbl.Exec();
	sc.Restore();			//将保存的屏幕数据还原
	win->SetCurrent();		//还原当前按纽

	return ret;
}

JMessageBoxImpl::JMessageBoxImpl(SHORT x, SHORT y, SHORT width, SHORT height,
				const std::string& title, const std::string& text,
				std::vector<std::string>& buttons, SHORT btnDistance,
				int lines, JWindow* parent_)
	: JWindow(x, y, width, height),
	  title_(title),
	  text_(text),
	  btnDistance_(btnDistance),
	  lines_(lines)
{
	short btnX = x + (width_ - btnDistance_) / 2;
	short btnY = y + height_ - 5;

	for (int i=0; i<static_cast<int>(buttons.size()); ++i)
	{
		JButton* btn = new JButton(btnX, btnY, buttons[i].length()+4, 3, buttons[i], this);
		btnX += buttons[i].length()+4+1;
	}
}

void JMessageBoxImpl::Draw()
{
	ClearWindow();	//清楚窗口

	SetTextColor(FCOLOR_YELLO);
	SetBkColor(BCOLOR_MAGENTA);
	DrawHLine(0, 0, Width()-1, '-');		//绘制边框
	DrawHLine(Height()-1, 0, Width()-1, '-');
	DrawVLine(0, 1, Height()-2, ' ');
	DrawVLine(Width()-1, 1, Height()-2, ' ');

	//刷新边框之中的背景色
	SetBkColor(BCOLOR_CYAN);
	JRECT rect = { 1, 1, Width()-2, Height()-2};
	FillRect(rect);

	//绘制title文本
	DrawText(2, 1, title_);

	//绘制字符串
	if (lines_ == 1)
		DrawText(Width()/2-text_.length()/2, 3, text_);
	else
	{
		for (int i=0; i<lines_; ++i)
		{
			std::string s;
			if (i == lines_ - 1)
				s = text_.substr(i*(Width()-6));	//指定长度的字符串复制
			else
				s = text_.substr(i*(Width()-6), width_-6);	//从一个字符串复制一个从指定位置开始，并具有指定长度的子字符串。
			DrawText(3, 3+i, s);
		}
	}

	//绘制JButton类，就是按纽
	std::vector<JWindow*>::iterator it;
	for (it = childs_.begin(); it!=childs_.end(); ++it)
	{
		(*it)->Draw();
	}
}

int JMessageBoxImpl::Exec()
{
	int pos = 0;
	while (1)
	{
		int key = _getch();
		if (key == 224)
		{
			key = (key << 8) + _getch();
		}

		JWindow* p;
		JWindow* win = jApp->GetCurrent();

		switch (key)
		{
		case KEY_ESC:
			return -1;
		case KEY_ENTER:
			return pos;
		case KEY_LEFT:
		case KEY_UP:
			p = FindPrev(jApp->GetCurrent());
			p->SetCurrent();
			p->Draw();
			p->Refresh();
			//p->Show();

			win->Draw();
			win->Refresh();
			pos--;
			if (pos < 0)
				pos = childs_.size() - 1;
			break;
		case KEY_RIGHT:
		case KEY_TAB:
		case KEY_DOWN:
			p = FindNext(jApp->GetCurrent());
			p ->SetCurrent();
			p->Draw();
			p->Refresh();
			//p->Show();

			win->Draw();
			win->Refresh();
			pos++;
			if (pos == childs_.size())
				pos = 0;
			break;
		default:
			break;
		}
	}
}
