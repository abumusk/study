#include "ReportForm.h"

#include "FormManager.h"
#include "DetailStatementForm.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Exception.h"

#include <vector>
#include <string>
#include <sstream>

using namespace UI;

ReportForm::ReportForm()
	: BankFrom(0, 0, 0, 0),
	  curpage_(0), cur_(0), start_(0), lines_(0), pages_(0)
{
}

ReportForm::ReportForm(SHORT x, SHORT y, SHORT w, SHORT h)
	: BankFrom(x, y, w, h),
	  curpage_(0), cur_(0), start_(0), lines_(0), pages_(0)
{
}

ReportForm::~ReportForm()
{
}

void ReportForm::Draw()
{
	DrawBorder();
	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);
	DrawText(2, 2, "SALE REPORT");

	stringstream ss;
	ss<<(curpage_+1) <<" / " <<pages_;

	DrawText(80 - 2 - ss.str().length(), 2, ss.str());

	DrawHLine(3, 1, 78, '-');
	DrawText(2, 4, "���            ��������     �ʺ� �Է��ʺ�  ժҪ      ���׽��          ���");
	DrawHLine(5, 1, 78, '-');
	DrawHLine(21, 1, 78, '-');
	DrawText(2, 22, "[ESC] [PGUP] [PGDN] [HOME] [END] [UP] [DOWN]");

	std::list<TransDetail>::const_iterator it;
	int i = start_;
	int j = 0;
	for (it = details_.begin(); it != details_.end(); ++it)
	{
		if (i == cur_)
		{
			SetTextColor(FCOLOR_BLUE | COMMON_LVB_UNDERSCORE);	//COMMON_LVB_UNDERSCORE��ǰ�ı����»���
			SetBkColor(BCOLOR_GREEN);
		}
		else
		{
			SetTextColor(FCOLOR_MAGENTA | COMMON_LVB_UNDERSCORE);
			SetBkColor(BCOLOR_GRAY);
		}

		std::stringstream ss;
		ss<<setw(4) <<setfill('0') <<i+1 <<" " <<it->trans_date <<"   "  <<setw(6) <<setfill('0') <<it->account_id <<
			"   ";
		if (!it->other_account_id.empty())
			ss<<setw(6)<<setfill('0')<<it->other_account_id;
		else
			ss<<setw(6)<<setfill(' ')<<"null";
		ss<<"  " <<it->abstract_name<<" " <<
			setw(13) <<setfill(' ') <<setprecision(2)<<setiosflags(std::ios::fixed)<<it->money<<" " <<
			setw(13) <<setfill(' ') <<setprecision(2)<<setiosflags(std::ios::fixed)<<it->balance;

		DrawText(2, 6+j, ss.str());
		i++;
		j++;
	}
	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);

	//SetTextColor(FCOLOR_MAGENTA | COMMON_LVB_UNDERSCORE);

	//SetBkColor(BCOLOR_GRAY);
    //                3                  23       32       41    47            61            75              
	//               "���            ��������     �ʺ� �Է��ʺ�  ժҪ      ���׽��          ���"
	//DrawText(2, 6, "0001 2009-01-01 12:12:12   000001   000002  ��� 1000000000.00 9999999999.00");
	//DrawText(2, 7, "0001 2009-01-01 12:12:12   000001   000002  ��� 1000000000.00 9999999999.00");
	//DrawText(2, 8, "0001 2009-01-01 12:12:12   000001   000002  ���         99.00      12345.00");
	//DrawText(2, 9, "0001 2009-01-01 12:12:12   000001   000002  ���         99.00      12345.00");
	//SetTextColor(FCOLOR_BLUE | COMMON_LVB_UNDERSCORE);
	//SetBkColor(BCOLOR_GREEN);
	//DrawText(2, 10, "0001 2009-01-01 12:12:12   000001   000002  ���         99.00      12345.00");

	//SetTextColor(FCOLOR_MAGENTA | COMMON_LVB_UNDERSCORE);

	JForm::Draw();
}

void ReportForm::OnKeyEvent(JEvent* e)
{
	int key = e->GetEventCode();
	e->Done();
	
	switch (key)
	{
	case KEY_ESC:
		JForm* form;
		Reset();
		ClearWindow();
		form = Singleton<FormManager>::Instance().Get("DetailStatementForm");
		form->Show();
		break;
	case KEY_PGDN:
		PageDown();
		break;
	case KEY_PGUP:
		PageUp();
		break;
	case KEY_DOWN:
		Down();
		break;
	case KEY_UP:
		Up();
		break;
	case KEY_HOME:
		Home();
		break;
	case KEY_END:
		End();
		break;
	}

	JForm::OnKeyEvent(e);
}

void ReportForm::Reset()
{
	start_ = 0;
	cur_ = 0;
	lines_ = 0;
	curpage_ = 0;
	pages_ = 0;
	details_.clear();
}

void ReportForm::PageUp()
{
	if (curpage_ == 0)
		return;
	
	--curpage_;

	start_ -= 15;	//ÿҳÿһ��
	cur_ = start_;	//��ǰ������
	BankSession* bs = Singleton<Client>::Instance().GetBankSession();
	bs->Clear();
	bs->SetCmd(cmd_);
	bs->SetAttribute("date", date_);
	bs->SetAttribute("begin_date", beginDate_);
	bs->SetAttribute("end_date", endDate_);
	bs->SetAttribute("account_id", accountId_);

	string page;
	stringstream ss;
	ss<<(start_ / 15);
	ss>>page;
	bs->SetAttribute("page", page);
	Singleton<TransactionManager>::Instance().DoAction(*bs);

	SetDetails(bs->GetDetails());

	ClearWindow();
	Show();
}

void ReportForm::PageDown()
{
	if (curpage_ == pages_ - 1)
		return;

	++curpage_;

	start_ += 15;
	cur_ = start_;
	BankSession* bs = Singleton<Client>::Instance().GetBankSession();
	bs->Clear();
	bs->SetCmd(cmd_);
	bs->SetAttribute("date", date_);
	bs->SetAttribute("begin_date", beginDate_);
	bs->SetAttribute("end_date", endDate_);
	bs->SetAttribute("account_id", accountId_);

	string page;
	stringstream ss;
	ss<<(start_ / 15);
	ss>>page;
	bs->SetAttribute("page", page);
	Singleton<TransactionManager>::Instance().DoAction(*bs);

	SetDetails(bs->GetDetails());

	ClearWindow();
	Show();
}

void ReportForm::Up()
{
	if (cur_ > 0)
	{
		--cur_;
	}
	if (cur_ < start_)
	{
		--curpage_;
		start_ -= 15;
		BankSession* bs = Singleton<Client>::Instance().GetBankSession();
		bs->Clear();
		bs->SetCmd(cmd_);
		bs->SetAttribute("date", date_);
		bs->SetAttribute("begin_date", beginDate_);
		bs->SetAttribute("end_date", endDate_);
		bs->SetAttribute("account_id", accountId_);
		string page;
		stringstream ss;
		ss<<(start_ /15);
		ss>>page;
		bs->SetAttribute("page", page);
		Singleton<TransactionManager>::Instance().DoAction(*bs);

		SetDetails(bs->GetDetails());
	
		ClearWindow();
		Show();
	}
	else
	{
		ClearWindow();
		Show();
	}
}

void ReportForm::Down()
{
	if (cur_ < lines_ - 1)
	{
		++cur_;
	}
	if (cur_ - start_ >= 15)
	{
		++curpage_;
		start_ += 15;
		cur_ = start_;
		BankSession* bs = Singleton<Client>::Instance().GetBankSession();
		bs->Clear();
		bs->SetCmd(cmd_);
		bs->SetAttribute("date", date_);
		bs->SetAttribute("begin_date", beginDate_);
		bs->SetAttribute("end_date", endDate_);
		bs->SetAttribute("account_id", accountId_);
		string page;
		stringstream ss;
		ss<<(start_ / 15);
		ss>>page;
		bs->SetAttribute("page", page);
		Singleton<TransactionManager>::Instance().DoAction(*bs);

		SetDetails(bs->GetDetails());

		ClearWindow();
		Show();
	}
	else
	{
		ClearWindow();
		Show();
	}
}

void ReportForm::Home()
{
	if (curpage_ == 0)
		return;

	start_ = 0;
	cur_ = 0;
	curpage_ = 0;

	BankSession* bs = Singleton<Client>::Instance().GetBankSession();
	bs->Clear();
	bs->SetCmd(cmd_);
	bs->SetAttribute("date", date_);
	bs->SetAttribute("begin_date", beginDate_);
	bs->SetAttribute("end_date", endDate_);
	bs->SetAttribute("account_id", accountId_);
	string page;
	stringstream ss;
	ss<<(start_ / 15);
	ss>>page;
	bs->SetAttribute("page", page);
	Singleton<TransactionManager>::Instance().DoAction(*bs);

	SetDetails(bs->GetDetails());

	ClearWindow();
	Show();
}

void ReportForm::End()
{
	if (curpage_ == pages_ - 1)
		return;

	curpage_ = pages_ - 1;
	start_ = 15 * curpage_;
	cur_ = start_;

	BankSession* bs = Singleton<Client>::Instance().GetBankSession();
	bs->Clear();
	bs->SetCmd(cmd_);
	bs->SetAttribute("date", date_);
	bs->SetAttribute("begin_date", beginDate_);
	bs->SetAttribute("end_date", endDate_);
	bs->SetAttribute("account_id", accountId_);

	string page;
	stringstream ss;
	ss<<(start_ / 15);
	ss>>page;
	bs->SetAttribute("page", page);
	Singleton<TransactionManager>::Instance().DoAction(*bs);

	SetDetails(bs->GetDetails());
	
	ClearWindow();
	Show();
}