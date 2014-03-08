#ifndef _LED_INDICATOR_H_
#define _LED_INDICATOR_H_

#include <DigitalOutput.h>

class LEDIndicator
{
public:
	LEDIndicator(int redPort, int greenPort, int bluePort)
	:red_(redPort), green_(greenPort), blue_(bluePort) {setColor(0,0,0);}

	void setColor(int red, int green, int blue);
private:
	DigitalOutput red_;
	DigitalOutput green_;	
	DigitalOutput blue_;
};

#endif
