#ifndef _SERVER_H_
#define _SERVER_H_

#include "../Public/SystemConfig.h"
#include "../Public/Singleton.h"

using namespace PUBLIC;

class Server
{
	friend class Singleton<Server>;
public:
	int Start();

	const string& GetServerIp() const
	{
		return serverIp_;
	}

	unsigned int GetPort() const
	{
		return potr_;
	}

	const string GetDbServerIp() const
	{
		return dbServerIp_;
	}

	unsigned int GetDbServerPort() const
	{
		return dbServerPort_;
	}

	const string& GetDbUser() const
	{
		return dbUser_;
	}

	const string& GetDbPass() const
	{
		return dbPass_;
	}

	const string& GetDbName() const
	{
		return dbName_;
	}

	double GetInteretRate() const
	{
		return interetRate_;
	}

private:
	Server();
	Server(const Server& rhs);
	~Server();

	SystemConfig config_;

	string serverIp_;
	unsigned short potr_;
	string dbServerIp_;
	unsigned short dbServerPort_;
	string dbUser_;
	string dbPass_;
	string dbName_;

	double interetRate_;
	
};

#endif //_SERVER_H_