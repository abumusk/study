#ifndef _OPENACCOUNT_H_
#define _OPENACCOUNT_H_

#include "Transaction.h"

namespace CMD
{

class OpenAccount : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_OPENACCOUNT_H_