#ifndef _JWINDOW_H_
#define _JWINDOW_H_

#include "JApplication.h"
#include "JWindowBase.h"

#include <vector>

namespace JFC
{

class JEvent;

class JWindow : public JWindowBase		//窗口类
{
public:
	JWindow();
	JWindow(SHORT x, SHORT y, SHORT width, SHORT height,
		JWindow* parent = jApp->Root());
	virtual ~JWindow();

	void AddChild(JWindow* win);
	void DelChild(JWindow* win);

	virtual void OnKeyEvent(JEvent* e);
	virtual void Draw();

	void Show();

	void SetCurrent();		//设置当前窗口
	bool IsCurrent() const { return this == jApp->GetCurrent(); }	//是否是当前窗口
	void SetCanFocus(bool canFocus) { canFocus_ = canFocus; }

	JWindow* FindFirst();				//查找第一个能设置焦点的子窗口
	JWindow* FindLast();				//查找最后一个能设置焦点的子窗口
	JWindow* FindPrev(JWindow* win);	//查找win窗口前一个能设置焦点的子窗口
	JWindow* FindNext(JWindow* win);	//查找win窗口下一个能设置焦点的子窗口

protected:
	JWindow* parent_;				//本窗口的父窗口，每个窗口都有一个父窗口，除根窗口
	std::vector<JWindow*> childs_;	//子窗口列表，每个窗口可以有多个子窗口
	bool canFocus_;					//是否可以具有焦点的窗口
};

}

#endif //_JWINDOW_H_