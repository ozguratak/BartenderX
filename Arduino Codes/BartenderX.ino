#include <ESP8266WiFi.h>
#include <Wire.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Wifi bilgileri
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

//OLED Ekran bilgileri
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Firebase bilgileri
#define FIREBASE_HOST "YOUR_FIREBASE_HOST"
#define FIREBASE_AUTH "YOUR_FIREBASE_AUTH"

FirebaseData firebaseDataBase;

// Pin tanımlamaları
const int valve1Pin = 5;
const int valve2Pin = 4;
const int valve3Pin = 0;
const int valve4Pin = 2;
const int valve5Pin = 14;
const int valve6Pin = 12;
const int valve7Pin = 13;
const int valve8Pin = 15;
const int mixerPin = 16;
const int emptyingValvePin = 3;
const int floatSensorPin = A0;

int valveCounter = 1;
int valveArray[8] = {valve1Pin, valve2Pin, valve3Pin, valve4Pin, valve5Pin, valve6Pin, valve7Pin, valve8Pin};
String dbValues[8] = {"/recipe/valve1", "/recipe/valve2", "/recipe/valve3", "/recipe/valve4", "/recipe/valve5", "/recipe/valve6", "/recipe/valve7", "/recipe/valve8"};
int mixerTime = 0;
bool mixerOn = false;

void setup() {
  Serial.begin(9600);

  //OLED Ekran başlatma
  Wire.begin(5, 4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // Wifi bağlantısı
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    showText("Connecting to Wi-Fi...");
    Serial.println("Connecting to WiFi...");
  }
  showText("Connected!");
  Serial.println("Wi-Fi connection complete!");
  Serial.println(WiFi.localIP());

  // Firebase bağlantısı
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  // Pin modları
  pinMode(emptyingValvePin, OUTPUT);
  digitalWrite(emptyingValvePin, LOW);
  for(int i=0; i<8; i++) {
    pinMode(valveArray[i], OUTPUT);
    digitalWrite(valveArray[i], LOW);
  }
  pinMode(mixerPin, OUTPUT);
  digitalWrite(mixerPin, LOW);

  Serial.print("System Ready!");
  showText("System Ready!");
}

void loop() {
    
  if(Firebase.getInt(firebaseDataBase, dbValues[valveCounter]) != 0) { // JSON verileri okuma
    int valveValue = Firebase.getInt(firebaseDataBase, dbValues[valveCounter]);
    openValve(valveArray[valveValue], valveValue); // valf açma işlemi
    valveCounter++;
  }
  else {
    valveCounter++;
  }
  
  if(valveCounter > 8 && !mixerOn) { // valf işlemleri tamamlandıktan sonra mixer için işlemlere geçiş
    mixerTime = Firebase.getInt(firebaseDataBase, "/recipe/mixerTime");
    if(mixerTime > 0) {
      mixerOn = true;
      startMixer(mixerTime);
      Serial.println("Mixing, please wait...");
      showText("Mixing, please wait...");
    }
    else {
      Serial.println("Preparing...");
      showText("Preparing...");
    }
  }

  if(!mixerOn && mixerTime == 0) { // mixer işlemi tamamlandıktan sonra boşaltma valfi işlemleri
    emptyingValve();
   
  }
}

void openValve(int valvePin, int valveValue) {
  digitalWrite(valvePin, HIGH);
  while(analogRead(floatSensorPin) != valveValue) { // şamandıra kontrolü
    delay(100);
  }
  digitalWrite(valvePin, LOW);
  valveCounter++;
}

void startMixer(int mixerTime) {
  digitalWrite(mixerPin, HIGH);
  delay(mixerTime * 1000);
  digitalWrite(mixerPin, LOW);
  mixerOn = false;
}

void emptyingValve() {
  digitalWrite(emptyingValvePin, HIGH); // boşaltma valfini aç
  while(analogRead(floatSensorPin) != 0) { // tank boşaltılana kadar bekleyin
    Serial.println(floatSensorPin);
    Serial.println(" Serving...");
    showText("Serving...");
    delay(100);
  }

  digitalWrite(emptyingValvePin, LOW); // boşaltma valfini kapat
  Serial.println("Served, Please take your glass.");
  showText("Ready! Please take your glass.");
}

void showText(String text) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x, &y, &w, &h); // metnin boyutlarını hesapla
  int16_t x_pos = (SCREEN_WIDTH - w) / 2; // metni yatayda ortalama için gereken pozisyonu hesapla
  int16_t y_pos = (SCREEN_HEIGHT - h) / 2; // metni dikeyde ortalama için gereken pozisyonu hesapla
  display.setCursor(x_pos, y_pos);
  display.print(text);
  display.display();
  delay(10000);
}





