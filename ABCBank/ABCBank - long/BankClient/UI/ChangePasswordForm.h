#ifndef _CHANGEPASSWORDFORM_H_
#define _CHANGEPASSWORDFORM_H_

//class ChangePasswordForm
//{
//public:
//	ChangePasswordForm();
//	~ChangePasswordForm();
//};

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


class ChangePasswordForm : public BankFrom
{
public:
	ChangePasswordForm();
	ChangePasswordForm(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& title);

	~ChangePasswordForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

private:
	void Reset();
	void Submit();

	void EscCancel(JEvent* e);

	std::string title_;

	JLabel* lblAccountId_;
	JEdit* editAccountId_;
	JLabel* lblAccountIdTip_;

	JLabel* lblPass_;
	JEdit* editPass_;
	JLabel* lblPassTip_;

	JLabel* lblNewPass_;
	JEdit* editNewPass_;
	JLabel* lblNewPassTip_;

	JLabel* lblNewPass2_;
	JEdit* editNewPass2_;
	JLabel* lblNewPassTip2_;

	JButton* btnSubmit_;
	JButton* btnReset_;
	JButton* btnCancel_;
};

}

#endif //_CHANGEPASSWORDFORM_H_