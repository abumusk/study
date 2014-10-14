#include "StringUtil.h"

using namespace PUBLIC;

std::string StringUtil::Trim(std::string& str)
{
	const std::string drop = " \t";
	str.erase(str.find_last_not_of(drop)+1);		//检测出str中最后一个不在drop中的字符，返回最后一个不在的位置
	return str.erase(0, str.find_first_not_of(drop));//检测出str中第一个不在drop中的字符，返回第一个不在的位置
}

std::string StringUtil::Ltrim(std::string& str)
{
	const std::string drop = " \t";
	return str.erase(0, str.find_first_not_of(drop));
}

std::string StringUtil::Rtrim(std::string& str)
{
	const std::string drop = " \t";
	return str.erase(str.find_last_not_of(drop)+1);
}

std::vector<std::string> StringUtil::Split(const std::string& str, const char delim)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t pos = str.find(delim);
	while (pos != std::string::npos)
	{
		if (pos > start)
		{
			result.push_back(str.substr(start, pos-start));
		}
		start = pos+1;
		pos = str.find(delim, start);//从start开始查找字符delim在当前字符串的位置
	}

	if (start < str.length())
	{
		result.push_back(str.substr(start));
	}

	return result;
}