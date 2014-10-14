#ifndef _TRANSDETAIL_H_
#define _TRANSDETAIL_H_

#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

namespace DAL
{

struct TransDetail
{
	//string trans_id;
	string account_id;
	string other_account_id;
	string abstract_name;
	double money;
	double balance;
	string trans_date;
	unsigned int total;
};

}


#endif //_TRANSDETAIL_H_