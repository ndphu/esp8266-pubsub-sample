#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Use GPIO #2 of ESP8266 V1
#define GPIO_OUT_PIN 2
// 15 seconds timeout for connection to AP
#define CONNECT_TIMEOUT 10 * 1000


#define WIFI_RECONNECT_INTERVAL 30 * 1000
int lastWiFiReconnect;


// Your SSID
const char* ssid = "Oooooops";
// Your PSK
const char* password = "hahahihihoho";

// Topic
char* inTopic = "esp8266_arduino_out_esp8266_again";
// IOT server
char* server = "iot.eclipse.org";

// This function is called for each message from broker
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  digitalWrite(GPIO_OUT_PIN, !digitalRead(GPIO_OUT_PIN));
}

//ESP8266WebServer configServer(80);


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  delay(50);
  //loadCredentials();
  pinMode(GPIO_OUT_PIN, OUTPUT);
  digitalWrite(GPIO_OUT_PIN, HIGH);

  Serial.println();
  //setupAP();
  connectToWiFi();
}

//void handleRoot() {
//  configServer.send(200, "text/html", "<h1>You are connected</h1>");
//}

//void handleConfig() {
//  StaticJsonBuffer<200> jsonBuffer;
//  JsonObject& configJson = jsonBuffer.parseObject(configServer.arg("plain"));
//  strcpy(ssid,configJson["ssid"]);
//  strcpy(password, configJson["psk"]);
//  char *debug = (char*)malloc(255 * sizeof(char));
//  sprintf(debug, "ssid = '%s' with psk = '%s'", ssid, password);
//  Serial.println(debug);
//  free(debug);
//}

//void setupAP() {
//  Serial.print("Configuring access point...");
//  /* You can remove the password parameter if you want the AP to be open. */
//  WiFi.softAP("ESP8266_Settings", "00001111");
//  IPAddress myIP = WiFi.softAPIP();
//  Serial.print("AP IP address: ");
//  Serial.println(myIP);
//  configServer.on("/", handleRoot);
//  configServer.on("/config", HTTP_POST, handleConfig);
//  configServer.begin();
//  Serial.println("HTTP server started");
//}


void connectToWiFi() {
  lastWiFiReconnect = millis();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  int start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if ((millis() - start) > CONNECT_TIMEOUT) {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(server, 1883);
    client.setCallback(callback);
  } else {
    Serial.println("Failed");
  }
}

void connectToBroker() {
  Serial.print("Attempting MQTT connection...");
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str())) {
    Serial.println("connected");
    client.subscribe(inTopic);
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println("");
  }
}

void loop() {
  //configServer.handleClient();
  if (WiFi.status() != WL_CONNECTED && ((millis() - lastWiFiReconnect) > WIFI_RECONNECT_INTERVAL)) {
    connectToWiFi();
  }
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    connectToBroker();
  }
  if (client.connected()) {
    client.loop();
  }
}

