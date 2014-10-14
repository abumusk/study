#ifndef _WITHDRAWALFORM_H_
#define _WITHDRAWALFORM_H_

#include "../JFC/JLabel.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"

#include "BankFrom.h"

#include <string>

using namespace JFC;

namespace UI
{


	class WithdrawalForm : public BankFrom
	{
	public:
		WithdrawalForm();
		WithdrawalForm(SHORT x, SHORT y, SHORT width, SHORT height,
			const std::string& title);

		~WithdrawalForm();

		virtual void Draw();
		virtual void OnKeyEvent(JEvent* e);

	private:
		void Reset();
		void Submit();

		void EscCancel(JEvent* e);

		std::string title_;

		JLabel* lblAccountId_;
		JEdit* editAccountId_;
		JLabel* lblAccountIdTip_;

		JLabel* lblPass_;
		JEdit* editPass_;
		JLabel* lblPassTip_;

		JLabel* lblMoney_;
		JEdit* editMoney_;
		JLabel* lblMoneyTip_;

		JButton* btnSubmit_;
		JButton* btnReset_;
		JButton* btnCancel_;
	};

}

#endif //_WITHDRAWALFORM_H_