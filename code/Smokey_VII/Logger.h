#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <string>

class Logger
{
public:
	Logger(const std::string& fileName);
	
	void InitLogging(void);
	void DisableLogging(void);
	void SetEnabled(bool enabled);
	int Log(const std::string& in, bool timeStamp=true);
private:
	bool init_;
	bool enabled_;
	bool error_;
	
	std::string fileName_;
	std::ofstream logFile_;
};

#endif
