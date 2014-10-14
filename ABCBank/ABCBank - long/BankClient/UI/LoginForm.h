#ifndef _LOGINFORM_H_
#define _LOGINFORM_H_

#include "../JFC/JForm.h"
#include "../JFC/JButton.h"
#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "BankFrom.h"

using namespace JFC;

namespace UI
{

class LoginForm : public BankFrom
{
public:
	LoginForm(SHORT x, SHORT y, SHORT width, SHORT height,
		JWindow* parent = jApp->Root());

	~LoginForm();

	virtual void Draw();

	virtual void OnKeyEvent(JEvent* e);

private:
	//void DrawBorder();
	void Login();	//login��Ŧ
	void Exit();	//exit��Ŧ
	void Rest();

	JEdit* editUser_;
	JLabel* lblUser_;
	JLabel* lblUserTip_;

	JEdit* editPass_;
	JLabel* lblPass_;
	JLabel* lblPassTip_;

	JButton* btnLogin_;		//�Ӵ��ڣ���Ŧ����
	JButton* btnExit_;

};

}


#endif //_LOGINFORM_H_