#include "LoginForm.h"

#include "FormManager.h"
#include "Validator.h"

#include "../BankSession.h"
#include "../TransactionManager.h"

#include "../JFC/JMessageBox.h"

#include "../../Public/Socket.h"
#include "../../Public/Exception.h"

#include "../Client.h"

using namespace UI;

LoginForm::LoginForm(SHORT x, SHORT y, SHORT width, SHORT height,
					JWindow* parent)
	: BankFrom(x, y, width, height, parent)
{
	lblUser_ = new JLabel(16, 8, 9, 1, "USERNAME:", this);
	editUser_ = new JEdit(26, 8, 21, 1, "", this);
	lblUserTip_ = new JLabel(50, 8, 22, 1, "长度3-10位，字母或数字", this);

	lblPass_ = new JLabel(16, 12, 9, 1, "PASSWORD:", this);
	editPass_ = new JEdit(26, 12, 21, 1, "", this, JEdit::EM_PASSWORD);
	lblPassTip_ = new JLabel(50, 12, 9, 1, "长度6-8位", this);
	
	btnLogin_ = new JButton(22, 17, 11, 3, "LOGIN", this);
	btnExit_ = new JButton(48, 17, 10, 3, "EXIT", this);

	editUser_->SetValidator(ValidateName);
	editPass_->SetValidator(ValidatePass);
}

LoginForm::~LoginForm()
{
	delete lblUser_;
	delete editUser_;
	delete lblUserTip_;

	delete lblPass_;
	delete editPass_;
	delete lblPassTip_;

	delete btnLogin_;
	delete btnExit_;
}

void LoginForm::Draw()
{
	DrawBorder();				//绘制边框

	SetTextColor(FCOLOR_BLUE);	//设置前景色
	SetBkColor(BCOLOR_WHITE);	//设置背景色

	DrawText(28, 3, "Login To The Bank System");	//绘制文本
	DrawHLine(4, 25, 54, '-');						//绘制-

	DrawText(2, 23, "Bank System, Author: www.cppcurse.com");

	JForm::Draw();		//显示调用基类的Draw函数

}

//void LoginForm::DrawBorder()
//{
//	SetTextColor(FCOLOR_YELLO);
//	SetBkColor(BCOLOR_RED);
//
//	DrawHLine(0, 0, Width()-1, '-');			//画横线并且用-做文体
//	DrawHLine(Height()-1, 0, Width()-1, '-');
//
//	DrawVLine(0, 1, Height()-2, ' ');			//画竖线，空白字符填充
//	DrawVLine(Width()-1, 1, Height()-2, ' ');
//}

void LoginForm::OnKeyEvent(JEvent* e)
{
	JWindow* win = e->GetSender();
	int key = e->GetEventCode();
	if (win == btnLogin_ && key == KEY_ENTER)
	{
		//属于LoginForm的特别事件，事件处理
		e->Done();	//说明事件处理完毕，不再进一步分发
		Login();
	}
	if (win == btnExit_ && key == KEY_ENTER)
	{
		//属于LoginForm的特别事件，事件处理
		e->Done();	//说明事件处理完毕，不再进一步分发
		Exit();
	}
	JForm::OnKeyEvent(e);
}

void LoginForm::Login()
{
	if (editUser_->GetText().length() < 3)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "用户名小于3位";

		int result = JMessageBox::Show("-ERROR-", msg, v);

		JWindow* win = jApp->GetCurrent();		//这里的win是之前按纽，就是引发消息窗的按纽
		editUser_->Show();
		win->Draw();
		win->Refresh();
		return;
	}
	if (editPass_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "密码小于6位";

		int result = JMessageBox::Show("-ERROR-", msg, v);

		JWindow* win = jApp->GetCurrent();
		editPass_->Show();
		win->Draw();
		win->Refresh();
		return;
	}
	//TODO：以下为实际的登陆操作
	/*

	Socket sock;
	sock.Create();
	sock.Connect("127.0.0.1", 8888);
	std::string str;
	str.append(editUser_->GetText());
	str.append("#");
	str.append(editPass_->GetText());
	sock.Send(str.c_str(), str.length());

	char buffer[1024] = {0};
	sock.Recv(buffer, sizeof(buffer));

	std::vector<std::string> v;
	v.push_back(" YES ");

	JMessageBox::Show("-ERROR-", buffer, v);


	Rest();
	JForm* form = Singleton<FormManager>::Instance().Get("MainMenuForm");
	form->ClearWindow();
	form->Show();
	*/

	try
	{
		BankSession* bs = Singleton<Client>::Instance().GetBankSession();
		bs->Clear();
		bs->SetCmd(CMD_LOGIN);
		bs->SetAttribute("name", editUser_->GetText());
		bs->SetAttribute("pass", editPass_->GetText());

		Singleton<TransactionManager>::Instance().DoAction(*bs);

		if (bs->GetErrorCode() == 0)
		{	
			std::vector<std::string> v;
			v.push_back(" YES ");

			JMessageBox::Show("-MESSAGE-", "登录成功", v);

			Rest();
			JForm* form = Singleton<FormManager>::Instance().Get("MainMenuForm");
			form->ClearWindow();
			form->Show();

		}
		else
		{
			std::vector<std::string> v;
			v.push_back(" YES ");
			JMessageBox::Show("-ERROR-", bs->GetErrorMsg(), v);

			ClearWindow();
			Show();
		}
	}
	catch (Exception& e)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		JMessageBox::Show("-ERROR-", e.what(), v);
		
		ClearWindow();
		Show();
	}


}

void LoginForm::Exit()
{
	std::vector<std::string> v;
	v.push_back(" YES ");
	v.push_back(" NO ");
	v.push_back(" 1 ");
	v.push_back(" 2 ");
	v.push_back(" 3 ");
	v.push_back(" 4 ");
	v.push_back(" 5 ");

	std::string msg = "Do you want to exit????????????????????????????????????????????";

	int result = JMessageBox::Show("-MESSAGE-", msg, v);

	if (result == 0)
	{
		system("cls");		//在控制台执行cls命令 就是清屏
		exit(0);
	}

	ClearWindow();
	Show();
}

void LoginForm::Rest()
{
	editUser_->SetText("");
	editPass_->SetText("");
}