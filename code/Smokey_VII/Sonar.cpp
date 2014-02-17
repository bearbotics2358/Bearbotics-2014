/* Sonar.cpp - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

*/

#include <stdlib.h> // atoi

Sonar::Sonar(int baud)
{
	int i;

	sport = new SerialPort(baud);
	rx_index = 0;
	for(i = 0; i < BUFF_SIZE; i++) {
		rx_buff[i] = 0;
	}
}

Sonar::periodic()
{
	// call this routine periodically to check for any readings and store
	// into result registers

  // get report if there is one
  // every time through loop, get command chars if available
	// and add to rx buffer
  // when '\r' (or '\t') found, process reading
  while(sport->GetBytesReceived() > 0) {
		// read one char
		sport->Read(&rx_buff[rx_index], 1);
    if((rx_buff[rx_index] == '\r') 
      || (rx_buff[rx_index] == '\t')) {
      // process reading
      rx_buff[rx_index] = 0;
			if(rx_index == 4) {
				switch(rx_buff[0]) {
				case 'A':
					range[0] = atoi(&rx_buff[1]);
					break;

				case 'B':
					range[1] = atoi(&rx_buff[1]);
					break;

				case 'C':
					range[2] = atoi(&rx_buff[1]);
					break;

				case 'D':
					range[3] = atoi(&rx_buff[1]);
					break;

				}
			}
      // reset for next command
      rx_index = 0;
    } else {
      rx_index++;
			// should never happen
			if(rx_index >= BUFF_LENGTH) {
				rx_index = 0;
			}
    }
  }
}  

int Sonar::GetCM(int port)
{
	return range[i];
}

float Sonar::GetFeet(int port)
{
	return ((float)range[i])/(2.54 * 12.0);
}

float Sonar::GetDistanceGoal()
{
	return GetFeet(LEFT_FRONT);
}

float Sonar::GetDistanceDriver()
{
	return GetFeet(REAR);
}

Sonar::EnablePort(int port)
{
	char cmd[3];
	cmd[0] = 'A' + port;
	cmd[1] = '1';
	cmd[2] = '\r';
	sport->Write(cmd, 3);
	sport->Flush();
}

Sonar::DisablePort(int port)
{
	char cmd[3];
	cmd[0] = 'A' + port;
	cmd[1] = '0';
	cmd[2] = '\r';
	sport->Write(cmd, 3);
	sport->Flush();
}

Sonar::EnableFrontOnly()
{
	EnablePort(LEFT_FRONT);
	EnablePort(RIGHT_FRONT);
	DisablePort(REAR);
	DisablePort(4);
}

Sonar::EnableRearOnly()
{
	DisablePort(LEFT_FRONT);
	DisablePort(RIGHT_FRONT);
	EnablePort(REAR);
	DisablePort(4);
}

