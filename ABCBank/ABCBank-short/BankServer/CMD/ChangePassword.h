#ifndef _CHANGEPASSWORD_H_
#define _CHANGEPASSWORD_H_

#include "Transaction.h"

namespace CMD
{

class ChangePassword : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_CHANGEPASSWORD_H_