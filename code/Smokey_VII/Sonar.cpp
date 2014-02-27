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
	sport->DisableTermination();
	sport->SetWriteBufferMode(SerialPort::kFlushOnAccess);
	sport->SetFlowControl(SerialPort::kFlowControl_None);
	sport->SetReadBufferSize(0);
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
	while(1) {
		int ret = sport->GetBytesReceived();
		printf("rcvd: %d\n", ret);
		if(ret == 0) {
			break;
		}
		// read one char
		printf("char rcvd ...");
		ret = sport->Read(&rx_buff[rx_index], 1);
		printf("ret: %d\n", ret);
		fflush(stdout);
		if((rx_buff[rx_index] == '\r') 
				|| (rx_buff[rx_index] == '\t')) {
			// process reading
			rx_buff[rx_index] = 0;
			printf("raw sonar: %s\n", rx_buff);
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
      break;
    } else {
      rx_index++;
			// should never happen
			if(rx_index >= SONAR_BUFF_SIZE) {
				rx_index = 0;
			}
    }
  }/* else {
	  printf("Nothing Recieved\n");
  }*/
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

void Sonar::RxFlush()
{
	char Kiwi;
	while(sport->GetBytesReceived() > 0){
		sport->Read(&Kiwi,1);
	}
	rx_index = 0;
}

