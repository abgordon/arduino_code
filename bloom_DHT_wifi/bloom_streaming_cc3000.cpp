#include "Arduino.h"
#include "Adafruit_CC3000.h"
#include "ccspi.h"
#include <SPI.h>
#define ADAFRUIT_CC3000_IRQ   3
#define ADAFRUIT_CC3000_VBAT  5
#define ADAFRUIT_CC3000_CS    10
#define AWS_PORT 49152
#include "bloom_streaming_cc3000.h"a

//#include <avr/dtostrf.h>
#include <avr/pgmspace.h>


bloom::bloom(char *username, char *api_key, char* stream_tokens[], char *filename, int nTraces)
  : cc3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,SPI_CLOCK_DIV2){
    log_level = 0;  // 0 = Debugging, 1 = Informational, 2 = Status, 3 = Errors, 4 = Quiet (// Serial Off)
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

bool bloom::init(){
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
          Serial.println(F("Couldn't resolve!"));
        }
        
        delay(500);
    }
    cc3000.printIPdotsRev(ip);

    //weird fib delay....
    // Try looking up the website's IP address
    //client = cc3000.connectTCP(ip, AWS_PORT);
//    while ( !client.connected() ) {
//        if(log_level < 4){
//            Serial.println(F("... Couldn\'t connect to Blooms' REST servers... trying again!"));
//        }
//        fibonacci_ += fibonacci_;
//        delay(min(fibonacci_, 60000));
//        client = cc3000.connectTCP(ip, AWS_PORT);
//    }

    // Try looking up the website's IP address
    client = cc3000.connectTCP(ip, AWS_PORT);
    while ( !client.connected() ) {
      if(log_level < 4) {
        Serial.println(F("... Couldn\'t connect to Blooms' REST servers... trying again!"));
      }
      delay(10000);
      client = cc3000.connectTCP(ip, AWS_PORT); 
    }
    fibonacci_ = 1;
    if(log_level < 3){} Serial.println(F("... Connected to Blooms' REST servers"));
    if(log_level < 3){} Serial.println(F("... Sending HTTP Post to Bloom"));
    print_(F("POST /clientresp HTTP/1.1\r\n"));
    print_(F("Host: http://52.26.172.154:49152\r\n"));
    print_(F("User-Agent: Arduino/0.6.0\r\n"));

    print_(F("Content-Length: "));
    int contentLength = 126 + len_(username_) + len_(fileopt) + nTraces_*(87+len_(maxpoints)) + (nTraces_-1)*2 + len_(filename_);
    if(world_readable){
        contentLength += 4;
    } else{
        contentLength += 5;
    }
    print_(contentLength);
    // contentLength =
    //   44  // first part of querystring below
    // + len_(username)  // upper bound on username length
    // + 5   // &key=
    // + 10  // api_key length
    // + 7  // &args=[...
    // + nTraces*(87+len(maxpoints)) // len({\"y\": [], \"x\": [], \"type\": \"scatter\", \"stream\": {\"token\": \") + 10 + len(\", "maxpoints": )+len(maxpoints)+len(}})
    // + (nTraces-1)*2 // ", " in between trace objects
    // + 22  // ]&kwargs={\"fileopt\": \"
    // + len_(fileopt)
    // + 16  // \", \"filename\": \"
    // + len_(filename)
    // + 21 // ", "world_readable":
    // + 4 if world_readable, 5 otherwise
    // + 1   // closing }
    //------
    // 126 + len_(username) + len_(fileopt) + nTraces*(86+len(maxpoints)) + (nTraces-1)*2 + len_(filename)
    //
    // Terminate headers with new lines
    print_(F("\r\n\r\n"));

    // Start printing querystring body
    print_(F("version=2.3&origin=plot&platform=arduino&un="));
    print_(username_);
    print_(F("&key="));
    print_(api_key_);
    print_(F("&args=["));
    // print a trace for each token supplied
    for(int i=0; i<nTraces_; i++){
        print_(F("{\"y\": [], \"x\": [], \"type\": \"scatter\", \"stream\": {\"token\": \""));
        print_(stream_tokens_[i]);
        print_(F("\", \"maxpoints\": "));
        print_(maxpoints);
        print_(F("}}"));
        if(nTraces_ > 1 && i != nTraces_-1){
            print_(F(", "));
        }
    }
    print_(F("]&kwargs={\"fileopt\": \""));
    print_(fileopt);
    print_(F("\", \"filename\": \""));
    print_(filename_);
    print_(F("\", \"world_readable\": "));
    if(world_readable){
        print_("true");
    } else{
        print_("false");
    }
    print_(F("}"));
    // final newline to terminate the POST
    print_(F("\r\n"));

    //
    // Wait for a response
    // Parse the response for the "All Streams Go!" and proceed to streaming
    // if we find it
    //

    if(log_level < 2){
        Serial.println(F("... Sent message, waiting for bloom's response..."));
    }
    if(!dry_run){
        char c;
        while(client.connected()){
            if(client.available()){
                c = client.read();

                Serial.print(c);
                client.close();
            }
        }
    }
    return true;

}
void bloom::openStream() {
    //
    // Start request to stream servers
    //
    if(log_level < 3){} Serial.println(F("... Connecting to bloom's streaming servers..."));

    //#define STREAM_SERVER "arduino.plot.ly"
    #define STREAM_SERVER "ec2-52-26-172-154.us-west-2.compute.amazonaws.com"
    Serial.println(F("... Looking up the IP address of ec2-52-26-172-154.us-west-2.compute.amazonaws.com"));
    uint32_t stream_ip = 0;
    // Try looking up the website's IP address
    while (stream_ip == 0) {
        if (! cc3000.getHostByName(STREAM_SERVER, &stream_ip)) {
            if(log_level < 4){} Serial.println(F("Couldn't resolve!"));
        }
    }

    client = cc3000.connectTCP(stream_ip, AWS_PORT);
    while ( !client.connected() ) {
        if(log_level < 4){} Serial.println(F("... Couldn\'t connect to servers... trying again!"));
        fibonacci_ += fibonacci_;
        delay(min(fibonacci_, 60000));
        client = cc3000.connectTCP(stream_ip, AWS_PORT);
    }
    fibonacci_ = 1;
    if(log_level < 3){} Serial.println(F("... Connected to bloom's streaming servers\n... Initializing stream"));

    print_(F("POST / HTTP/1.1\r\n"));
    print_(F("Host: arduino.http://52.26.172.154/\r\n"));
    print_(F("User-Agent: Arduino\r\n"));
    print_(F("Transfer-Encoding: chunked\r\n"));
    print_(F("Connection: close\r\n"));
    if(convertTimestamp){
        print_(F("bloom-convertTimestamp: \""));
        print_(timezone);
        print_(F("\"\r\n"));
    }
    print_(F("\r\n"));

    if(log_level < 3){} Serial.println(F("... Done initializing, ready to stream!"));
}

void bloom::closeStream(){
    print_(F("0\r\n\r\n"));
    client.close();
}
void bloom::reconnectStream(){
    while(!client.connected()){
        if(log_level<4) Serial.println(F("... Disconnected from streaming servers"));
        closeStream();
        openStream();
    }
}
void bloom::jsonStart(int i){
    // Print the length of the message in hex:
    // 15 char for the json that wraps the data: {"x": , "y": }\n
    // + 23 char for the token: , "token": "abcdefghij"
    // = 38
    if(log_level<2) Serial.print(i+44, HEX);
    if(!dry_run) client.print(i+44, HEX);
    print_(F("\r\n{\"x\": "));
}
void bloom::jsonMiddle(){
    print_(F(", \"y\": "));
}
void bloom::jsonEnd(char *token){
    print_(F(", \"streamtoken\": \""));
    print_(token);
    print_(F("\"}\n\r\n"));
}

int bloom::len_(int i){
    // int range: -32,768 to 32,767
    if(i > 9999) return 5;
    else if(i > 999) return 4;
    else if(i > 99) return 3;
    else if(i > 9) return 2;
    else if(i > -1) return 1;
    else if(i > -10) return 2;
    else if(i > -100) return 3;
    else if(i > -1000) return 4;
    else if(i > -10000) return 5;
    else return 6;
}
int bloom::len_(unsigned long i){
    // max length of unsigned long: 4294967295
    if(i > 999999999) return 10;
    else if(i > 99999999) return 9;
    else if(i > 9999999) return 8;
    else if(i > 999999) return 7;
    else if(i > 99999) return 6;
    else if(i > 9999) return 5;
    else if(i > 999) return 4;
    else if(i > 99) return 3;
    else if(i > 9) return 2;
    else return 1;
}
int bloom::len_(char *i){
    return strlen(i);
}
void bloom::plot(unsigned long x, int y, char *token){
    reconnectStream();  
    jsonStart(len_(x)+len_(y));
    print_(x);
    jsonMiddle();
    print_(y);
    jsonEnd(token);
}


void bloom::plot(unsigned long x, float y, char *token){

    reconnectStream();

    char s_[15];
    dtostrf(y,2,3,s_);

    jsonStart(len_(x)+len_(s_)-1);
    print_(x);
    jsonMiddle();
    print_(y);
    jsonEnd(token);
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


void bloom::post_data(float t, float h) {

    
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
          Serial.println(F("Couldn't resolve!"));
        }
        
        delay(500);
    }
    cc3000.printIPdotsRev(ip);

    // Try looking up the website's IP address
    client = cc3000.connectTCP(ip, AWS_PORT);
    while ( !client.connected() ) {
        if(log_level < 4){
            Serial.println(F("... Couldn\'t connect to Blooms' REST servers... trying again!"));
        }
        fibonacci_ += fibonacci_;
        delay(min(fibonacci_, 60000));
        client = cc3000.connectTCP(ip, AWS_PORT);
    }
    fibonacci_ = 1;
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
    

    int contentLength = 10;
//    if(world_readable){
//        contentLength += 4; 
//    } else{
//        contentLength += 5;
//    }
    print_(contentLength);
    Serial.print(contentLength);
    print_(F("\r\n"));
    Serial.print("\r\n");
    print_(F("Content-Type: application/x-www-form-urlencoded\r\n\r\n"));  
    Serial.print("Content-Type: application/x-www-form-urlencoded\r\n\r\n");

    // Start printing querystring body
    // This is parsed server-side
    print_(t);
    Serial.print(t);

    //split character :
    print_(F("%3A"));
    Serial.print("%3A");

    print_(h);
    Serial.print(h);
  


    // final newline to terminate the POST
    print_(F("\r\n"));
    Serial.print("\r\n");

    //Serial.print("\r\n"); //for my sanity
    // end POST formatting 
    // ************************************************************

    Serial.println("Post sent, closing connection.....");
    client.close();


}





//    print_(F("temp_f="));
//    Serial.print("temp_f=");
//    print_(t);
//    Serial.print(t);
//    print_(F("&relative_humidity="));
//    Serial.print("&relative_humidity=");
//    print_(h);
//    Serial.print(h);
//    print_(F("\r\n"));
//    Serial.print("\r\n");




//    //application/json
//    print_(F("Action=SendMessage\r\n"));
//    Serial.print("Action=SendMessage\r\n");
//    print_(F("&MessageBody={\"fields\": [{\"temperature_c\": \""));
//    Serial.print("&MessageBody={\"fields\": [{\"temperature_c\": \"");
//    print_(t);
//    Serial.print(t);
//    print_(F("\", \"relative_humidity\": \""));
//    Serial.print("\", \"relative_humidity\": \"");
//    print_(h);
//    Serial.print(h);
//
//    print_(F("\"}]}"));
//    Serial.print("\"}]}");
//    // final newline to terminate the POST
//    print_(F("\r\n"));
//    Serial.print("\r\n");
