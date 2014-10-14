#ifndef _JMESSAGEBOX_H_
#define _JMESSAGEBOX_H_

#include "JWindow.h"

#include <string>
#include <vector>

namespace JFC
{

class JMessageBox
{
public:
	static int Show(const std::string& title,
		const std::string& text,
		std::vector<std::string>& buttons);
};

class JMessageBoxImpl : public JWindow
{
public:
	JMessageBoxImpl(SHORT x, SHORT y, SHORT width, SHORT height,
		const std::string& title, const std::string& text,
		std::vector<std::string>& buttons, SHORT btnDistance,
		int lines, JWindow* parent = jApp->Root());

	~JMessageBoxImpl() {}

	virtual void Draw();

	int Exec();

private:
	std::string title_;
	std::string text_;

	short btnDistance_;	//按纽宽度
	int lines_;			//文本显示行数

};

}

#endif //_JMESSAGEBOX_H_