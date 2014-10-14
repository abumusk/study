#include "DetailStatementForm.h"

#include "FormManager.h"
#include "MainMenuForm.h"
#include "Validator.h"

#include "../JFC/JMessageBox.h"


using namespace UI;

DetailStatementForm::DetailStatementForm()
	:BankFrom(0, 0, 0, 0, 0)
{
}

DetailStatementForm::DetailStatementForm(SHORT x, SHORT y, SHORT width, SHORT height,
									   const std::string& title)
   : BankFrom(x, y, width, height),
     title_(title)
{
	btnDetail1_ = new JButton(18, 6, 40, 3, "View the one day's detail", this);
	btnDetail2_ = new JButton(18, 11, 40, 3, "View the detail in a range", this);
	btnDetail3_ = new JButton(18, 16, 40, 3, "Search the record by account id", this);
	btnReturn_ = new JButton(65, 20, 10, 3, "RETURN", this);

}

DetailStatementForm::~DetailStatementForm()
{
}

void DetailStatementForm::Draw()
{
	DrawBorder();
	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);
	JRECT rect = { 1, 1, Width()-2, Height()-2 };
	FillRect(rect);

	DrawText(5, 2, title_);
	DrawHLine(3, 2, Width()-3, '-');

	JForm::Draw();
}

void DetailStatementForm::OnKeyEvent(JEvent* e)
{
	int key = e->GetEventCode();

	if (key == KEY_ESC)
	{
		EscCancel(e);
	}
	else if (key == KEY_ENTER)
	{
		JForm* form;
		if (e->GetSender() == btnDetail1_)
		{
			form = Singleton<FormManager>::Instance().Get("QueryDayBillForm");
			ClearWindow();
			form->Show();

			e->Done();
		}
		else if (e->GetSender() == btnDetail2_)
		{
			//ClearWindow();
			form = Singleton<FormManager>::Instance().Get("QueryHistoryBillForm");
			ClearWindow();
			form->Show();

			e->Done();
		}
		else if (e->GetSender() == btnDetail3_)
		{
			form = Singleton<FormManager>::Instance().Get("QueryAccountHistoryBillForm");
			ClearWindow();
			form->Show();

			e->Done();
		}
		else if (e->GetSender() == btnReturn_)
		{
			EscCancel(e);
		}
	}

	JForm::OnKeyEvent(e);
}


void DetailStatementForm::EscCancel(JEvent* e)
{
	JForm* form;
	form = Singleton<FormManager>::Instance().Get("MainMenuForm");
	dynamic_cast<MainMenuForm*>(form)->GetItems()[6]->SetCurrent();
	form->ClearWindow();
	form->Show();
	e->Done();
}