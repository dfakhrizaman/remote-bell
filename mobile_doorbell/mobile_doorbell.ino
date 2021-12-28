#include <WiFi.h>                
#include <HTTPClient.h>                                
#include <IOXhop_FirebaseESP32.h>
#include "time.h"
#include "creds.h" // Firebase and IFTTTcredentials


// WiFi Credentials
#define WIFI_SSID "password"
#define WIFI_PASSWORD "daffauchiha"

int buzzerPin = 2;
String isBellPressed = "FALSE";

// Variables  for date & time
const char* ntpServer = "pool.ntp.org";
const int   daylightOffset_sec = 21600;
const long  gmtOffset_sec = 3600;

// Task initialization
TaskHandle_t mainTask;
TaskHandle_t sendMail;


// Returns Date
String printLocalDate()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain date");
    return "Failed to obtain Date";
  }

  char timeStringBuff[28];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%B %d", &timeinfo);
  
  return timeStringBuff;  
}


// Returns Hour:Minute
String printLocalHour()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Failed to obtain time";
  }

  char timeStringBuff[28];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
  
  return timeStringBuff;  
}

// Trigger buzzer and built-in LED
void triggerOutput(){
  Serial.println("Ringing Started");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(300);
  }

  Serial.println("Ringing Stopped");
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
  }

  Serial.println();
  Serial.print("Connected to WIFI");
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Establish firebase connection
  Firebase.begin(FIREBASE_Host, FIREBASE_authorization_key);
  Firebase.setString("isBellPressed", "FALSE");


  // Get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  String date = {printLocalDate()};
  Serial.println (date);
  
  String hour = {printLocalHour()};
  Serial.println (hour);

  // Run main task
  xTaskCreatePinnedToCore(
                    mainTaskCode,   /* Task function. */
                    "mainTask",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &mainTask,      /* Task handle to keep track of created task */
                    0            /* pin task to core 0 */
  );

  delay(500);
}

// Controller function for running sendMail Task
void sendMailTrigger(){
  xTaskCreatePinnedToCore(
                    sendMailcode,   /* Task function. */
                    "sendMail",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &sendMail,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
}

// Send mail
void sendMailcode( void * pvParameters ){
  Serial.println("Sending Email");

  //TODO: Implement HTTP Request
  HTTPClient http;
  
  http.begin(iftttUrl);
  int httpResponseCode = http.GET();
  
  Serial.println("Email sent");
  

  vTaskDelete(NULL);
}

// Main task, running for infinite time
void mainTaskCode( void * pvParameters ){
  Serial.println("Starting main task.");
  
  for(;;){

    isBellPressed = Firebase.getString("isBellPressed");

    if (isBellPressed == "TRUE") {
      triggerOutput();
      Firebase.setString("isBellPressed", "FALSE");

      sendMailTrigger();
      
    }
    
    else if (isBellPressed == "FALSE") {             
      Serial.println("OFF");
    }
  
    else {
       Serial.println("Wrong value entered! Only ON or OFF accepted");
    }
    
  }

  vTaskDelete(NULL);
}

void loop() {}
