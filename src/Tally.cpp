#include <FastLED.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
//#include <stdarg.h>
#define NUM_LEDS 5

std::string ssid = "AcidBox";
std::string password = "Gtk980Dfr";
std::string serverName = "http://192.168.1.181:8088/tallyupdate/?key=f5ca0bcb-d03d-4179-8327-3623fbc980a8";
std::string hostname = "TallyLight01";
CRGBArray<NUM_LEDS> leds;
WiFiServer server(80);
char hexColor[8];
char formu[100];

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void changeLEDcolor(long ledcolor){

  // Update LED Color
  for(int i = 0; i < 5; i++) { 
  leds[i] = ledcolor;
  FastLED.show();
  }
  
}

void boottingLED(){
  long ledcolor = 0xfa0000;
    
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

void serveWebPage(WiFiClient client){    
  
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            }
            //HTML HEADER
            char headHtml[400] PROGMEM = R"rawliteral(
              <!DOCTYPE html><html>
                <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
                <link rel=\"icon\" href=\"data:,\">
                <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
              )rawliteral" ; 
            client.println(headHtml);          
              char colorCss[100] = ".button { background-color: #";
              strcat(colorCss,hexColor);
              strcat(colorCss,"; border: none; color: white; padding: 16px 40px; </style></head>");
            client.println(colorCss);
  
            // HTML CONTENTE
            char formulair[700] PROGMEM = R"rawliteral(
              <body>
                <h1>OpenTally Web Config</h1>
                  <form action="/get">
                    SSID: <input type="text" name="SSID"><br>
                    PASSWORD: <input type="text" name="PASSWORD"><br>
                    <select id="cars" name="Camera">
                      <option value="Cam1">Cam1</option>
                      <option value="Cam2">Cam2</option>
                      <option value="Cam3">Cam3</option>
                      <option value="Cam4">Cam4</option>
                    </select>
                    <input type="submit" value="Submit">
                  </form><br>
            )rawliteral";

            client.println(formulair);
            
            // Display current state, and ON/OFF buttons for GPIO 26
            char colorHtml[50] = "<p>Color in use #";
            strcat(colorHtml,hexColor);
            strcat(colorHtml,"</p>");
            client.println(colorHtml);  
            // If the output26State is off, it displays the ON button       
            client.println("<p><button class=\"button\"></button></p>");
            //client.println("<p><a href=\"/26/on\"><button class=\"button\"></button></a></p>");

            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");                       // If a new client connects,
  
}

long checkTallyColor(){
  HTTPClient http;
  
  std::string serverPath = serverName;
  String payload;
  char color[8];
  long ledcolor = 0 ;
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) {
    payload = http.getString();
    payload.remove(0,14);  // Remove useless character , conserve only HEX Color code
    payload.remove(6,3);
    payload.c_str();
    payload.toCharArray(color, 8); //Converting payload from shit arduino String to long HEX Color code 
    strcpy (hexColor, color);
    ledcolor = (long) strtol(&color[1],NULL,16);
     }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return ledcolor ;
}


void wifiSetup() {

  changeLEDcolor(0xfa0000);
  delay(500);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Hostname : ");
  Serial.println(hostname.c_str());
  WiFi.setHostname(hostname.c_str()); //define hostname
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    changeLEDcolor(0xfab300);
    delay(250);
    Serial.print(".");
    changeLEDcolor(0x000000);
    delay(250);
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); 
  for(int i = 0; i < 5; i++) {
    changeLEDcolor(0xfab300);
    delay(20);
    Serial.print(".");
    changeLEDcolor(0x000000);
    delay(20);
  }
  Serial.println();
}


void setup() { 
  FastLED.addLeds<WS2812B,14,GRB>(leds, NUM_LEDS);
  gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
  boottingLED();
  wifiSetup();
  server.begin();
  
   
  }

void loop(){ 
  long color = checkTallyColor();
  Serial.print(formu);
  changeLEDcolor(color);
  WiFiClient client = server.available();
  if (client) {
  serveWebPage(client);
  
  }

  delay(500); 
}

