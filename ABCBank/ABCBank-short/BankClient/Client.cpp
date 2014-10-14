#include "Client.h"

#include "../Public/Socket.h"

#include <sstream>
using namespace std;

Client::Client(void) : config_("client.conf")
{
	Socket::Startup();
	serverIp_ = config_.GetProperty("CLIENT.SERVER_IP");
	string potr = config_.GetProperty("CLIENT.PORT");
	stringstream ss;
	ss<<potr;
	ss>>port_;
}

Client::~Client(void)
{
	Socket::Cleanup();
}
