#include <WiFiClient.h> 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //version 5
 
//wifi settings
//please don't connect to and hijack my wifi if you ever come to my house
const char *ssid = ""; 
const char *password = "";
 
//flask server to post to
const char *host = "http://192.168.1.127/post";  

//Temperature sensor
int ThermoPin = 0;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
String weather = "";

void connectToWifi()
{
  //turn off first
  WiFi.mode(WIFI_OFF);        
  delay(1000); 

  //connect to router
  WiFi.begin(ssid, password);     
  Serial.print("Connecting");
  
  while (WiFi.status() != WL_CONNECTED) 
    Serial.println("...");
    //if stuck in loop forever, there's an issue
  
  //connection successful 
  Serial.print("connected to ");
  Serial.println(ssid);
}
 
void setup() 
{
  pinMode(ThermoPin, INPUT);
  //we can allow setup to take some time, like how we boot up routers, it's ok disha
  delay(1000);
  Serial.begin(115200);
  connectToWifi();
}

void getTemp()
{
  Vo = analogRead(ThermoPin);
  
  //voltage divider 
  R2 = R1 * (1023.0 / (float)Vo - 1.0);

  //steinhart-Hart equation to find temp based on resistance 
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0;

  //interpretation of temp
  if (T >= 100)
      weather = "sweltering";
    else if (T >= 90)
      weather = "HOT";
    else if (T >= 80)
      weather = "Lukewarm";
    else if (T >= 70)
      weather = "warm";
    else if (T >= 60)
      weather = "chilly";
    else 
      weather = "COLD";
}

void loop() 
{
  getTemp();

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) 
  { 
    StaticJsonBuffer<300> jsonbuffer;
    JsonObject& sensorjson = jsonbuffer.createObject(); 

    //adding info to json
    sensorjson["id"] = 1;
    sensorjson["type"] = "temperature";
    sensorjson["location"] = "left_window";
    sensorjson["value"] = T;
    sensorjson["status"] = weather; 

    //make it pretty!
    char jsonoutput[300];
    sensorjson.prettyPrintTo(jsonoutput, sizeof(jsonoutput));
    Serial.println(jsonoutput);

    //connect to flask server
    HTTPClient http;
    http.begin(host);
    http.addHeader("Content-Type", "application/json");
 
    int httpCode = http.POST(jsonoutput);
    String payload = http.getString(); //response from flask
    Serial.println(httpCode); //http return code
    Serial.println(payload); //request response payload
 
    http.end(); //close connection
  } 
  else 
  {
    Serial.println("error connecting to WiFi");
  }
 
  delay(3000);  //Send a request every 3 seconds
}
