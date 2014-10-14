#ifndef _BANKSERVICE_H_
#define _BANKSERVICE_H_

#include "IBankService.h"

namespace DAL
{

class BankService : public IBankService
{
public:
	int UserLogin(const string& user, const string& pass);
	//����
	int OpenAccount(Account& account);
	//����
	int CloseAccount(Account& account, double& interst);
	//�޸�����
	int ChangePassword(Account& account, const string& newPass);
	//���
	int Deposit(Account& account);
	//ȡ��
	int Withdrawal(Account& account);
	//ת��
	int Transfer(Account& account, const string& otherAccountId);
	//��ѯ���
	int BalanceInquiry(Account& account);
	//��ѯ�ձ���
	int QueryDayBill(list<TransDetail>& result, int page, const string& date);
	//�������ڲ�ѯ��ϸ
	int QueryHistoryBill(list<TransDetail>& result, int page, const string& begin, const string& end);
	//�����˺Ų�ѯ��ϸ
	int QueryAccountHistoryBill(list<TransDetail>& result, int page, const string& accountId, const string& begin, const string& end);
};

}


#endif //_BANKSERVICE_H_