#include <I2C.h>
#include <DigitalModule.h>

#include "LEDIndicator_I2C.h"

LEDIndicator_I2C::LEDIndicator_I2C(uint8_t modulePort)
:ap_i2c(NULL)
{
	DigitalModule* module = DigitalModule::GetInstance(modulePort); 
	if (module) {
		ap_i2c = module->GetI2C(kAddress);
	}
}

LEDIndicator_I2C::~LEDIndicator_I2C()
{
	delete ap_i2c;
	ap_i2c = NULL;
}

bool LEDIndicator_I2C::SetColor(int red, int green, int blue)
{
	uint8_t out[3];
	out[0] = red;
	out[1] = green;
	out[2] = blue;
	return ap_i2c->Transaction(out, 3, NULL, 0); 
}
