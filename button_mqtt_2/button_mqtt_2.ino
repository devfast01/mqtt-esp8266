#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#else
#error "Board not found"
#endif
#include <PubSubClient.h>

#define RelayPin1 2  
#define pbutton   4
const char *ssid = ""; // Enter your WiFi name
const char *password = "";  // Enter WiFi password
const char* mqttServer = "192.168.1.120";
const char* mqttUserName = ""; // MQTT username
const char* mqttPwd = ""; // MQTT password
//const char* clientID = "EspClient00002"; // client id

#define sub1 "switch1"

#define pub1 "switch1_status"

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (80)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int pushed = 0;

void setup_wifi() {
 delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 if (client.connect("ESP32Client")) {
 Serial.println("MQTT connected");
      // ... and resubscribe
      client.subscribe(sub1);
} 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strstr(topic, sub1))
  {
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '0') {
      digitalWrite(RelayPin1, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      client.publish(pub1, "0");
    } else {
      digitalWrite(RelayPin1, LOW);  // Turn the LED off by making the voltage HIGH
      client.publish(pub1, "1");
    }    
  }
}

void button(){
int buttonState = digitalRead(pbutton); // read new state

  if (buttonState == LOW) {
    digitalWrite(RelayPin1, HIGH); // turn on
    client.publish(sub1, "HIGH");
    
  }
  else
  if (buttonState == HIGH) {
    digitalWrite(RelayPin1, LOW);  // turn off
    client.publish(sub1, "LOW");
    
  }
}


void setup() {
  Serial.begin(115200);

  pinMode(pbutton,INPUT);  
  pinMode(RelayPin1, OUTPUT);
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(pbutton,HIGH);
 
  setup_wifi();
   client.setServer(mqttServer, 1883);
   client.setCallback(callback);
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }

  client.loop();
  
  if (Serial.available()) {
Serial.write(Serial.read());

}
  button();
  delay(1000);
}
