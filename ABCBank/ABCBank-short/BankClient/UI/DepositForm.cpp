#include "DepositForm.h"

#include "FormManager.h"
#include "MainMenuForm.h"
#include "ReceiptForm.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Exception.h"

using namespace UI;

DepositForm::DepositForm()
	:BankFrom(0, 0, 0, 0, 0)
{
}

DepositForm::DepositForm(SHORT x, SHORT y, SHORT width, SHORT height,
									   const std::string& title)
	: BankFrom(x, y, width, height),
	  title_(title)
{
	lblAccountId_ = new JLabel(14, 5, 11, 1, "ACCOUNT ID:", this);
	editAccountId_ = new JEdit(26, 5, 21, 1, "", this);
	lblAccountIdTip_ = new JLabel(50, 5, 11, 1, "����6λ������", this);

	lblMoney_ = new JLabel(19, 8, 12, 1, "MONEY:", this);
	editMoney_ = new JEdit(26, 8, 21, 1, "", this);
	lblMoneyTip_ = new JLabel(50, 8, 15, 1, "С���������λ", this);


	btnSubmit_ = new JButton(5, 20, 10, 3, "SUBMIT", this);
	btnReset_ = new JButton(50, 20, 10, 3, "RESET", this);
	btnCancel_ = new JButton(65, 20, 10, 3, "CANCEL", this);

	editAccountId_->SetValidator(ValidateAccountId);
	editMoney_->SetValidator(ValidatePass);
}

DepositForm::~DepositForm()
{
}

void DepositForm::Draw()
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

void DepositForm::OnKeyEvent(JEvent* e)
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

void DepositForm::Reset()
{
	editAccountId_->SetText("");
	editMoney_->SetText("");
}

void DepositForm::Submit()
{
	if (editAccountId_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "�ʺ�С��6λ";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editAccountId_->Show();
		return;
	}
	if (editMoney_->GetText().empty())
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "����Ϊ��";

		int result = JMessageBox::Show("-ERROR-", msg, v);
		ClearWindow();
		Show();
		editMoney_->Show();
		return;
	}

	//TODO ����Ϊʵ�ʵĴ�����
	try
	{
		BankSession bs;
		bs.SetCmd(CMD_DEPOSIT);
		bs.SetAttribute("account_id", editAccountId_->GetText());
		bs.SetAttribute("money", editMoney_->GetText());

		Singleton<TransactionManager>::Instance().DoAction(bs);
		if (bs.GetErrorCode() == 0)
		{
			Reset();

			ReceiptForm* form;
			form = dynamic_cast<ReceiptForm*>(Singleton<FormManager>::Instance().Get("ReceiptForm"));
			form->SetReceiptFormType(ReceiptForm::RFT_DEPOSIT);
			form->SetTitle("���ɹ�");
			form->SetItemText("��������", bs.GetResponse("trans_date"));
			form->SetItemText("��    ��", bs.GetResponse("name"));
			form->SetItemText("��    ��", bs.GetAttribute("account_id"));
			form->SetItemText("���׽��", bs.GetAttribute("monery"));
			form->SetItemText("ժ    Ҫ", "���");
			form->SetItemText("��    ��", bs.GetResponse("balance"));
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
		v.push_back(" YES");

		JMessageBox::Show("-ERROR-", e.what(), v);
		ClearWindow();
		Show();

		return;
	}	

}

void DepositForm::EscCancel(JEvent* e)
{
	Reset();
	JForm* form;
	form = Singleton<FormManager>::Instance().Get("MainMenuForm");
	dynamic_cast<MainMenuForm*>(form)->GetItems()[1]->SetCurrent();
	form->ClearWindow();
	form->Show();
	e->Done();
}