#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)
#define CHAT_ID "XXXXXXXXX"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int RelayPin1 = 2;
bool BulbState1 = HIGH;
const int RelayPin2 = 4;
bool BulbState2 = HIGH;

// Handle what happens when you receive new messages
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
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/on1 to turn BULB1 ON \n";
      welcome += "/off1 to turn BULB1 OFF \n";
      welcome += "/on2 to turn BULB2 ON \n";
      welcome += "/off2 to turn BULB2 OFF \n";
      welcome += "/state1 to request current BULB1  state \n";
      welcome += "/state2 to request current BULB2 state \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/on1") {
      bot.sendMessage(chat_id, "BULB1 state set to ON", "");
      BulbState1 = LOW;
      digitalWrite(RelayPin1, BulbState1);
    }
    
    if (text == "/off1") {
      bot.sendMessage(chat_id, "BULB1 state set to OFF", "");
      BulbState1 = HIGH;
      digitalWrite(RelayPin1, BulbState1);
    }
    
    if (text == "/state1") {
      if (digitalRead(RelayPin1)){
        bot.sendMessage(chat_id, "BULB1 is OFF", "");
      }
      else{
        bot.sendMessage(chat_id, "BULB1 is ON", "");
      }
    }
    if (text == "/on2") {
      bot.sendMessage(chat_id, "BULB2 state set to ON", "");
      BulbState2 = LOW;
      digitalWrite(RelayPin2, BulbState2);
    }
    
    if (text == "/off2") {
      bot.sendMessage(chat_id, "BULB2 state set to OFF", "");
      BulbState2 = HIGH;
      digitalWrite(RelayPin2, BulbState2);
    }
    
    if (text == "/state2") {
      if (digitalRead(RelayPin2)){
        bot.sendMessage(chat_id, "BULB2 is OFF", "");
      }
      else{
        bot.sendMessage(chat_id, "BULB2 is ON", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  digitalWrite(RelayPin1, BulbState1);

  pinMode(RelayPin2, OUTPUT);
  digitalWrite(RelayPin2, BulbState2);
  
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
}
