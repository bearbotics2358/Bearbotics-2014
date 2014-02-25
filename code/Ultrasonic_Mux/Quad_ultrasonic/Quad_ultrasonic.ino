/*
  Quad_ultrasonic
  
  This program reads up to 4 Maxbotix ultrasonic sensors and sends
  the output back to the connected computer (CRIO).
  
  The program triggers one sensor at a time and reads its response.
  It waits up to 100 msec for the response.  If none recevied, it
  substitutes a reading of "999".
  
  A Maxbotix sensor output string example is "R123<CR>".  The program
  substitutes "A", "B", "C", or "D" for the "R", depending on which
  sensor.
  
  The CR is replaced with a TAB for all but the last enabled sensor
  to assist in debugging.
  
  The program uses the Software Serial to read the sensors.  This
  must be the New Software Serial library.  It is the included lib
  for version of Arduino >= 1...
  
  Not all pins on the Leonardo support change interrupts, 
 so only the following can be used for RX: 
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 
 created 2/12/14 BD from a dual software serial example
 
 updated 2/15/14
 - adding commands to this unit from CRIO
 A0, A1, B0, B1, C0, C1, D0, D1
 updated 2/16/14
 - all working now
 
 
 */

#include <SoftwareSerial.h>
#include <ctype.h>

SoftwareSerial *serial[4];

// trigger lines to sensors
int trig[4] = {8, 7, 6, 5};
// indicate which channel is currently active
int leds[4] = {10, 15, 17, 19};
// channel tx letters
int letters[4] = {'A', 'B', 'C', 'D'};
// channel enable flags
int enable[4] = {1, 1, 1, 1};
char cmd_buf[10];
int cmd_index = 0;

int i;
long t1;

void setup()
{
  // port to CRIO
  Serial.begin(38400);

  // SoftwareSerial(rx pin,tx pin, inverted)
  serial[0] = new SoftwareSerial(9, 2, false);
  serial[1] = new SoftwareSerial(14, 2, false);
  serial[2] = new SoftwareSerial(16, 2, false);
  serial[3] = new SoftwareSerial(18, 2, false);

  for(i = 0; i < 4; i++) {
    // Start each software serial port
    serial[i]->begin(9600);
    // Setup trigger outputs
    pinMode(trig[i], OUTPUT);
    digitalWrite(trig[i], LOW);
    // Setup indicator leds
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void loop()
{
  char rxbuf[10];
  int j;
  int max_sensor = 0;
  char c1;
  char c2;
  int k;

  // get command if there is one
  // every time through loop, get command chars if available
  // when '\r' (or '\n') found, process command
  while(Serial.available() > 0) {
    cmd_buf[cmd_index] = Serial.read();
    if((cmd_buf[cmd_index] == '\r') 
      || (cmd_buf[cmd_index] == '\n')) {
      // process command
      cmd_buf[cmd_index + 1] = 0;
      //Serial.print("cmd = ");
      Serial.println(cmd_buf);
      if(cmd_index == 2) {
        // temp variables in case optimizer is not efficient
        c1 = toupper(cmd_buf[0]);
        c2 = cmd_buf[1];
        if((c1 >= 'A') && (c1 <= 'D')
        && (c2 >= '0') && (c2 <= '1')) {
          enable[int(c1 - 'A')] = int(c2 - '0');
        }
      }

      // reset for next command
      cmd_index = 0;
    } else {
      cmd_index++;
    }
  }
  
  // check and process sensors
  for(i = 0; i < 4; i++) {
    // clear receive buffer
    for(j = 0; j < 10; j++) {
      rxbuf[j] = 0;
    }
    if(enable[i]) {
      // if this channel enabled, trigger and read it
      // indicate active channel
      digitalWrite(leds[i], HIGH);
      // activate serial port and clear rx buff
      serial[i]->listen();
      serial[i]->flush();
      t1 = millis();
      // trigger reading
      digitalWrite(trig[i], HIGH);

      // while there is data coming in, read it
      while(1) {
        if(serial[i]->available() > 0) {
          // shift rx buffer
          for(j = 0; j < 4; j++) {
            rxbuf[j] = rxbuf[j + 1];
          }
          rxbuf[4] = serial[i]->read() & 0x00ff;
          if(rxbuf[4] == '\r') {
            break;
          }
        }
        if(millis() > t1 + 100) {
          // timeout - set to distance of "999"
          rxbuf[1] = '9';
          rxbuf[2] = '9';
          rxbuf[3] = '9';
          rxbuf[4] = '\r';
          break;
        }
      }
      // ok, got a message, stop trigger
      digitalWrite(trig[i], LOW);
      // substitute port letter for 'R'
      rxbuf[0] = letters[i];    
      // determine if this is the last reading
      max_sensor = 0;
      for(j = 0; j < 4; j++) {
        if(enable[j]) {
          max_sensor = j;
        }
      }
      // if not last reading, substitute tab instead of CR
      if(i != max_sensor) {
        rxbuf[4] = '\t';
      }
      rxbuf[5] = 0;
      Serial.print(rxbuf);
      // turn off indicator led
      digitalWrite(leds[i], LOW);
    }
  }
}






