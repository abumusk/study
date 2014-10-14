#ifndef _REPORTFORM_H_
#define _REPORTFORM_H_

#include "BankFrom.h"
#include "../JFC/JEdit.h"
#include "../JFC/JButton.h"
#include "../JFC/JLabel.h"
#include "../JFC/JEvent.h"

#include <list>
#include "../TransDetail.h"

using namespace JFC;

namespace UI
{

class ReportForm : public BankFrom
{
public:
	ReportForm();
	ReportForm(SHORT x, SHORT y, SHORT w, SHORT h);
	~ReportForm();

	virtual void Draw();
	virtual void OnKeyEvent(JEvent* e);
	void SetDetails(std::list<TransDetail>& details)
	{
		details_.swap(details);
	}

	void SetCmd(int cmd)
	{
		cmd_ = cmd;
	}

	void SetDate(const std::string& date)
	{
		date_ = date;
	}

	void SetBeginDate(const std::string& date)
	{
		beginDate_ = date;
	}

	void SetEndDate(const std::string& date)
	{
		endDate_ = date;
	}

	void SetAccountId(const std::string& accountId)
	{
		accountId_ = accountId;
	}

	void SetLines(int lines)
	{
		lines_ = lines;
		pages_ = (lines_ + 14) / 15;
	}

private:
	void Reset();

	void PageUp();
	void PageDown();
	void Up();
	void Down();
	void Home();
	void End();

	int cmd_;
	std::list<TransDetail> details_;

	std::string date_;
	std::string beginDate_;
	std::string endDate_;
	std::string accountId_;

	int start_;		//第一行序号
	int cur_;		//光标所在行序号

	int curpage_;	//当前页号
	int lines_;		//总行数
	int pages_;		//总页数
};

}

#endif //_REPORTFORM_H_