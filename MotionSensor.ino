#include <WiFiClient.h> 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //version 5
#include <Servo.h>
 
//wifi settings
//please don't connect to and hijack my wifi if you ever come to my house
const char *ssid = ""; 
const char *password = "";
 
//flask server to post to
const char *host = "http://192.168.1.127/post";  

const int doormotion = 2;
const int frontmotion = 3;
const int servo = 5; 
Servo crawler;
int numpeople = 0;
bool dooropen = false;

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
  pinMode(doormotion, INPUT);
  crawler.attach(servo);
  //we can allow setup to take some time, like how we boot up routers, it's ok disha
  delay(1000);
  Serial.begin(115200);
  connectToWifi();
}

void sendDoorStatus(int val, String stat)
{
  if (WiFi.status() == WL_CONNECTED) 
  { 
    StaticJsonBuffer<300> jsonbuffer;
    JsonObject& sensorjson = jsonbuffer.createObject(); 

    //adding info to json
    sensorjson["id"] = 2;
    sensorjson["type"] = "motion";
    sensorjson["location"] = "door";
    sensorjson["value"] = val;
    sensorjson["status"] = stat; 

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
  delay(1000);  //Send a request every 1 seconds
}

void loop() 
{
  if (digitalRead(frontmotion) == HIGH && digitalRead(doormotion) == HIGH)
    sendDoorStatus(0, "closed");
  else if (digitalRead(frontmotion) == LOW && digitalRead(doormotion) == HIGH)
  {
    while(digitalRead(frontmotion) == LOW)
    {
      if(digitalRead(doormotion) == LOW)
      {
        sendDoorStatus(2, "entering");
        numpeople++;
        dooropen = true;
        break;
      }
    }
    if (dooropen == false)
    {
      sendDoorStatus(1, "someone_at_door");
    }
    else if (dooropen == true && numpeople == 1) //there is only one person in the room and the light needs to turn on
    {
      dooropen = false;
      crawler.write(120);
      delay(10000); //give someone ten seconds to close the door after they enter
    }
  }
  else if (digitalRead(frontmotion) == HIGH && digitalRead(doormotion) == LOW)
  {
    while (digitalRead(doormotion) == LOW)
    {
      if (digitalRead(frontmotion) == LOW)
      {
        sendDoorStatus(3, "leaving");
        numpeople--;
        dooropen = true;
        break;
      }
    }
    if (dooropen == false)
    {
      sendDoorStatus(4, "open_close");
    }
    else if (dooropen == true && numpeople == 0) //there is no one in the room and the light needs to turn off
    {
      dooropen = false;
      crawler.write(0);
      delay(10000); //give someone ten seconds to close the door after they leave
    }
  }
}
