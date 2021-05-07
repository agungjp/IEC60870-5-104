/*ESEMPIO IEC60870-5-104 SLAVE ETHERNET*/

#include <Ethernet.h>
#include <IEC60870-5-104.h>

//I create the IEC60870-5-104 SERVER instance
EthernetServer srv(2404); //I create the server
IEC104_SLAVE slave(&srv);

//MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 2, 177);

//Memory per timer
unsigned long lastSend=0;

//Setup - runs at startup
void setup()
{
  //I set up serial communication
  Serial.begin(115200);

  //I set up the network card
  Ethernet.init(5);   // MKR ETH shield
  Ethernet.begin(mac, ip);
}

//Loop - Infinite loop on CORE 1
void loop()
{
  //------------------------------------------------------------RECEIVING AN SP or DP COMMAND------------------------------------------------------------//
  
  //With the function available, I update the server and obtain the number of telegrams waiting to be read in output
  int messages = slave.available();

  //I run a for loop to read all the messages in the buffer
  for(int i=0; i<messages; i++)
  {
    //I create support variables to read the buffer
    byte type = 0; int ca = 0; long ioa = 0; long value = 0;

    //I read an array in the buffer
    slave.read(&type,&ca,&ioa,&value);

    //I use the data received
    if(type==C_SC_NA_1) Serial.println("You received Single Point Command from C.A.:"+String(ca)+"-IOA:"+String(ioa)+" with value "+String(value));
    else if(type==C_DC_NA_1) Serial.println("You received Double Point Command from C.A.:"+String(ca)+"-IOA:"+String(ioa)+" with value "+String(value));
  }

  //---------------------------------------------------------------SENDING A SP or ME MEASUREMENT---------------------------------------------------------------//

    //Scelgo il mio Common Address
    int ca = 50;

    //I create four variables to send, two Boolean (Single Point), and two analog (Normalized Measure)
    bool val1 = false;
    bool val2 = true;
    int val3 = 500;
    int val4 = -500;

    //I scale the values to make them normalized
    long val3n = map(500,0,1000,0,32767); 
    long val4n = map(-500,-1000,1000,-32767,32767);

    //I can send several objects of the same type in one telegram
    
    long IOA_SP[2] = {10, 11}; //I enter the IOAs that I will use for single points
    long IOA_ME[2] = {20, 21}; //I enter the IOAs that I will use for the measurements
    
    long VAL_SP[2] = {val1, val2}; //I enter the values
    long VAL_ME[2] = {val3n, val4n}; //I enter the values

    //If 5 "have passed since the last sending, resend the measurement
    if(millis()-lastSend>5000)
    {
      //I send the Measured Normalized Value
      int quantitySP = 2; //Number of items to send
      slave.send(M_ME_NA_1,quantitySP,ca,IOA_SP,VAL_SP);

      //I send the Single Point Information
      int quantityME = 2; //Number of items to send
      slave.send(M_SP_NA_1,quantityME,ca,IOA_ME,VAL_ME);

      //I update the memory
      lastSend=millis();
    }
}
