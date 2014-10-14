#ifndef _DETAILSTATEMENTFORM_H_
#define _DETAILSTATEMENTFORM_H_

//class DetailStatementForm
//{
//public:
//	DetailStatementForm(void);
//	~DetailStatementForm(void);
//};

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


	class DetailStatementForm : public BankFrom
	{
	public:
		DetailStatementForm();
		DetailStatementForm(SHORT x, SHORT y, SHORT width, SHORT height,
			const std::string& title);

		~DetailStatementForm();

		virtual void Draw();
		virtual void OnKeyEvent(JEvent* e);

	public:
		void EscCancel(JEvent* e);

		std::string title_;

		JButton* btnDetail1_;
		JButton* btnDetail2_;
		JButton* btnDetail3_;
		JButton* btnReturn_;

	};

}

#endif //_DETAILSTATEMENTFORM_H_