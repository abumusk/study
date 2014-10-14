#include "JApplication.h"

#include "JEvent.h"
#include "JWindow.h"

#include <conio.h>

using namespace JFC;

JApplication* JApplication::self_ = 0;

JApplication::JApplication()
{
	if (self_)
		return;
	
	self_ = this;

	SetConsoleOutputCP(936);		//���ÿ���̨֧�������ַ�
	COORD size = {80, 25};
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);	//���ÿ���̨��С (80��25��) δ���ô�Сֱ�����п���̨�Ĵ�С�ǲ�ֹ25�е�

	HideCursor();
	root_ = new JWindow(0, 0, 80, 25, 0);
	root_->Show();

}

JApplication::~JApplication()
{
	delete root_;
}

//��ʾ���
void JApplication::ShowCursor() const
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);	//��ȡ���������
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hout, &cci);		//��ȡ���
	cci.bVisible = true;
	SetConsoleCursorInfo(hout, &cci);		//���ù��
}

//���ع��
void JApplication::HideCursor() const
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hout, &cci);
	cci.bVisible = false;
	SetConsoleCursorInfo(hout, &cci);
}

int JApplication::Exec()
{
	while (1)
	{
		int key = _getch();
		if (key == 224)			//���ⰴ��		�ϡ� �¡� �� �ҡ� F11�� F12�� home�� pg up�� pg down�� end�� delete�� insert
		{
			key = (224 << 8) + _getch();
		}
		else if (key == 0)		//�����	F1~F10	���������ⰴ����Ҫ�ڴε���_getch()�������������¼�
		{
			key = -1 * _getch();
		}

		if (current_)
		{
			JEvent e(key, current_);
			current_->OnKeyEvent(&e);
		}
	}
	return 0;
}