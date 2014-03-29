#include "LEDIndicator.h"

void LEDIndicator::SetColor(int red, int green, int blue)
{
	red_.Set((red > 0) ? 0 : 1);
	green_.Set((green > 0) ? 0 : 1);
	blue_.Set((blue > 0) ? 0 : 1);
}
