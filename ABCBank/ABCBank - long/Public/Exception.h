#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

namespace PUBLIC
{


class Exception : public std::exception		//“Ï≥£¿‡
{
public:
	explicit Exception(const char* message)
		: message_(message)
	{
		FillStackTrace();
	}

	explicit Exception(const std::string message)
		: message_(message)
	{
		FillStackTrace();
	}


	virtual ~Exception() throw ()
	{

	}

	virtual const char* what() const throw();
	const char* StackTrace() const throw();

private:
	void FillStackTrace();

	std::string message_;
	std::string stackTrace_;
};

}

#endif //_EXCEPTION_H_