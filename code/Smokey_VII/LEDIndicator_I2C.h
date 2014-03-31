#ifndef _LED_INDICATOR_I2C_H_
#define _LED_INDICATOR_I2C_H_

class I2C;

class LEDIndicator_I2C
{
private:
	static const uint8_t kAddress = 0x08;

	I2C* ap_i2c;

public:
	LEDIndicator_I2C(uint8_t modulePort);
	~LEDIndicator_I2C(void);

	bool SetColor(int red, int green, int blue);
};

#endif
