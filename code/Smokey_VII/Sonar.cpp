/* Sonar.cpp - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

*/

#include "Sonar.h"
#include <stdlib.h> // atoi

Sonar::Sonar(int baud)
{
	int i;

	sport = new SerialPort(baud);
	sport->SetWriteBufferMode(SerialPort::kFlushOnAccess);
	rx_index = 0;
	for(i = 0; i < SONAR_BUFF_SIZE; i++) {
		rx_buff[i] = 0;
	}
}

void Sonar::periodic()
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
			if(rx_index >= SONAR_BUFF_SIZE) {
				rx_index = 0;
			}
    }
  }
}  

int Sonar::GetCM(SensorEnum port)
{
	return range[port];
}

float Sonar::GetFeet(SensorEnum port)
{
	return ((float)range[port])/(2.54 * 12.0);
}

float Sonar::GetDistanceFront()
{
	return GetFeet(kLeftFront);
}

float Sonar::GetDistanceRear()
{
	return GetFeet(kRear);
}

void Sonar::EnablePort(SensorEnum port)
{
	char cmd[3];
	cmd[0] = 'A' + port;
	cmd[1] = '1';
	cmd[2] = '\r';
	sport->Write(cmd, 3);
	sport->Flush();
}

void Sonar::DisablePort(SensorEnum port)
{
	char cmd[3];
	cmd[0] = 'A' + port;
	cmd[1] = '0';
	cmd[2] = '\r';
	sport->Write(cmd, 3);
	sport->Flush();
}

void Sonar::EnableFrontOnly()
{
	EnablePort(kLeftFront);
	EnablePort(kRightFront);
	DisablePort(kRear);
	DisablePort(kNone);
}

void Sonar::EnableRearOnly()
{
	DisablePort(kLeftFront);
	DisablePort(kRightFront);
	EnablePort(kRear);
	DisablePort(kNone);
}

