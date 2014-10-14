#ifndef _JFORM_H_
#define _JFORM_H_

#include "JWindow.h"
#include "JEvent.h"

namespace JFC
{

	class JForm : public JWindow		//窗口类容器，包括多个窗口
{
public:
	JForm();
	JForm(SHORT x, SHORT y, SHORT width, SHORT height,
		JWindow* parent = jApp->Root());
	virtual ~JForm();

	virtual void OnKeyEvent(JEvent* e);	//按键事件处理函数

	virtual void Draw();
};

}

#endif //_JFORM_H_