#include "Arduino.h"
#include "Adafruit_CC3000.h"
#include "ccspi.h"
#include <SPI.h>
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
#define AWS_PORT 49152
#include "bloom_streaming_cc3000.h"a

#define USERNAME "mg"
#define DEVICE_ID "1"
#define TEMP_H "1"
#define CO2 "2"

//#include <avr/dtostrf.h>
#include <avr/pgmspace.h>


bloom::bloom(char *username, char *api_key, char* stream_tokens[], char *filename, int nTraces)
  : cc3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,SPI_CLOCK_DIV2){
    log_level = 2;  // 0 = Debugging, 1 = Informational, 2 = Status, 3 = Errors, 4 = Quiet (// Serial Off)
    dry_run = false;
    username_ = username;
    api_key_ = api_key;
    stream_tokens_ = stream_tokens;
    filename_ = filename;
    nTraces_ = nTraces;
    maxpoints = 30;
    fibonacci_ = 1;
    world_readable = true;
    convertTimestamp = true;
    timezone = "America/Montreal";
    fileopt = "overwrite";
}

void bloom::print_(int d){
    if(log_level < 2) Serial.print(d);
    if(!dry_run) client.print(d);
}
void bloom::print_(unsigned long d){
    if(log_level < 2) Serial.print(d);
    if(!dry_run) client.print(d);
}
void bloom::print_(float d){
    if(log_level < 2) Serial.print(d);
    if(!dry_run) client.print(d);
}
void bloom::print_(char *d){
    if(log_level < 2) Serial.print(d);
    if(!dry_run) client.fastrprint(d);
}
void bloom::print_(const __FlashStringHelper* d){
    if(log_level < 2) Serial.print(d);
    if(!dry_run) client.fastrprint(d);
}

char *dtostrf (float val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}






void bloom::post_t_h(float t, float h) {    
    //
    //  Validate a stream with a REST post to bloom
    //
    if(dry_run && log_level < 3){
        Serial.println(F("... This is a dry run, we are not connecting to bloom's servers..."));
    }
    else if(log_level < 3) {
        Serial.println(F("... Attempting to connect to blooms' REST servers"));
    }

    uint32_t ip = 0;
    Serial.print(F("... Attempting to resolve IP address of bloom..."));
    while  (ip  ==  0)  {
        if  (!  cc3000.getHostByName("ec2-52-26-172-154.us-west-2.compute.amazonaws.com", &ip))  {
        //if  (!  cc3000.getHostByName("https://sqs.us-west-2.amazonaws.com/282218789794/arduino_datapoints", &ip))  {
          Serial.println(F("Couldn't resolve!"));
        }
        
        delay(500);
    }

    cc3000.printIPdotsRev(ip);

    // Try looking up the website's IP address
    client = cc3000.connectTCP(ip, AWS_PORT);
    while ( !client.connected() ) {
      if(log_level < 4) {
        Serial.println(F("... Couldn\'t connect to Blooms' REST servers... trying again!"));
      }
      delay(10000);
      client = cc3000.connectTCP(ip, AWS_PORT); 
    }
    if(log_level < 3){} Serial.println(F("... Connected to Blooms' REST servers"));
    if(log_level < 3){} Serial.println(F("... Sending HTTP Post to Bloom"));


    // ************************************************************
    // begin POST formatting
    Serial.print("\r\n"); //for my sanity

//application/x-www-form-urlencode

    print_(F("POST /postdata HTTP/1.1\r\n"));
    Serial.print("POST /postdata HTTP/1.1\r\n");
    print_(F("Host: http://52.26.172.154:49152\r\n"));
    Serial.print("Host: http://52.26.172.154:49152\r\n");
    print_(F("User-Agent: Arduino/0.6.0\r\n"));
    Serial.print("User-Agent: Arduino/0.6.0\r\n");
    print_(F("Accept: application/x-www-form-urlencoded\r\n"));
    Serial.print("Accept: application/x-www-form-urlencoded\r\n");
    print_(F("Content-Length: "));
    Serial.print("Content-Length: ");
    
    //21 is magic number for some reason...
    //longer messages cause read ECONNRESET, need to add a error handler to reset server in this case?
    int contentLength = 12;

    print_(contentLength);
    Serial.print(contentLength);
    print_(F("\r\n"));
    Serial.print("\r\n");
    print_(F("Content-Type: application/x-www-form-urlencoded\r\n\r\n"));  
    Serial.print("Content-Type: application/x-www-form-urlencoded\r\n\r\n");

    // Start printing querystring body
    // This is parsed server-side
    //input type: temp,h
    print_(TEMP_H);
    Serial.print(TEMP_H);
//    print_(F("%3A"));
//    Serial.print("%3A");
//    print_(USERNAME);
//    Serial.print(USERNAME);
//    //split character :
//    print_(F("%3A"));
//    Serial.print("%3A");
//    print_(DEVICE_ID);
//    Serial.print(DEVICE_ID);
    print_(F("%3A"));
    Serial.print("%3A");    
    print_(t);
    Serial.print(t);
    print_(F("%3A"));
    Serial.print("%3A");
    print_(h);
    Serial.print(h);
    // final newline to terminate the POST
    print_(F("\r\n"));
    Serial.print("\r\n");

    // end POST formatting 
    // ************************************************************

    Serial.println("Post sent, closing connection.....");
    client.close();
}


void bloom::post_co2(int percentage) {    
    //
    //  Validate a stream with a REST post to bloom
    //
    if(dry_run && log_level < 3){
        Serial.println(F("... This is a dry run, we are not connecting to bloom's servers..."));
    }
    else if(log_level < 3) {
        Serial.println(F("... Attempting to connect to blooms' REST servers"));
    }

    uint32_t ip = 0;
    Serial.print(F("... Attempting to resolve IP address of bloom..."));
    while  (ip  ==  0)  {
        if  (!  cc3000.getHostByName("ec2-52-26-172-154.us-west-2.compute.amazonaws.com", &ip))  {
        //if  (!  cc3000.getHostByName("https://sqs.us-west-2.amazonaws.com/282218789794/arduino_datapoints", &ip))  {
          Serial.println(F("Couldn't resolve!"));
        }
        
        delay(500);
    }

    cc3000.printIPdotsRev(ip);

    // Try looking up the website's IP address
    client = cc3000.connectTCP(ip, AWS_PORT);
    while ( !client.connected() ) {
      if(log_level < 4) {
        Serial.println(F("... Couldn\'t connect to Blooms' REST servers... trying again!"));
      }
      delay(10000);
      client = cc3000.connectTCP(ip, AWS_PORT); 
    }
    if(log_level < 3){} Serial.println(F("... Connected to Blooms' REST servers"));
    if(log_level < 3){} Serial.println(F("... Sending HTTP Post to Bloom"));


    // ************************************************************
    // begin POST formatting
    Serial.print("\r\n"); //for my sanity

//application/x-www-form-urlencode

    print_(F("POST /postdata HTTP/1.1\r\n"));
    Serial.print("POST /postdata HTTP/1.1\r\n");
    print_(F("Host: http://52.26.172.154:49152\r\n"));
    Serial.print("Host: http://52.26.172.154:49152\r\n");
    print_(F("User-Agent: Arduino/0.6.0\r\n"));
    Serial.print("User-Agent: Arduino/0.6.0\r\n");
    print_(F("Accept: application/x-www-form-urlencoded\r\n"));
    Serial.print("Accept: application/x-www-form-urlencoded\r\n");
    print_(F("Content-Length: "));
    Serial.print("Content-Length: ");
    
    //21 is magic number for some reason...
    //longer messages cause read ECONNRESET, need to add a error handler to reset server in this case?
    int contentLength = 8;

    print_(contentLength);
    Serial.print(contentLength);
    print_(F("\r\n"));
    Serial.print("\r\n");
    print_(F("Content-Type: application/x-www-form-urlencoded\r\n\r\n"));  
    Serial.print("Content-Type: application/x-www-form-urlencoded\r\n\r\n");

    // Start printing querystring body
    // This is parsed server-side
    //input type: co2
    print_(CO2);
    Serial.print(CO2);
    print_(F("%3A"));
    Serial.print("%3A");
//    print_(USERNAME);
//    Serial.print(USERNAME);
//    //split character :
//    print_(F("%3A"));
//    Serial.print("%3A");
//    print_(DEVICE_ID);
//    Serial.print(DEVICE_ID);
//    print_(F("%3A"));
//    Serial.print("%3A");    
    print_(percentage);
    Serial.print(percentage);
    // final newline to terminate the POST
    print_(F("\r\n"));
    Serial.print("\r\n");

    // end POST formatting 
    // ************************************************************

    Serial.println("Post sent, closing connection.....");
    client.close();
}

