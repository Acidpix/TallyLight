#include <FastLED.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define NUM_LEDS 60

String ssid = "AcidBox";
String password = "Gtk980Dfr";
String serverName = "http://192.168.1.82:8088/tallyupdate/?key=d6694b30-ded0-4d93-aa6f-5b66fda14b0b";
CRGBArray<NUM_LEDS> leds;

void setup() { 
  wifiSetup();
  FastLED.addLeds<WS2812B,14,GRB>(leds, NUM_LEDS); 
  }

void loop(){ 
  String Data = checkTallyColor();
  String color = extractColor(Data);
  changeLEDcolor(color);
  Serial.println (color);
  delay(500); 
}


void changeLEDcolor(String x){

// Convert HEX To RGB
  long dec = (long) strtol(&x[1],NULL,16);
  long r = dec >>16;
  long g = dec >>8 & 0xFF; 
  long b = dec &0xFF;
  Serial.println (r);
  Serial.println (g);
  Serial.println (b);
  

// Update LED Color
  for(int i = 0; i < 15; i++) { 
  leds[i] = CRGB(r,g,b);
  FastLED.show();
  }
  
}


String extractColor(String x){

  x.remove(0,14);
  x.remove(6,3);

    
  return x;
}

String checkTallyColor(){
  HTTPClient http;
  
  String serverPath = serverName;
  String payload = "";
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}


void wifiSetup() {
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 
}
