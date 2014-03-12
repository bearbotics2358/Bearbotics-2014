/* Sonar.cpp - receive ultrasonic ranging measurements from sensors and
	 control which are active using Ardunio based ultrasonic multiplexer

	 created 2/16/14 BD

	 updated 3/11/14 BD
	 - added past values of range to permit filtering out bad samples
*/

#include "Sonar.h"
#include <stdio.h>
#include <stdlib.h> // atoi

Sonar::Sonar(int baud)
{
	int i;
	int j;

	sport = new SerialPort(baud);
	sport->DisableTermination();
	sport->SetWriteBufferMode(SerialPort::kFlushOnAccess);
	sport->SetFlowControl(SerialPort::kFlowControl_None);
	sport->SetReadBufferSize(0);
	rx_index = 0;
	for(i = 0; i < SONAR_BUFF_SIZE; i++) {
		rx_buff[i] = 0;
	}
	for(i = 0; i < SONAR_UNITS; i++ ) {
		for(j = 0; j < SONAR_HISTORY_MAX; j++) {
			range[i][j] = 0;
		}
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
	int i;

	while(1) {
		int ret = sport->GetBytesReceived();
		// printf("rcvd: %d\n", ret);
		if(ret == 0) {
			break;
		}
		// read one char
		// printf("char rcvd ...");
		ret = sport->Read(&rx_buff[rx_index], 1);
		// printf("\n");
		fflush(stdout);
		if((rx_buff[rx_index] == '\r') 
				|| (rx_buff[rx_index] == '\t')) {
			// process reading
			rx_buff[rx_index] = 0;
//			printf("raw sonar: %s\n", rx_buff);
			if(rx_index == 4) {
				switch(rx_buff[0]) {
				case 'R': // for ultrasonic unit connected directly to CRIO
				case 'A':
					for(i = SONAR_HISTORY_SIZE - 1; i > 0; i--) {
						range[0][i] = range[0][i - 1];
					}
					range[0][0] = atoi(&rx_buff[1]);
					break;

				case 'B':
					for(i = SONAR_HISTORY_SIZE - 1; i > 0; i--) {
						range[1][i] = range[1][i - 1];
					}
					range[1][0] = atoi(&rx_buff[1]);
					break;

				case 'C':
					for(i = SONAR_HISTORY_SIZE - 1; i > 0; i--) {
						range[2][i] = range[2][i - 1];
					}
					range[2][0] = atoi(&rx_buff[1]);
					break;

				case 'D':
					for(i = SONAR_HISTORY_SIZE - 1; i > 0; i--) {
						range[3][i] = range[3][i - 1];
					}
					range[3][0] = atoi(&rx_buff[1]);
					break;

				default:
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
//	  printf("Nothing Recieved\n");
  }*/
}  

int Sonar::GetCM(SensorEnum port)
{
	return range[port][0];
}

int Sonar::GetCMsafe(SensorEnum port)
{
	int ret;
	
	// try to determine if new reading is bogus
	// observed bad readings are single readings, surrounded by good readings
	// first try, compare delta readings - throw this one out if it appears
	// too large
	if(abs(range[port][0] - range[port][1])
		 < 5 * abs(range[port][1] - range[port][2])) {
		// appears ok, return this reading
		ret = range[port][0];
	} else {
		// looks erroneous, use last reading
		ret = range[port][1];
	}
	return ret;
}

float Sonar::GetFeet(SensorEnum port)
{
	return ((float)GetCMsafe(port))/(2.54 * 12.0);
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

