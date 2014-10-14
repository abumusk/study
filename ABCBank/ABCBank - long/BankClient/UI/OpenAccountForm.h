#ifndef _OPNEACCOUNTFORM_H_
#define _OPNEACCOUNTFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


class OpenAccountForm : public BankFrom
{
public:
	OpenAccountForm();
	OpenAccountForm(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& title);

	~OpenAccountForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

private:
	void Reset();
	void Submit();

	void EscCancel(JEvent* e);

	std::string title_;

	JLabel* lblName_;
	JEdit* editName_;
	JLabel* lblNameTip_;

	JLabel* lblId_;
	JEdit* editId_;
	JLabel* lblIdTip_;

	JLabel* lblPass_;
	JEdit* editPass_;
	JLabel* lblPassTip_;

	JLabel* lblPass2_;
	JEdit* editPass2_;
	JLabel* lblPassTip2_;

	JLabel* lblMoney_;
	JEdit* editMoney_;
	JLabel* lblMoneyTip_;

	JButton* btnSubmit_;
	JButton* btnReset_;
	JButton* btnCancel_;
};

}

#endif //_OPNEACCOUNTFORM_H_