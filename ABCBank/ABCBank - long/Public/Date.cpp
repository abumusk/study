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
	atm.tm_isdst = -1;			//����ʱ

	time_ = _mktime64(&atm);	//��ȡyear-month-dya 00:00:00 ����1970-01-01 00:00:00������
}

//_snprintf_sΪ����sizeOfBuffer�Ļ����϶����һ��count��count�ƺ���������������Ŀ�ȵġ�
//����õ����ַ���������count�����ǻᱻ�ضϵ�count�ĳ��Ⱥ����ټ�һ��null-terminate����Ȼ��
//���ĵ����ȼ�����sizeOfBuffer�����Ӧ��������ַ�����С�ﵽ��sizeOfBuffer����ô������ˣ�
//�������£��������������һ�����󣬻��buffer��������Ϊһ���յ��ַ�����
//��_snprintf_s�ĺô���������count������������ַ������㳬����count�������ó��ȣ�
//��Ȼ�������������ַ������ضϵ�count��С��
//�������С���ַ��������null-terminate��Ȼ���count���õĺ�sizeOfBufferһ������߸��󣬾�ʧȥ�����á�
//���������ַ���������sizeOfBuffer����������Ȼ����Ϊ���ַ��������ϣ�����������������ã�
//���Խ�count��������Ϊ_TRUNCATE������ʵ��Ч����count=sizeOfBuffer-1.
std::string Date::toInString() const
{
	char buf[32] = {0};
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%4d-%02d-%02d", year(), month(), day());//_TRUNCATE����sizeof(buf)-1
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