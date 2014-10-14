#include "QueryDayBillForm.h"

#include "FormManager.h"
#include "DetailStatementForm.h"
#include "ReportForm.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Exception.h"

using namespace UI;

QueryDayBillForm::QueryDayBillForm()
:BankFrom(0, 0, 0, 0, 0)
{
}

QueryDayBillForm::QueryDayBillForm(SHORT x, SHORT y, SHORT width, SHORT height)
: BankFrom(x, y, width, height)
{
	editDate_ = new JEdit(29, 9, 20, 1, "", this);

	btnQuery_ = new JButton(28, 13, 11, 3, "QUERY", this);
	btnCancel_ = new JButton(41, 13, 10, 3, "CANCEL", this);

	editDate_->SetValidator(ValidateDate);
}

QueryDayBillForm::~QueryDayBillForm()
{
}

void QueryDayBillForm::Draw()
{
	DrawBorder();

	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);

	DrawText(3, 2, "-DETAIL QUERY-");

	DrawText(10, 4, "DATE:");
	DrawText(39, 4, "YYYY-MM-DD");

	DrawText(10, 6, "Default:2013-07-01");

	JForm::Draw();
}

void QueryDayBillForm::OnKeyEvent(JEvent* e)
{
	int key = e->GetEventCode();

	if (key == KEY_ESC)
	{
		EscCancel(e);
	}
	else if (key == KEY_ENTER)
	{
		if (e->GetSender() == btnCancel_)
		{
			EscCancel(e);
		}
		else if (e->GetSender() == btnQuery_)
		{
			Query();
			e->Done();
		}
	}

	JForm::OnKeyEvent(e);
}

void QueryDayBillForm::Reset()
{
	editDate_->SetText("");
}

void QueryDayBillForm::Query()
{
	if (editDate_->GetText().length() < 10)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "日期小于10位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editDate_->Show();
		return;
	}

	//TODO 以下为实际的查询操作(某日交易记录)
	try
	{
		BankSession* bs = Singleton<Client>::Instance().GetBankSession();
		bs->Clear();
		bs->SetCmd(CMD_DAY_BILL);
		bs->SetAttribute("date", editDate_->GetText());
		bs->SetAttribute("page", "0");
		
		Singleton<TransactionManager>::Instance().DoAction(*bs);
		if (bs->GetErrorCode() == 0)
		{
			ReportForm* form;
			form = dynamic_cast<ReportForm*>(Singleton<FormManager>::Instance().Get("ReportForm"));
			form->SetCmd(CMD_DAY_BILL);
			list<TransDetail>& tds = bs->GetDetails();
			list<TransDetail>::iterator it = tds.begin();
			form->SetLines(it->total);
			form->SetDetails(bs->GetDetails());
			form->SetDate(editDate_->GetText());

			Reset();
			form->ClearWindow();
			form->Show();
		}
		else
		{
			std::vector<std::string> v;
			v.push_back(" YES ");

			JMessageBox::Show("-ERROR-", bs->GetErrorMsg(), v);
			Reset();
			ClearWindow();
			Show();

			return;
		}
	}
	catch (Exception& e)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");

		JMessageBox::Show("-ERROR-", e.what(), v);
		Reset();
		ClearWindow();
		Show();

		return;
	}

}

void QueryDayBillForm::EscCancel(JEvent* e)
{
	Reset();
	JForm* form;
	form = Singleton<FormManager>::Instance().Get("DetailStatementForm");
	dynamic_cast<DetailStatementForm*>(form)->btnDetail1_->SetCurrent();
	form->ClearWindow();
	form->Show();
	e->Done();
}