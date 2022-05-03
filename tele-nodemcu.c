#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
const char* ssid = "iot";
const char* password = "qwertyuiop!";

#define BOTtoken "xxxxxx"  //token

#define CHAT_ID "###" // telegram user id 

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;  // D0 FOR NODEMCU / 2 FOR ESP32
bool ledState = LOW;

const int pir = 16;
const int flame = 5;
const int rdsw = 4;

const int siren = 0;  
const int load1 = 14;
const int load2 = 12;
const int load3 = 13;

int pv,fv,rv;
int pvL,fvL,rvL;


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
  
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/led_on to turn Led ON \n";
      welcome += "/led_off to turn Led OFF \n";
      welcome += "/siren_on to turn Alarm ON \n";
      welcome += "/siren_off to turn Alarm OFF \n";
      welcome += "/load1_on to turn Load-1 ON \n";
      welcome += "/load1_off to turn Load-1 OFF \n";
      welcome += "/load2_on to turn Load-2 ON \n";
      welcome += "/load2_off to turn Load-2 OFF \n";
       welcome += "/load3_on to turn Load-3 ON \n";
      welcome += "/load3_off to turn Load-3 OFF \n";
      welcome += "/state to request current states \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    
    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = LOW; 
      digitalWrite(ledPin, ledState);
    }
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
     if (text == "/siren_on") {
      bot.sendMessage(chat_id, "Alarm state set to ON", "");
    
      digitalWrite(siren, HIGH);
    }
    if (text == "/siren_off") {
      bot.sendMessage(chat_id, "Alarm state set to OFF", "");
    
       digitalWrite(siren, LOW);
    }

    if (text == "/load1_on") {
      bot.sendMessage(chat_id, "Load - 1 state set to ON", "");
      
      digitalWrite(load1, HIGH);
    }
    if (text == "/load1_off") {
      bot.sendMessage(chat_id, "Load - 1 state set to OFF", "");
    
      digitalWrite(load1, LOW);
    }

    if (text == "/load2_on") {
      bot.sendMessage(chat_id, "Load - 2 state set to ON", "");
     
      digitalWrite(load2, HIGH);
    }
    if (text == "/load2_off") {
      bot.sendMessage(chat_id, "Load - 2 state set to OFF", "");
     
       digitalWrite(load2, LOW);
    }

     if (text == "/load3_on") {
      bot.sendMessage(chat_id, "Load - 3 state set to ON", "");
     
      digitalWrite(load3, HIGH);
    }
    if (text == "/load3_off") {
      bot.sendMessage(chat_id, "Load - 3 state set to OFF", "");
     
       digitalWrite(load3, LOW);
    }
    
    if (text == "/state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF", "");
      }

       if (digitalRead(siren)){
        bot.sendMessage(chat_id, "Alarm is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "Alarm is OFF", "");
      }

       if (digitalRead(load1)){
        bot.sendMessage(chat_id, "LOAD - 1 is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LOAD - 1 is OFF", "");
      }

       if (digitalRead(load2)){
        bot.sendMessage(chat_id, "LOAD - 2 is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LOAD - 2 is OFF", "");
      }

       if (digitalRead(load3)){
        bot.sendMessage(chat_id, "LOAD - 3 is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LOAD - 3 is OFF", "");
      }
    }
}
}
void setup() {
  Serial.begin(115200);
 
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  
  pinMode(pir, INPUT);
   pinMode(flame, INPUT);
      pinMode(rdsw, INPUT_PULLUP);
      
  pinMode(siren, OUTPUT);
   pinMode(load1, OUTPUT);
     pinMode(load2, OUTPUT);
        pinMode(load3, OUTPUT);
   digitalWrite(siren, LOW);
   digitalWrite(load1, LOW);
     digitalWrite(load2, LOW);
          digitalWrite(load3, LOW);
     // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  
}
void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }


  pv = digitalRead(pir);
  fv = digitalRead(flame);
  rv = digitalRead(rdsw);


  if( pv == HIGH && pvL==0){pvL=1;   bot.sendMessage(CHAT_ID, "Intruder Detected", "");}
  else if ( pv == LOW && pvL==1){ pvL=0; }
   else;

  
  if( fv == LOW && fvL==0){fvL=1;   bot.sendMessage(CHAT_ID, "Fire Detected", "");}
  else if ( fv == HIGH && fvL==1){ fvL=0; }
   else;

   
  if( rv == HIGH && rvL==0){rvL=1;   bot.sendMessage(CHAT_ID, "Door Breaking Detected", "");}
  else if ( rv == LOW && rvL==1){ rvL=0; }
   else;
   
   
}
