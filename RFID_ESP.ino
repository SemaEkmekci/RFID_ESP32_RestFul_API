#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <SPI.h>  // harici modül ile iletişim kurabilmek için veri gönderme veri alma işlemleri vs.
#include <MFRC522.h>  //  RFID-RC522 modülüyle iletişim kurmayı sağlar

#define SS_PIN  5  
#define RST_PIN 4

// #define WiFi_SSID "ekmekci"
// #define WiFi_PASSWORD "1234567890"
// wifi connect ----------------------------------------------------

const char* ssid = "ekmekci";
const char* password = "1234567890";

// send data json ---------------------------------------------------

// unsigned long lm;
// unsigned long delayTime;
String JsonData = "";
String url = "http://192.168.1.35:3000/api/postData";


// RFID control ---------------------------------------------------

int readsuccess;
char str[32] = "";  //id bilgisini tutmak için dizi
String UID_Result = ""; //id bilgisini tutmak için string


MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522.h kütüphanesinden bir nesne oluşturuyor.

// read RFID
int readRFID() {  
  if(!mfrc522.PICC_IsNewCardPresent()) { //Yeni bir kart mı?
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) { //id okundu mu?
    return 0;
  }
  
  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;
  
  mfrc522.PICC_HaltA(); //kartın çalışmasını durdur
  mfrc522.PCD_StopCrypto1(); //kriptololamayı durdur.
  
  return 1;
}

void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {  //byte olarak okunan kart id'sini string bir değere döndürüyor.
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F; // İlk yarıbayt
    byte nib2 = (array[i] >> 0) & 0x0F;  // İkinci yarıbayt
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA; // İlk yarıbaytı ASCII'ye dönüştür
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;  // İkinci yarıbaytı ASCII'ye dönüştür
  }
  buffer[len*2] = '\0'; // Stringi sonlandır
}
//________________________________________________________________________________


// wifi connect ----------------------------------------------------
void connectToWiFi(){

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the Wifi network");
}

void sendDataToServer(const char* url, const char* contentType) {

  StaticJsonDocument<50> doc;
  doc["UID"] = UID_Result;
  serializeJson(doc, JsonData);
  Serial.print(JsonData);
  Serial.println();

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", contentType);
  int httpresponseCode = http.POST(JsonData);
  if (httpresponseCode > 0) {
  Serial.println(httpresponseCode);
  String payload = http.getString();
  Serial.println(payload);
  } else {
  Serial.print("Error on sending POST: ");
  Serial.println(httpresponseCode);
  }
  http.end();
}





//________________________________________________________________________________VOID SETUP()
void setup(){
  // put your setup code here, to run once:

  Serial.begin(115200); // Seri haberleşme başlatılıyor
  Serial.println();
  delay(1000);
  connectToWiFi();
// RFID control ---------------------------------------------------
  SPI.begin();       // SPI başlatılıyor 
  mfrc522.PCD_Init(); // MFRC522 modülü başlatılıyor
  delay(1000);
  Serial.println();
  Serial.println("KART OKUTUNUZ");
}

void loop(){

  if (WiFi.status() == WL_CONNECTED) {
      readsuccess = readRFID();
      if(readsuccess){
      Serial.println();
      Serial.print("UID : ");
      Serial.println(UID_Result);
      delay(500);
      
      sendDataToServer("http://192.168.114.1:3000/api/postData", "application/json");
      
      }else{
        Serial.print("Kart okuma başarısız.");
      }


  }else{
    Serial.print("İnternet bağlantısı yok.");
  }
  delay(1000);
  JsonData = "";
}





