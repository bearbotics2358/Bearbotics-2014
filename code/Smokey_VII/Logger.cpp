#include "Logger.h"

#include <iostream>
#include <Timer.h>

using std::string;

Logger::Logger(const string& fileName)
:logFile_()
{
	error_ = false;
	fileName_ = fileName;
	enabled_ = false;
}

void Logger::InitLogging(void) 
{
	if(!init_){
		try {
			logFile_.open(fileName_.c_str(), std::ios::out);
		} catch (std::ofstream::failure& e) {
			printf("Exception Occurred! %s\n", e.what());
			error_ = true;
			return;			
		}
		Log(">>>>>>>>>>>>>>>>START LOG<<<<<<<<<<<<<<<<", false);
	}
}

void Logger::DisableLogging(void)
{
	if(init_) {
		Log(">>>>>>>>>>>>>>>>STOP LOG<<<<<<<<<<<<<<<<", false);
		logFile_.close();
	}
}

void Logger::SetEnabled(bool enabled)
{
	enabled_ = enabled;
}

int Logger::Log(const string& in, bool timeStamp)
{
	if(enabled_ && !error_) {
		string out = in + "\n";
		printf(out.c_str());
		if (timeStamp) logFile_ << "[" << Timer::GetFPGATimestamp() << "] ";
		logFile_ << out;
		return 1;
	} else if (error_) return -1;
	else return 0;
}
