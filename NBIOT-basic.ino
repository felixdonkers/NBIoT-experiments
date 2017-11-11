#include "Arduino.h"
#if defined(ARDUINO_AVR_LEONARDO)
#define USB Serial
#define UBLOX Serial1
#elif defined(ARDUINO_SODAQ_EXPLORER)
#define USB SerialUSB
#define UBLOX Serial
#elif defined(ARDUINO_SAM_ZERO)
#define USB SerialUSB
#define UBLOX Serial1
 
#else
#error "Please select a Arduino Leonardo, Sodaq ExpLoRer, M0, Crowduino or add your board."
#endif
 
// Pin to turn on/off the nb-iot module
#define powerPin 7
#define commands 25
 
String atcommands[commands] = {  "AT+NRB",
              "AT+NCONFIG?",
              "AT+NCONFIG=\"AUTOCONNECT\",\"FALSE\"",
              "AT+NCONFIG=\"CR_0354_0338_SCRAMBLING\",\"FALSE\"",
              "AT+NCONFIG=\"CR_0859_SI_AVOID\",\"FALSE\"",
              "AT+CFUN=0",
              "AT+NCDP=\"172.16.14.22\"",
              "AT+CGDCONT=1,\"IP\",\"oceanconnect.t-mobile.nl\"",
              "AT+CFUN=1",
              "AT+COPS=1,2,\"20416\"",
              "AT+CSQ",
              "AT+CGATT?",
              "AT+CGPADDR",
              "AT+NSMI=1",
              "AT+NMGS=11,\"48454c4c4f2046454c4958\"",
              "AT+NQMGS",
              "AT+NUESTATS",
              "AT+CEREG=4",
              "AT+CEREG?",
              "ATI9",
              "AT+CSCON=1",
              "AT+CSCON?",
              "AT+CGSN=1",
              "AT+CPSMS?",
              "AT+CPSMS=1"
              };
int pos = 0;
 
 
void setup()
{
  // Turn the nb-iot module on
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  // Start communication
  USB.begin(9600);
  UBLOX.begin(9600);  //ublox should always be 9600
  while ((!USB) && (millis() < 10000)) {
    // Wait for USB or start after 10 seconds
  }
  USB.println("type an AT command or type m for Menu of commands"); 
  delay(1000);
  //clear UBLOX buffer
  while(UBLOX.available()) {
    UBLOX.read();
  }
}
 
void loop()
{
  int len=160;  // allow commands up to 160 characters
  char buffer[len];  
  int commandFound= 0;
  if (USB.available())
  {
    uint8_t c = USB.read();
    if(pos==0 && c=='m')
    {
      for(int x=0 ; x < commands ; x++){
        USB.print(x+1);
        if(x<9){
          USB.print("   ");
        } else
        {
            USB.print("  ");
        }
        USB.println(atcommands[x]);
      }
      USB.println("you can use one of the above shortcodes, just type the number and press enter");
      buffer[0]=0;
    } else
    {
      switch (c) {
          case '\n': // Ignore new-lines
            break;
          case '\r': // Return on CR
            commandFound=1;
            break;
          default:
            USB.write(c);   //display what is typed
            if (pos < len-1) {
              buffer[pos++] = c;
              buffer[pos] = 0;
            }
   
      }
      if(commandFound ){
        String inputline(buffer);
        int menuchoice=inputline.toInt();
        USB.println();
        if(menuchoice>0){
          inputline=atcommands[menuchoice-1];
          USB.println(inputline);
        }
        UBLOX.println(inputline);
        commandFound=0;
        pos = 0;
       }   
      }
    }
    while (UBLOX.available())
    {    
      USB.write(UBLOX.read());
    }
  
}
 
