#ifndef _QUERY_ACCOUNT_HISTORY_BILL_FORM_H_
#define _QUERY_ACCOUNT_HISTORY_BILL_FORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{

class QueryAccountHistoryBillForm : public BankFrom
{
public:
	QueryAccountHistoryBillForm();
	QueryAccountHistoryBillForm(SHORT x, SHORT y, SHORT width, SHORT height);

	~QueryAccountHistoryBillForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

private:
	void Reset();
	void Query();

	void EscCancel(JEvent* e);


	JEdit* editAccountId_;
	JEdit* editBeginDate_;
	JEdit* editEndDate_;

	JButton* btnQuery_;
	JButton* btnCancel_;
};

}

#endif //_QUERY_ACCOUNT_HISTORY_BILL_FORM_H_