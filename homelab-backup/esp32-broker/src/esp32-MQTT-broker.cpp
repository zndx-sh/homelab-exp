#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char* ssid = "GITAM";
const char* password = "Gitam$$123";

// Bonjour / mDNS hostname of your Mac
const char* mqtt_host = "lohithsrikar.local";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int buttonPin = D3;

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker: ");
    Serial.print(mqtt_host);
    Serial.print(":");
    Serial.println(mqtt_port);
    if (client.connect("NodeMCU-trigger")) {
      Serial.println("MQTT connected!");
    } else {
      Serial.print("MQTT connection failed, state: ");
      Serial.println(client.state());
      Serial.println("Retrying in 1 second...");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("=== NodeMCU MQTT Trigger ===");
  
  pinMode(buttonPin, INPUT_PULLUP);

  connectWiFi();

  // Start mDNS
  if (!MDNS.begin("nodemcu")) {
    Serial.println("mDNS failed to start!");
  } else {
    Serial.println("mDNS started: nodemcu.local");
  }

  // On ESP8266, use the hostname directly with setServer
  // The WiFiClient will resolve .local addresses
  client.setServer(mqtt_host, mqtt_port);
  Serial.println("Setup complete!");
}

void loop() {
  MDNS.update(); // Required for ESP8266 mDNS
  
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button pressed! Publishing: home/lab/backup -> backup-now");
    client.publish("home/lab/backup", "backup-now");
    delay(500); // debounce
  }
}

