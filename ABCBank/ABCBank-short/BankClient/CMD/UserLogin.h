#ifndef _USERLOGIN_H_
#define _USERLOGIN_H_

#include "Transaction.h"

namespace CMD
{

class UserLogin : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_USERLOGIN_H_