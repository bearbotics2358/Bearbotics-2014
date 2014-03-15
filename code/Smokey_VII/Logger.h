#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <string>

class Logger
{
public:
	Logger(const std::string& fileName);
	
	void SetEnabled(bool enabled);
	int Log(const std::string& in, bool timeStamp=true);
private:
	bool enabled_;
	bool error_;
	std::string fileName_;
	ofstream logFile_;
};

#endif
