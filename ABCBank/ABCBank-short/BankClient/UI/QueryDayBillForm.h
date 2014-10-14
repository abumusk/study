#ifndef _QUERYDAYBILLFORM_H_
#define _QUERYDAYBILLFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{

class QueryDayBillForm : public BankFrom
{
public:
	QueryDayBillForm();
	QueryDayBillForm(SHORT x, SHORT y, SHORT width, SHORT height);

	~QueryDayBillForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

private:
	void Reset();
	void Query();

	void EscCancel(JEvent* e);

	JEdit* editDate_;

	JButton* btnQuery_;
	JButton* btnCancel_;
};

}


#endif //_QUERYDAYBILLFORM_H_