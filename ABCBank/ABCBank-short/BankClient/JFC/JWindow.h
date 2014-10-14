#ifndef _JWINDOW_H_
#define _JWINDOW_H_

#include "JApplication.h"
#include "JWindowBase.h"

#include <vector>

namespace JFC
{

class JEvent;

class JWindow : public JWindowBase		//������
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

	void SetCurrent();		//���õ�ǰ����
	bool IsCurrent() const { return this == jApp->GetCurrent(); }	//�Ƿ��ǵ�ǰ����
	void SetCanFocus(bool canFocus) { canFocus_ = canFocus; }

	JWindow* FindFirst();				//���ҵ�һ�������ý�����Ӵ���
	JWindow* FindLast();				//�������һ�������ý�����Ӵ���
	JWindow* FindPrev(JWindow* win);	//����win����ǰһ�������ý�����Ӵ���
	JWindow* FindNext(JWindow* win);	//����win������һ�������ý�����Ӵ���

protected:
	JWindow* parent_;				//�����ڵĸ����ڣ�ÿ�����ڶ���һ�������ڣ���������
	std::vector<JWindow*> childs_;	//�Ӵ����б�ÿ�����ڿ����ж���Ӵ���
	bool canFocus_;					//�Ƿ���Ծ��н���Ĵ���
};

}

#endif //_JWINDOW_H_