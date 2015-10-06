/*
 *combination of DHT and MG811, all with wifi 
 * 
*/



#include "Adafruit_CC3000.h"
#include "ccspi.h"
#include <SPI.h>
#include "bloom_streaming_cc3000.h"
#include "DHT.h"


//CC3000 vars
#define WLAN_SSID       "CenturyLink1488"
#define WLAN_PASS       "lokiisgod"
#define WLAN_SECURITY   WLAN_SEC_WPA2


//DHT vars
#define DHTTYPE DHT22
#define DHTPIN 2 
DHT dht(DHTPIN, DHTTYPE);

//MG811 vars
#define         MG_PIN                       (0)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (6)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between samples 
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTGAE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   
                                                     //two points are taken from the curve. 
                                                     //with these two points, a line is formed which is
                                                     //"approximately equivalent" to the original curve.
                                                     //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
                                                     //slope = ( reaction voltage ) / (log400 –log1000) 




                                                     
// Sign up to plotly here: https://plot.ly
// View your API key and streamtokens here: https://plot.ly/settings
#define nTraces 2
// View your tokens here: https://plot.ly/settings
// Supply as many tokens as data traces
// e.g. if you want to ploty A0 and A1 vs time, supply two tokens
char *tokens[nTraces] = {"6xyi34x0k0", "gbg3xqajwn"};
// arguments: username, api key, streaming token, filename
bloom graph("panstix69", "3xnepv6va9", tokens, "dht22_mg811_to_wifi", nTraces);


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte my_ip[] = { 174, 16, 129, 54 }; // google will tell you: "public ip address"


void wifi_connect(){
  /* Initialise the module */
  Serial.println(F("\n... Initializing..."));
  if (!graph.cc3000.begin())
  {
    Serial.println(F("... Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  // Optional SSID scan
  // listSSIDResults();

  if (!graph.cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }

  Serial.println(F("... Connected!"));

  /* Wait for DHCP to complete */
  Serial.println(F("... Request DHCP"));
  while (!graph.cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }
}



void setup() {
  graph.maxpoints = 100;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  wifi_connect();

//  bool success;
//  success = graph.init();
//  //if(!success){while(true){}}
//  graph.openStream();
  dht.begin();

    pinMode(BOOL_PIN, INPUT);                        //set pin to input
    digitalWrite(BOOL_PIN, HIGH);                    //turn on pullup resistors
  
}

float h, t;
int percentage;
float volts;


void loop() {

  //lets try 5 seconds.
  delay(5000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  //float f = dht.readTemperature(true); //no f for now
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    //return;
  } else {
    graph.post_t_h(t,h);  
  }


  delay(5000);
 
    //mg811 reads
  volts = MGRead(MG_PIN);
  percentage = MGGetPercentage(volts,CO2Curve);
  graph.post_co2(percentage);
}







/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of SEN-000007
Remarks: This function reads the output of SEN-000007
************************************************************************************/ 
float MGRead(int mg_pin)
{
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}
 
/*****************************  MQGetPercentage **********************************
Input:   volts   - SEN-000007 output measured in volts
         pcurve  - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(MG-811 output) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}
