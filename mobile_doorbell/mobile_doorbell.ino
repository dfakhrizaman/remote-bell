#include <WiFi.h>                
#include <HTTPClient.h>                                
#include <IOXhop_FirebaseESP32.h>
#include "time.h"
                                         
#define FIREBASE_Host "mobile-doorbell-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_authorization_key "FjuBvesaB2QYL6eOA1veTxdiGRgt07h0PdDlsOmf"
#define WIFI_SSID "password"
#define WIFI_PASSWORD "daffauchiha"

int buzzerPin = 2;

String isBellPressed = "FALSE";


String iftttUrl = "https://maker.ifttt.com/trigger/bell-rang/with/key/fqUcX7qCK_HPv1TmXRMj1ZZypBynHDUXrsH9oTYUv9o";

// Variables  for date & time
const char* ntpServer = "pool.ntp.org";
const int   daylightOffset_sec = 21600;
const long  gmtOffset_sec = 3600;


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

void sendMailcode( void * pvParameters ){
  Serial.println("Sending Email");
  delay(4000);
  Serial.println("Email sent");
  //TODO: Implement HTTP Request
  vTaskDelete(NULL);
}

void mainTaskCode( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){

    isBellPressed = Firebase.getString("isBellPressed");

    if (isBellPressed == "TRUE") {
      triggerOutput();
      Firebase.setString("isBellPressed", "FALSE");

      sendMailTrigger();
      
    }
  
    else if (isBellPressed == "FALSE") {             
      Serial.println("OFF");
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(buzzerPin, LOW);
    }
  
    else {
       Serial.println("Wrong value entered! Only ON or OFF accepted");
    }
    
  }

  vTaskDelete(NULL);
}

void loop() {}
