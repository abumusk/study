#ifndef _QUERYDAYBILL_H_
#define _QUERYDAYBILL_H_

#include "Transaction.h"

namespace CMD
{

class QueryDayBill : public Transaction
{
public:
	virtual void Execute(BankSession& session);
};

}

#endif //_QUERYDAYBILL_H_