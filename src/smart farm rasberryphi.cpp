#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>

#define ORG "phr5l9"
#define DEVICE_TYPE "ESP32"
#define DEVICE_ID "hwangyj123"
#define TOKEN "GC3GwWjdBUk2Gqp3Qp"
#define AUthMethod "use-token-auth"
#define LED2_STATE "/LED2/"    
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif
 int i;
int pin         =  12; 
int numPixels   = 16; 
int pixelFormat = NEO_GRB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;
#define DELAYVAL 500 

int flag=0;
char mqttServer[] = ORG".messaging.internetofthings.ibmcloud.com";
char clientID[] ="d:" ORG":"DEVICE_TYPE":"DEVICE_ID;
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";
char subTopic[] = "iot-2/cmd/LED2/fmt/text";

const char* ssid = "Hwang_desk";
const char* password = "50875087";



void rxMsg(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient mqttClient(mqttServer, 1883, rxMsg, wifiClient);
HTU21D htSensor;

const uint8_t LED2 =2; 
uint32_t preTime;


void wifiConnect(){
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\r\nWiFi connected");
}

void mqttConnect(){
  if(!mqttClient.connected()){
    Serial.print("Reconnecting MQTT client to ");
    Serial.println(mqttServer);
    while (!mqttClient.connect(clientID, AUthMethod, TOKEN)){
      Serial.print(".");
      delay(500);
    }
    Serial.println("IBM Waston IoT Platform connected!");
  }
  Serial.print("subscribe to ");
  Serial.print(subTopic);
  if(mqttClient.subscribe(subTopic)) {
    Serial.println("Subscription OK");
  } else {
    Serial.println("Subscription Failed");
  }
}

void lightup(){
 
  if(i!=numPixels){
  pixels->clear(); 
    for( i=0; i<numPixels; i++) { 
    pixels->setPixelColor(i, pixels->Color(150, 150, 150));
    pixels->show();   
    delay(DELAYVAL);
    }
  }
 }

void lightdown(){
   pixels->clear();
   i=0;
   pixels->show();
   delay(DELAYVAL); 
    

}


void setup(){
  Serial.begin(115200);
  Serial.println("IBM Watson IoT platform Subscription Test program");
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);

Wire.begin(21, 22, 100000); // sda 21, sck:22
  htSensor.begin();

  wifiConnect();
  mqttConnect();

  preTime = 0;

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels = new Adafruit_NeoPixel(numPixels, pin, pixelFormat);
  pixels->begin(); 
}

void loop(){
  String payload = "";

  mqttClient.loop();

  if ((millis() - preTime) >= 3000) {

    float humid = htSensor.readHumidity();
    float temp = htSensor.readTemperature();

    if ( (humid < 900.0) && (temp < 900.0)) {
      Serial.printf(" Temperature: %.2f [C], Humidity: %.2f [%%]\r\n", temp, humid);

      preTime = millis();

      payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      payload += ",\"temperature\":";
      payload += String(temp);
      payload += "}}";

      if (mqttClient.publish(pubTopic1, payload.c_str())) {
        Serial.println("Publish ok!");
      } else {
        Serial.println("Publish failed!");
      }

      payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      payload += ",\"humidity\":";
      payload += String(humid);
      payload += "}}";

      if (mqttClient.publish(pubTopic2, payload.c_str())) {
        Serial.println("Publish ok!");
      } else {
        Serial.println("Publish failed!");
      }
    } else {
      Serial.println("Error: in reading temperature or humidity");
    }

    if(flag==0){
    if(temp<26) lightup();
    if(temp>30) lightdown();
    }
  }
  delay(1000);
  

}

void rxMsg(char* rtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("Receive amessage of a topic: ");
  Serial.println(rtopic);

  payload[payloadLength] = '\0';
  Serial.printf("Payload: %s\r\n", (char *)payload);

  if(String(rtopic).indexOf(LED2_STATE) >= 0){
    String cmd = String((char *)payload);

    if(cmd == "ON"){
      flag=0;
    } 
    else if(cmd == "OFF") {
      flag=1;
      i=0;
      pixels->clear();
      pixels->show();
      delay(DELAYVAL);  
    }
  } else {
    Serial.print("rxMSg: unexpected topic:");
    Serial.println(rtopic);
  }
}


char mqttServer[] = ORG".messaging.internetofthings.ibmcloud.com";
char clientID[] = "d:"ORG":"DEVICE_TYPE":"DEVICE_ID;
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";


const char* ssid = "Behappy";
const char* password = "1234567890";

WiFiClient wifiClient;
PubSubClient mqttClient(mqttServer, 1883, NULL, wifiClient);

HTU21D htSensor;
const uint8_t LED2 = 2;

uint32_t preTime;

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!!");
}

void mqttConnect() {
  if (!mqttClient.connected()) {
    Serial.print("Reconnecting MQTT client to ");
    Serial.println(mqttServer);
    while (!mqttClient.connect(clientID, AUthMethod, TOKEN)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("IBM Waston IoT Platform connected!!");
  }
}

void light(int num){

  if(num<25)
    ledcWrite (0,10);
    delay(15);
  

  if(num>25){
    ledcWrite (0,255);
  }
}


void setup()
{
  Serial.begin(115200);

  double realFreq = ledcSetup (0, 5000, 8);
  Serial.printf("Real PWM freq.: %.2f\r\n, realFreq");
  ledcAttachPin(LED2, 0);


  Serial.println("IBM Watson IoT Platform Test Program");
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);

  Wire.begin(21, 22, 100000); // sda 21, sck:22
  htSensor.begin();

  wifiConnect();
  mqttConnect();

  preTime = 0;

}







void loop() 
{
  String payload = "";

  mqttClient.loop();

  if ((millis() - preTime) >= 3000) {

    float humid = htSensor.readHumidity();
    float temp = htSensor.readTemperature();

    if ( (humid < 900.0) && (temp < 900.0)) {
      Serial.printf(" Temperature: %.2f [C], Humidity: %.2f [%%]\r\n", temp, humid);

      preTime = millis();

      payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      payload += ",\"temperature\":";
      payload += String(temp);
      payload += "}}";

      if (mqttClient.publish(pubTopic1, payload.c_str())) {
        Serial.println("Publish ok!");
      } else {
        Serial.println("Publish failed!");
      }

      payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      payload += ",\"humidity\":";
      payload += String(humid);
      payload += "}}";

      if (mqttClient.publish(pubTopic2, payload.c_str())) {
        Serial.println("Publish ok!");
      } else {
        Serial.println("Publish failed!");
      }
    } else {
      Serial.println("Error: in reading temperature or humidity");
    }
    light((int)temp);
  }
  delay(1000);
}




char mqttServer[] = ORG".messaging.internetofthings.ibmcloud.com";
char clientID[] ="d:" ORG":"DEVICE_TYPE":"DEVICE_ID;
//char pubtopic[] = "iot-2/evt/status1/fmt/json";
char subTopic[] = "iot-2/cmd/LED2/fmt/text";

const char* ssid = "Hwang_desk";
const char* password = "50875087";



void rxMsg(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient mqttClient(mqttServer, 1883, rxMsg, wifiClient);

const uint8_t LED2 =2; 
uint32_t preTime;

void wifiConnect(){
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\r\nWiFi connected");
}

void mqttConnect(){
  if(!mqttClient.connected()){
    Serial.print("Reconnecting MQTT client to ");
    Serial.println(mqttServer);
    while (!mqttClient.connect(clientID, AUthMethod, TOKEN)){
      Serial.print(".");
      delay(500);
    }
    Serial.println("IBM Waston IoT Platform connected!");
  }
  Serial.print("subscribe to ");
  Serial.print(subTopic);
  if(mqttClient.subscribe(subTopic)) {
    Serial.println("Subscription OK");
  } else {
    Serial.println("Subscription Failed");
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println("IBM Watson IoT platform Subscription Test program");
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);

  wifiConnect();
  mqttConnect();

  preTime = 0;
}

void loop(){
  mqttClient.loop();
  delay(1000);
}





void rxMsg(char* rtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("Receive amessage of a topic: ");
  Serial.println(rtopic);

  payload[payloadLength] = '\0';
  Serial.printf("Payload: %s\r\n", (char *)payload);

  if(String(rtopic).indexOf(LED2_STATE) >= 0){
    String cmd = String((char *)payload);

    if(cmd == "ON"){
      digitalWrite(LED2, HIGH);
    } else if(cmd == "OFF") {
      digitalWrite(LED2, LOW);
    }
  } else {
    Serial.print("rxMSg: unexpected topic:");
    Serial.println(rtopic);
  }
}
