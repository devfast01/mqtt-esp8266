#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int BUTTON_PIN = 4; //button is connected to GPIO pin D2
// Update these with values suitable for your network.

const char *ssid = ""; // Enter your WiFi name
const char *password = "";  // Enter WiFi password

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// MQTT Broker
const char *mqtt_broker = "mqtt.flespi.io";
const char *topic = "button";
const char *mqtt_username = "i8Fd55AtPJjByuspExB7AEnYCmbv9cchaIl3BJmfitGEOWRgjidHMpveBuxywAG";
const char *mqtt_password = "";
const int mqtt_port = 1883;

// with flesbi via internet
//const char* mqttServer   = "mqtt.flespi.io";
//const char* mqttUser     = "Ti8Fd55AtPJjByuspExB7AEnYCmbv9cchaIl3BJmfitGEOWRgjidHMpveBuxywAG";

//const char* mqtt_server = "192.168.1.120";

//const char* mqtt_server = "iot.eclipse.org"; // buda internetli server flesbi yaly


long lastMsg = 0;
char msg[50];

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  
} //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String client_id = "ESP8266Client-";
    client_id += String(WiFi.macAddress());
    Serial.println("Connecting to public emqx mqtt broker.....");
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_broker, 1883);
  mqttClient.setCallback(callback);
  pinMode(BUTTON_PIN,INPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  long now = millis();
  int status;
  //每隔0.5s发送一次数据
  if (now - lastMsg > 500) {
     lastMsg = now;
     status=digitalRead(BUTTON_PIN);
     String msg="Button status: ";
     if(status==HIGH )
     {
        msg= msg+ " Not Press";
       char message[58];
       msg.toCharArray(message,58);
       Serial.println(message);
       //publish sensor data to MQTT broker
      mqttClient.publish(topic, message);
       }
     else
     {
      msg= msg+ "Pressed";
       char message[58];
       msg.toCharArray(message,58);
       Serial.println(message);
       //publish sensor data to MQTT broker
      mqttClient.publish(topic, message); // topic = button
     }
    }
    delay(500);
     
}
