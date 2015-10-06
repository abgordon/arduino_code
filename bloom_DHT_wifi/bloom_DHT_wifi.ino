//connect DH22 sensor to wifi

#include "Adafruit_CC3000.h"
#include "ccspi.h"
#include <SPI.h>
#include "bloom_streaming_cc3000.h"
#include "DHT.h"

#define WLAN_SSID       "CenturyLink1488"
#define WLAN_PASS       "lokiisgod"
#define WLAN_SECURITY   WLAN_SEC_WPA2


//init DH22 sensor
#define DHTTYPE DHT22
#define DHTPIN 2 
DHT dht(DHTPIN, DHTTYPE);


// Sign up to plotly here: https://plot.ly
// View your API key and streamtokens here: https://plot.ly/settings
#define nTraces 2
// View your tokens here: https://plot.ly/settings
// Supply as many tokens as data traces
// e.g. if you want to ploty A0 and A1 vs time, supply two tokens
char *tokens[nTraces] = {"6xyi34x0k0", "gbg3xqajwn"};
// arguments: username, api key, streaming token, filename
bloom graph("panstix69", "3xnepv6va9", tokens, "dht22_to_wifi", nTraces);


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
  Serial.println("init 2");
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
}

float h, t;

void loop() {
  // Wait a few seconds between measurements.
  //delay(2000);
  Serial.println("Beginning Loop.....");
  //lets try 200 seconds
  //delay(200000);
  //lets try 10 seconds.
  delay(10000);

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
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  //float hi = dht.computeHeatIndex(f, h);

//  Serial.print("Humidity: "); 
//  Serial.print(h);
//  Serial.print(" %\t");
//  Serial.print("Temperature: "); 
//  Serial.print(t);
//  Serial.print(" *C \n\r");
//  Serial.print(f);
//  Serial.print(" *F\t");
//  Serial.print("Heat index: ");
//  Serial.print(hi);
//  Serial.println(" *F");

  graph.post_data(t,h );
}
