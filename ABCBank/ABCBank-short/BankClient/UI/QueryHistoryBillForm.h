#ifndef _QUERYHISTORYBILLFORM_H_
#define _QUERYHISTORYBILLFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{

class QueryHistoryBillForm : public BankFrom
{
public:
	QueryHistoryBillForm();
	QueryHistoryBillForm(SHORT x, SHORT y, SHORT width, SHORT height);

	~QueryHistoryBillForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

private:
	void Reset();
	void Query();

	void EscCancel(JEvent* e);


	JEdit* editBeginDate_;
	JEdit* editEndDate_;

	JButton* btnQuery_;
	JButton* btnCancel_;
};

}

#endif //_QUERYHISTORYBILLFORM_H_
