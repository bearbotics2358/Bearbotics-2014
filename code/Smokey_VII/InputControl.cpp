#include <Timer.h>

#include "InputControl.h"

void ToggleLatch::Toggle(void)
{
	if (time_.HasPeriodPassed(timeBetween_)) {
		value_ = !value_;
		time_.Reset();
	}
}

void ToggleLatch::Init(void) 
{
	time_.Reset(); 
	time_.Start();
}

void ToggleLatch::Disable(void)
{
	time_.Stop();
	time_.Reset();
}
