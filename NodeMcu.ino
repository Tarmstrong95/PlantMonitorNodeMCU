#include <ESP8266WiFi.h>

//variables
int water_level = 0;
int photo_level = 0;

//Constants
int sleepTime = 1000;
const char* ssid = "TristonArm_2.4G";
const char* password = "Thanchanok1205";

WiFiClient client;

//LED pins
int ledRed = D0;
int ledYellow = D1;
int ledGreen = D2;

//power out pins
int photo_resistor_input = D8;
int photo_resister_pin = D4;

//input pins
int respin = A0;

void setup() {
  // set serial data rate
  Serial.begin(9600);
  while (!Serial) {}

  // set pinmodes so the boards knows where to direct power
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(photo_resistor_input, INPUT);
  pinMode(photo_resister_pin, OUTPUT);

  // Maker sure no power is going out on start
  digitalWrite(photo_resister_pin, LOW);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledRed, LOW);

  // Just printing the start
  Serial.println("Started");

  // WIFI logic
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  Serial.println();
  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void loop() {
  // read and modify the incoming data
    doWaterSensor();
    doPhotoSensor();
}



void doPhotoSensor() {
  digitalWrite(photo_resister_pin, HIGH);

  photo_level = analogRead(photo_resistor_input);

  Serial.println();
  char string[] = "sunlight_status: ";
  char* final_message = string;
  char snum[4];
  Serial.println(strcat(final_message, itoa(photo_level, snum, 10)));

  //Send message through client
  Serial.println("Sending client message");
  if (client.connect("192.168.1.103", 5825)) {
    Serial.println("Connected to TCP server");
    client.write(final_message);
  } else {
    Serial.println("Failed to connect to TCP server");
  }
  return;

  digitalWrite(photo_resister_pin, LOW);
}



void doWaterSensor() {
  water_level = analogRead(respin);
  water_level = water_level - 300;
  char *water_status = "";

  if (water_level <= 0) {
    water_status = "Empty";
    digitalWrite(ledRed, HIGH);
  } else {
    if (water_level <= 250) {
      water_status = "Low";
      digitalWrite(ledYellow, HIGH);
    }
    if (water_level > 250 && water_level <= 300) {
      water_status = "Medium";
      digitalWrite(ledYellow, HIGH);
      digitalWrite(ledGreen, HIGH);
    }
    if (water_level > 300) {
      water_status = "High";
      digitalWrite(ledGreen, HIGH);
    }
  }
  sendWaterStatus(water_status);
  sendWaterLevel(water_level);
  clearLEDS();
}






void clearLEDS() {
  delay(sleepTime); //To maintain light on
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen, LOW);
  delay(sleepTime); //To maintain light off
}






/** SEND WATER LEVEL BY CONNECTING TO THE TCP SERVER AT PORT 192.168.1.103 **/
void sendWaterLevel(int level) {
  Serial.println();
  char string[] = "water_level: ";
  char snum[3];
  char* final_message = string;
  Serial.println(strcat(final_message, itoa(level, snum, 10)));

  //Send message through client
  Serial.println("Sending client message");
  if (client.connect("192.168.1.103", 5825)) {
    Serial.println("Connected to TCP server");
    client.write(final_message);
  } else {
    Serial.println("Failed to connect to TCP server");
  }
  return;
}






/** SEND WATER STATUS BY CONNECTING TO THE TCP SERVER AT PORT 192.168.1.103 **/
void sendWaterStatus(char* message) {
  Serial.println();
  char string[] = "water_status: ";
  char* final_message = string;
  Serial.println(strcat(final_message, message));

  //Send message through client
  Serial.println("Sending client message");
  if (client.connect("192.168.1.103", 5825)) {
    Serial.println("Connected to TCP server");
    client.write(final_message);
  } else {
    Serial.println("Failed to connect to TCP server");
  }
  return;
}
