#include "CloseAccountForm.h"

#include "FormManager.h"
#include "MainMenuForm.h"
#include "ReceiptForm.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Exception.h"

using namespace UI;

CloseAccountForm::CloseAccountForm()
	:BankFrom(0, 0, 0, 0, 0)
{
}

CloseAccountForm::CloseAccountForm(SHORT x, SHORT y, SHORT width, SHORT height,
									   const std::string& title)
   : BankFrom(x, y, width, height),
     title_(title)
{
	lblAccountId_ = new JLabel(14, 5, 11, 1, "ACCOUNT ID:", this);
	editAccountId_ = new JEdit(26, 5, 21, 1, "", this);
	lblAccountIdTip_ = new JLabel(50, 5, 11, 1, "长度6位，数字", this);

	lblPass_ = new JLabel(8, 8, 17, 1, "ACCOUNT PASSWORD:", this);
	editPass_ = new JEdit(26, 8, 21, 1, "", this, JEdit::EM_PASSWORD);
	lblPassTip_ = new JLabel(50, 8, 11, 1, "长度6-8位", this);


	btnSubmit_ = new JButton(5, 20, 10, 3, "SUBMIT", this);
	btnReset_ = new JButton(50, 20, 10, 3, "RESET", this);
	btnCancel_ = new JButton(65, 20, 10, 3, "CANCEL", this);

	editAccountId_->SetValidator(ValidateAccountId);
	editPass_->SetValidator(ValidatePass);
}

CloseAccountForm::~CloseAccountForm()
{
}

void CloseAccountForm::Draw()
{
	DrawBorder();
	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);
	JRECT rect = { 1, 1, Width()-2, Height()-2};
	FillRect(rect);

	DrawText(5, 2, title_);
	DrawHLine(3, 2, Width()-3, '-');

	JForm::Draw();
}

void CloseAccountForm::OnKeyEvent(JEvent* e)
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
		else if (e->GetSender() == btnReset_)
		{
			Reset();
			editAccountId_->SetCurrent();
			Show();
			e->Done();
		}
		else if (e->GetSender() == btnSubmit_)
		{
			Submit();
			e->Done();
		}
	}

	JForm::OnKeyEvent(e);
}

void CloseAccountForm::Reset()
{
	editAccountId_->SetText("");
	editPass_->SetText("");
}

void CloseAccountForm::Submit()
{
	if (editAccountId_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "帐号小于6位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editAccountId_->Show();
		return;
	}
	if (editPass_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "密码小于6位";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editPass_->Show();
		return;
	}

	//TODO 以下为实际的销户操作
	try
	{
		BankSession bs;
		bs.SetCmd(CMD_CLOSE_ACCOUNT);
		bs.SetAttribute("account_id", editAccountId_->GetText());
		bs.SetAttribute("pass", editPass_->GetText());

		Singleton<TransactionManager>::Instance().DoAction(bs);
		if (bs.GetErrorCode() == 0)
		{
			ReceiptForm* form;
			form = dynamic_cast<ReceiptForm*>(Singleton<FormManager>::Instance().Get("CloseAccountReceiptForm"));
			form->SetReceiptFormType(ReceiptForm::RFT_CLOSE_ACCOUNT);
			form->SetTitle("销户成功");

			double total = Convert::StringToDouble(bs.GetResponse("balance"))
				+ Convert::StringToDouble(bs.GetResponse("interest"));

			form->SetItemText("销户日期", bs.GetResponse("close_date"));
			form->SetItemText("户    名", bs.GetResponse("name"));
			form->SetItemText("帐    号", bs.GetResponse("account_id"));
			form->SetItemText("余    额", bs.GetResponse("balance"));
			form->SetItemText("利    息", bs.GetResponse("interest"));
			form->SetItemText("总    计", Convert::DoubleToString(total));

			Reset();
			form->Show();
		}
		else
		{
			std::vector<std::string> v;
			v.push_back(" YES ");

			JMessageBox::Show("-ERROR-", bs.GetErrorMsg(), v);
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
		ClearWindow();
		Show();

		return;
	}
}

void CloseAccountForm::EscCancel(JEvent* e)
{
	Reset();
	JForm* form;
	form = Singleton<FormManager>::Instance().Get("MainMenuForm");
	dynamic_cast<MainMenuForm*>(form)->GetItems()[7]->SetCurrent();
	form->ClearWindow();
	form->Show();
	e->Done();
}