#ifndef _CLOSEACCOUNT_H_
#define _CLOSEACCOUNT_H_

#include "Transaction.h"

namespace CMD
{

class CloseAccount : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_CLOSEACCOUNT_H_