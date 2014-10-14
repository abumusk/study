#ifndef _BALANCEINQUIRYFORM_H_
#define _BALANCEINQUIRYFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


class BalanceInquiryForm : public BankFrom
{
public:
	BalanceInquiryForm();
	BalanceInquiryForm(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& title);

	~BalanceInquiryForm();

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

	JButton* btnSubmit_;
	JButton* btnReset_;
	JButton* btnCancel_;
};

}
#endif //_BALANCEINQUIRYFORM_H_