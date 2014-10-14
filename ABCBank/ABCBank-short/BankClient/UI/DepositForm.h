#ifndef _DEPOSITFORM_H_
#define _DEPOSITFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


class DepositForm : public BankFrom
{
public:
	DepositForm();
	DepositForm(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& title);

	~DepositForm();

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

	JLabel* lblMoney_;
	JEdit* editMoney_;
	JLabel* lblMoneyTip_;

	JButton* btnSubmit_;
	JButton* btnReset_;
	JButton* btnCancel_;
};

}

#endif //_DEPOSITFORM_H_
