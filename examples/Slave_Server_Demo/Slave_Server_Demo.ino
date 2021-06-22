/* Example IEC60870-5-104 SLAVE*/

#include <WiFi.h>
#include <IEC60870-5-104.h>

//Creo l'istanza SERVER IEC60870-5-104
WiFiServer srv(2404); //Creo il server
IEC104_SLAVE slave(&srv);

//Memory for timer
unsigned long lastSend=0;

//Setup - runs at startup
void setup()
{
  //I set up serial communication
  Serial.begin(115200);

  //I set up the network card
  //WiFi.config (local_IP, gtw_IP, sub_IP);
  WiFi.mode(WIFI_STA); //Set the Wi-Fi module as a client
  WiFi.begin("SSID","password"); //Connect to the network
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
    if(type==C_SC_NA_1) Serial.println("Hai ricevuto un Single Point Command da C.A.:"+String(ca)+"-IOA:"+String(ioa)+" uguale a "+String(value));
    else if(type==C_DC_NA_1) Serial.println("Hai ricevuto un Double Point Command da C.A.:"+String(ca)+"-IOA:"+String(ioa)+" uguale a "+String(value));
  }

  //---------------------------------------------------------------SENDING AN SP or ME MEASUREMENT---------------------------------------------------------------//

    //I choose my Common Address
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
