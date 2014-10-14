#include "Date.h"
#include <assert.h>

using namespace PUBLIC;

Date::Date(int year, int month, int day)
{
	assert(year >= 1900);
	assert(month >=1 && month <= 12);
	assert(day >=1 && day <= 31);

	struct tm atm;

	atm.tm_sec = 0;
	atm.tm_min = 0;
	atm.tm_hour = 0;
	atm.tm_mday = day;
	atm.tm_mon = month - 1;	//tm_mon is 0 based
	atm.tm_year = year - 1900; //tm_year is 1900 based
	atm.tm_isdst = -1;			//夏令时

	time_ = _mktime64(&atm);	//获取year-month-dya 00:00:00 距离1970-01-01 00:00:00的秒数
}

//_snprintf_s为何在sizeOfBuffer的基础上多添加一个count？count似乎是用来控制理想的宽度的。
//如果得到的字符串超过了count，于是会被截断到count的长度后面再加一个null-terminate。当然，
//更改的优先级还是sizeOfBuffer。如果应该输出的字符串大小达到了sizeOfBuffer，那么就溢出了，
//溢出情况下，函数把这个当成一个错误，会把buffer缓冲区置为一个空的字符串。
//而_snprintf_s的好处就是有了count参数，输出的字符串就算超过了count缓冲区得长度，
//仍然会有输出，输出字符串被截断到count大小，
//在这个大小的字符串后面加null-terminate当然如果count设置的和sizeOfBuffer一样大或者更大，就失去了作用。
//如果输出的字符串超过了sizeOfBuffer，缓冲区仍然被置为空字符串。因此希望缓冲区被尽量利用，
//可以将count参数设置为_TRUNCATE，这样实际效果是count=sizeOfBuffer-1.
std::string Date::toInString() const
{
	char buf[32] = {0};
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4d-%02d-%02d", year(), month(), day());//_TRUNCATE等于sizeof(buf)-1
	return buf;
}

int Date::year() const
{
	struct tm ttm;
	struct tm* ptm;

	ptm = getLocalTm(&ttm);
	return ptm ? (ptm->tm_year) + 1900 : 0;
}

int Date::month() const
{
	struct tm ttm;
	struct tm* ptm;

	ptm = getLocalTm(&ttm);
	return ptm ? ptm->tm_mday : 0;
}

int Date::day() const
{
	struct tm ttm;
	struct tm* ptm;

	ptm = getLocalTm(&ttm);
	return ptm ? ptm->tm_mday : 0;
}

int Date::weekDay() const
{
	struct tm ttm;
	struct tm* ptm;

	ptm = getLocalTm(&ttm);
	return ptm ? (ptm->tm_wday) + 1 : 0;
}

struct tm* Date::getLocalTm(struct tm* ptm) const
{
	if (ptm != NULL)
	{
		struct tm ptmTemp;
		errno_t err = _localtime64_s(&ptmTemp, &time_);

		if (err != 0)
		{
			return NULL;
		}

		*ptm = ptmTemp;
		return ptm;
	}

	return NULL;
}