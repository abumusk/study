#ifndef _BALANCEINQUIRY_H_
#define _BALANCEINQUIRY_H_

#include "Transaction.h"

namespace CMD
{

//��ѯ���

class BalanceInquiry : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_BALANCEINQUIRY_H_