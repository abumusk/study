#ifndef _MAINMENUFORM_H_
#define _MAINMENUFORM_H_

#include "../JFC/JForm.h"
#include "../JFC/JLinkText.h"
#include "../JFC/JEvent.h"
#include "BankFrom.h"

using namespace JFC;

namespace UI
{

class MainMenuForm : public BankFrom
{
public:
	MainMenuForm();
	MainMenuForm(SHORT x, SHORT y, SHORT width, SHORT height);

	~MainMenuForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);

	void OpenAccount();
	void Deposit();
	void Withdraw();
	void Transfer();
	void BalanceInquiry();
	void ChangePassword();
	void DetailStatement();
	void CloseAccount();
	void Quit();

	std::vector<JLinkText*>& GetItems() { return items_; }

private:
	//void DrawBorder();

	std::vector<JLinkText*> items_;
	
};

}

#endif //_MAINMENUFORM_H_