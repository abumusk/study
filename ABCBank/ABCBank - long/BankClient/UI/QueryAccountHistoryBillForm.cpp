#include "QueryAccountHistoryBillForm.h"

#include "FormManager.h"
#include "DetailStatementForm.h"
#include "ReportForm.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Exception.h"

using namespace UI;

QueryAccountHistoryBillForm::QueryAccountHistoryBillForm()
	:BankFrom(0, 0, 0, 0, 0)
{
}

QueryAccountHistoryBillForm::QueryAccountHistoryBillForm(SHORT x, SHORT y, SHORT width, SHORT height)
	: BankFrom(x, y, width, height)
{
	editAccountId_ = new JEdit(29, 9, 20, 1, "", this);
	editBeginDate_ = new JEdit(29, 11, 20, 1, "", this);
	editEndDate_ = new JEdit(29, 13, 20, 1, "", this);

	btnQuery_ = new JButton(28, 15, 11, 3, "QUERY", this);
	btnCancel_ = new JButton(41, 15, 10, 3, "CANCEL", this);

	editAccountId_->SetValidator(ValidateAccountId);
	editBeginDate_->SetValidator(ValidateDate);
	editEndDate_->SetValidator(ValidateDate);
}

QueryAccountHistoryBillForm::~QueryAccountHistoryBillForm()
{
}

void QueryAccountHistoryBillForm::Draw()
{
	DrawBorder();

	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);

	DrawText(3, 2, "-DETAIL QUERY-");

	DrawText(4, 4, "ACCOUNT ID:");
	DrawText(39, 4, "LENGTH OF 6");

	DrawText(4, 6, "BEGIN DATE:");
	DrawText(39, 6, "YYYY-MM-DD");

	DrawText(6, 8, "END DATE:");
	DrawText(39, 8, "YYYY-MM-DD");

	JForm::Draw();
}

void QueryAccountHistoryBillForm::OnKeyEvent(JEvent* e)
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

void QueryAccountHistoryBillForm::Reset()
{
	editAccountId_->SetText("");
	editBeginDate_->SetText("");
	editEndDate_->SetText("");
}

void QueryAccountHistoryBillForm::Query()
{
	if (editAccountId_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "帐号长度小于6位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editAccountId_->Show();
		return;
	}
	if (editBeginDate_->GetText().length() < 10)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "开始日期小于10位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editBeginDate_->Show();
		return;
	}
	if (editEndDate_->GetText().length() < 10)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "结束日期小于10位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editEndDate_->Show();
		return;
	}

	//TODO 以下为实际的查询操作
	try
	{
		BankSession* bs = Singleton<Client>::Instance().GetBankSession();
		bs->Clear();
		bs->SetCmd(CMD_ACCOUNT_HISTORY_BILL);
		bs->SetAttribute("account_id", editAccountId_->GetText());
		bs->SetAttribute("begin_date", editBeginDate_->GetText());
		bs->SetAttribute("end_date", editEndDate_->GetText());
		bs->SetAttribute("page", "0");

		Singleton<TransactionManager>::Instance().DoAction(*bs);
		if (bs->GetErrorCode() == 0)
		{
			ReportForm* form;
			form = dynamic_cast<ReportForm*>(Singleton<FormManager>::Instance().Get("ReportForm"));
			form->SetCmd(CMD_ACCOUNT_HISTORY_BILL);
			list<TransDetail>& tds = bs->GetDetails();
			list<TransDetail>::iterator it = tds.begin();
			form->SetLines(it->total);
			form->SetDetails(bs->GetDetails());
			form->SetAccountId(editAccountId_->GetText());
			form->SetBeginDate(editBeginDate_->GetText());
			form->SetEndDate(editEndDate_->GetText());

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

void QueryAccountHistoryBillForm::EscCancel(JEvent* e)
{
	Reset();
	JForm* form;
	form = Singleton<FormManager>::Instance().Get("DetailStatementForm");
	dynamic_cast<DetailStatementForm*>(form)->btnDetail3_->SetCurrent();
	form->ClearWindow();
	form->Show();
	e->Done();
}