#include <FastLED.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
//#include <string>

#define NUM_LEDS 55

std::string ssid = "electrolab";
std::string password = "plopplop";
std::string serverName = "http://172.16.14.30:8088/tally/?key=f5ca0bcb-d03d-4179-8327-3623fbc980a8";
CRGBArray<NUM_LEDS> leds;

void changeLEDcolor(std::string str){

// Update LED Color
  char color[8];
  strcpy(color, str.c_str());
  long ledcolor = (long) strtol(&color[1],NULL,16);
  for(int i = 0; i < 5; i++) { 
  leds[i] = ledcolor;
  FastLED.show();
  }
  
}

void boottingLED(){
  char color[8] = "fa0000" ;
  long ledcolor = (long) strtol(&color[1],NULL,16);
    
    for (int a = 0; a < 5; a++){
      for(int i = 0; i < 5; i++) { 
      
      leds[i] = ledcolor;
      FastLED.show();
      delay(50);
      leds[i] = CRGB::Black;
      FastLED.show();
      delay(50);
      }
    }

  
}

std::string extractColor(std::string x){

  x.erase(0,14);
  x.erase(6,3);
  return x;
}

std::string checkTallyColor(){
  HTTPClient http;
  
  std::string serverPath = serverName;
  String payload = "";
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  int httpResponseCode2 = http.GET();
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  //cConverting payload
  char buf[40] = "" ;
  payload.toCharArray(buf, 40);
  std::string Payload(buf);

  char color2[20];
  strcpy(color2, Payload.c_str());
  Serial.print(color2);
  
  return Payload;
}


void wifiSetup() {

  changeLEDcolor("fa0000");
  delay(500);
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    changeLEDcolor("fab300");
    delay(250);
    Serial.print(".");
    changeLEDcolor("000000");
    delay(250);


  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 
  for(int i = 0; i < 5; i++) {
    changeLEDcolor("fab300");
    delay(20);
    Serial.print(".");
    changeLEDcolor("000000");
    delay(20);
  }
}


void setup() { 
  FastLED.addLeds<WS2812B,14,GRB>(leds, NUM_LEDS);
  boottingLED();
  wifiSetup();
   
  }

void loop(){ 
  std::string Data = checkTallyColor();
  std::string color = extractColor(Data);
  //Serial.print(color2);
  //changeLEDcolor(color);
  //Serial.print (color);
  delay(500); 
}

