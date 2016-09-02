#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <MicroGear.h>
SoftwareSerial swSer(14, 12);
/* Setting WIFI */
const char* ssid     = "ESPERT-3020";
const char* password = "espertap";

void test_t();

/* Setting NETPIE */
#define APPID                 "WebApp"
#define KEY                   "kuZyWfJrig1mb7E"
#define SECRET                "ZNMyde0RNtPR04SUz2kLS0woM"
char ALIAS[15]                = "14365118"; // Change your serial number device
char html_alias[15]           = "html_";

String netpie_incoming_text   = "\0";
int secure_incoming_text      = 0;

EspClass Esp;

WiFiClient client;
int timer = 0, state_connect  = 0, state_change_alias = 0;
MicroGear microgear(client);
/////////////////////
int start_new = 0, mod1_t_on = 4, mod1_t_off = 4, mod2_t_f = 20, mod2_di = 10, mod3_t_LED = 4, t_ref = 26;
int cmdistance, h, t, LED, mod, year1, month1, day1, hour1, minute1;
String json1;
int x = 0;
/////////////////
/* Setting DHT22 */


/* Setting JSON */
StaticJsonBuffer<200> jsonExportBuffer;
JsonObject& json_export       = jsonExportBuffer.createObject();

StaticJsonBuffer<200> jsonIncomingBuffer;
JsonObject& json_incoming     = jsonIncomingBuffer.createObject();

/* Function NETPIE */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  netpie_incoming_text = (char*)msg;
  Serial.println(netpie_incoming_text);
  JsonObject& json_incoming = jsonIncomingBuffer.parseObject(netpie_incoming_text);

  //  if (!json_incoming.success()) {
  //    Serial.println("parseObject() failed");
  //  } else {

  /* Compare value */
  if (json_incoming["set_temp"] != NULL) {
    t_ref = json_incoming["set_temp"];
    secure_incoming_text = 1; // state secure data
  }

  if (json_incoming["set_height"] != NULL) {
    mod2_di = json_incoming["set_height"];
    // insert your function

    secure_incoming_text = 1; // state secure data
  }

  if (json_incoming["set_light"] != NULL) {
    mod3_t_LED = json_incoming["set_light"];
    // insert your function

    secure_incoming_text = 1; // state secure data
  }

  if (json_incoming["set_on_water"] != NULL) {
    mod1_t_on = json_incoming["set_on_water"];
    // insert your function

    secure_incoming_text = 1; // state secure data
  }

  if (json_incoming["set_off_water"] != NULL) {
    mod1_t_on = json_incoming["set_off_water"];
    // insert your function

    secure_incoming_text = 1; // state secure data
  }
  if (json_incoming["set_water"] != NULL) {
    mod2_t_f = json_incoming["set_water"];
    // insert your function

    secure_incoming_text = 1; // state secure data
  }

  if (secure_incoming_text == 1) {

    Serial.println(netpie_incoming_text);
    microgear.chat(html_alias, "receiver success");
    test_t();
    secure_incoming_text = 0;
  }

  jsonIncomingBuffer = StaticJsonBuffer<200>(); // Clear jsonBuffer
  //Serial.println(Esp.getFreeHeap());

  //}

}
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void setup() {
  pinMode(2, INPUT);
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(9600);
  swSer.begin(9600);
  swSer.setTimeout(100);
  Serial.println("Starting...");
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());

  microgear.init(KEY, SECRET, ALIAS);
  microgear.connect(APPID);

  if (state_change_alias == 0) {
    strcat(html_alias, ALIAS);
    state_change_alias = 1;
  }

}

void loop() {

  if (microgear.connected()) {
    if (state_connect == 0) {
      Serial.println("Microgear Connected");
      state_connect = 1;
    }
    microgear.loop();

    if (timer >= 1000) {


      //      if (isnan(humid) || isnan(temp)) {
      //        Serial.println("Failed to read from DHT sensor!");
      //      } else {
      //        json_export["Temperature"] = temp;
      //        json_export["Humidity"] = humid;
      //        Serial.println();
      //        char buffer[256];
      //        json_export.printTo(buffer, sizeof(buffer));
      //        Serial.print("Publish : ");
      //        Serial.println(buffer);

      microgear.chat(html_alias, json1);

      //}
      timer = 0;
    } else {
      if (digitalRead(2) == 0) {
        start_new = 1;
        test_t();
        delay(100);
        start_new = 0;
        test_t();
      }
      if (swSer.available()) {
        json1 = swSer.readString();
        x = 1;
      }
      if (x == 1) {
        StaticJsonBuffer<500> jsonBuffer2;
        JsonObject& root = jsonBuffer2.parseObject(json1);
        cmdistance = root["cmdistance"];
        h = root["Humidity"];
        t = root["Temperature"];
        LED = root["LED"];
        mod = root["mod"];
        year1 = root["year"];
        month1 = root["month"];
        day1 = root["day"];
        hour1 = root["hour"];
        minute1 = root["minute"];
        Serial.print(cmdistance);
        Serial.print(" ");
        Serial.print(h);
        Serial.print(" ");
        Serial.print(t);
        Serial.print(" ");
        Serial.print(LED);
        Serial.print(" ");
        Serial.print(mod);
        Serial.print(" ");
        Serial.print(year1);
        Serial.print(" ");
        Serial.print(month1);
        Serial.print(" ");
        Serial.print(day1);
        Serial.print(" ");
        Serial.print(hour1);
        Serial.print(" ");
        Serial.print(minute1);
        Serial.println("");
        x = 0;
      }
      timer += 1;
    }
  } else {
    Serial.println("connection lost, reconnect...");
    if (timer >= 5000) {
      microgear.connect(APPID);
      timer = 0;
      state_connect = 0;
    } else {
      timer += 1;
    }
  }
  delay(1);

}
void test_t() {
  StaticJsonBuffer<200> jsonBuffer3;
  JsonObject& root = jsonBuffer3.createObject();
  JsonArray& data = root.createNestedArray("data");
  data.add(start_new);
  data.add(mod1_t_on);
  data.add(mod1_t_off);
  data.add(mod2_t_f);
  data.add(mod2_di);
  data.add(mod3_t_LED);
  data.add(t_ref);
  root.printTo(Serial);
  root.printTo(swSer);
  swSer.println();
  Serial.println();
  start_new = 0;
}
