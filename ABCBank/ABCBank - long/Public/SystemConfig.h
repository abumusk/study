#ifndef _SYSTEMCONFIG_H_
#define _SYSTEMCONFIG_H_

#include <string>
#include <map>
#include <fstream>

using namespace std;

namespace PUBLIC
{
	
class SystemConfig
{
public:
	SystemConfig(const string& filepath);
	void Load();
	const std::string& GetProperty(const string& name);
	
private:
	map<string, string> properties_;
	string filepath_;
};

}

#endif //_SYSTEMCONFIG_H_