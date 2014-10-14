#ifndef _BANKFROM_H_
#define _BANKFROM_H_

#include "../JFC/JForm.h"

using namespace JFC;

class BankFrom : public JForm
{
public:
	BankFrom(SHORT x, SHORT y, SHORT width, SHORT height,
		JWindow* parent = jApp->Root())
		: JForm(x, y, width, height, parent)
	{

	}

protected:
	void DrawBorder()
	{
		SetTextColor(FCOLOR_YELLO);
		SetBkColor(BCOLOR_RED);

		DrawHLine(0, 0, Width()-1, '-');			//�����߲�����-������
		DrawHLine(Height()-1, 0, Width()-1, '-');

		DrawVLine(0, 1, Height()-2, ' ');			//�����ߣ��հ��ַ����
		DrawVLine(Width()-1, 1, Height()-2, ' ');
	}

};

#endif //_BANKFROM_H_