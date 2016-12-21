#include <Wire.h>
#include <SPI.h>



#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <ESP8266WiFi.h>

//const char* ssid     = "iPhone";           // insert your SSID
//const char* password = "puccapipi";   // insert your password

const char* ssid     = "hi";           // insert your SSID
const char* password = "12345678";   // insert your password

//const char* ssid     = "IOTCourse"; 
//const char* password = ""; 

const int d0 = 16;
const int d1 = 5;
const int res = 4;
const int dc = 0;
const int cs = 2;


WiFiClient client;

const char* server = "api.openweathermap.org";  // server's address
const char* resource = "/data/2.5/weather?id=1668341&units=metric&APPID=7f9abc6839ab446bf8d5894bdae77685"; // insert your API key

char response[600]; // this fixed sized buffers works well for this project using the NodeMCU.

Adafruit_SSD1306 display(d1,d0,dc,res,cs);

void setup(){
    delay(3000);
    // initialize serial
    Serial.begin(115200);
    while(!Serial){
        delay(100);
        }
      
    // initialize WiFi
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());    
  display.begin(SSD1306_SWITCHCAPVCC,0x3D);
  display.clearDisplay();
 display.setTextWrap(false);
}



void loop(){
    
        // connect to server  
        bool ok = client.connect(server, 80);
        bool beginFound = false;
        
        while(!ok){
            //Serial.print(".");
            delay(500);        
        }
    
        delay(500);
    
        //Send request to resource
        client.print("GET ");
        client.print(resource);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    
        delay(100);
    
        //Reading stream and remove headers
        client.setTimeout(10000);
   
        bool ok_header = ok_header = client.find("\r\n\r\n");
    
        while(!ok_header){
        // wait
        }
         
        client.readBytes(response, 800);
        
        // uncomment line below to see data received for debugging purposes or just fun    
        // Serial.println(response); 
        
        // process JSON
        DynamicJsonBuffer jsonBuffer;
        
        // But.....make sure the stream header is valid
        // Sometime OWM includes invalid data after the header
        // Parsing fails if this data is not removed
        
        if(int(response[0]) != 123){
            Serial.println("Wrong start char detected");
        int i = 0;
        while(!beginFound){
            if(int(response[i]) == 123){ // check for the "{" 
            beginFound = true;
            Serial.print("{ found at ");
            Serial.println(i);
            }
            i++;
        }

            int eol = sizeof(response);
            Serial.print("Length = ");
            Serial.println(eol);
  
    
            //restructure by shifting the correct data
            Serial.println("restructure");
            for(int c=0; c<(eol-i); c++){
                response[c] = response[((c+i)-1)];
                Serial.print(response[c]);
            }
     
            Serial.println("Done...");

                
            }
        
        
        JsonObject& root = jsonBuffer.parseObject(response);
        
        if (!root.success()) {
        Serial.println("JSON parsing failed!");
        } 
        else {
        //Serial.println("JSON parsing worked!");
        }
        
        const char* location = root["name"]; 
        const char* weather = root["weather"][0]["description"];
        double temp = root["main"]["temp"];
        
        // Print data to Serial
        Serial.print("*** ");
        Serial.print(location);
        Serial.println(" ***");
        Serial.print("Type: ");
        Serial.println(weather);
        Serial.print("Temp ");
        Serial.print(temp);
        Serial.println("C");
        Serial.println("----------"); 
     
        client.stop(); // disconnect from server
        
        display.clearDisplay();
         display.setTextColor(WHITE);
    display.setTextSize(1);
        display.setCursor(0,0);
        display.print("*** ");
        display.print(location);
       display.print(" ***");
       display.setCursor(0,10);
        display.print("Type: ");
        display.print(weather);
        display.setCursor(0,20);
        display.print("Temp ");
        display.print(temp);
        display.print("C");
        display.display();
      
        for(int x=0; x<10; x++){ // wait for new connection with progress indicator
            Serial.print(".");
            delay(99999); // the OWM free plan API does NOT allow more then 60 calls per minute
        }
        
        Serial.println("");
     
    }
