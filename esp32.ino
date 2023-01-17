#include<FirebaseESP32.h>
#include <WiFi.h>  
#include "DHT.h"
#define DHTPIN 22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


const char* SSID = "Wokwi-GUEST";
const char* PASS = "";

#define relay 5

#define FIREBASE_HOST "https://applied-well-358510-default-rtdb.firebaseio.com/"
              
#define FIREBASE_AUTH "c1jAH0jxJKZWVKTDBpuEqcIolSVcvYYDVMUiEBx2"

// mendeklarasikan objek data dari FirebaseESP8266
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  Serial.println();
  dht.begin();
  pinMode(relay, OUTPUT);
  WiFi.begin(SSID, PASS);                                
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   
  Firebase.reconnectWiFi(true);
  delay(1000);
  pinMode(22, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI TERPUTUS MENCOBA TERHUUNG KEMBALI...");
      WiFi.begin(SSID, PASS);
  }
  pompa();
    float t = dht.readTemperature();
    float h = dht.readHumidity();  
    if(isnan(t) || isnan(h)){
        Serial.println("Failed to read DHT11");
        }else{
          Serial.print("Humidity: ");
          Serial.print(h);
          Serial.print(" %\t");
          Serial.print("Temperature: ");
          Serial.print(t);
          Serial.println(" *C");
          delay(3000);
          sendDataToFirebase(h);
          sendDataTemperature(t);
        }
}

void sendDataToFirebase(int value) {
  Firebase.setInt(firebaseData,"/Hasil/Kelembapan", value); // kirim data ke path "/sensor/value" di Firebase
}
void sendDataTemperature(int value) {
  Firebase.setInt(firebaseData,"/Hasil/Temperature", value); // kirim data ke path "/sensor/value" di Firebase
}
void sendDataToFirebasee(String value) {
  Firebase.setString(firebaseData,"/Hasil/Status", value); // kirim data ke path "/sensor/value" di Firebase
}
void sendDataToFirebaseee(int value) {
  Firebase.setString(firebaseData,"/Hasil/Button", value); // kirim data ke path "/sensor/value" di Firebase
}

String val ;
void pompa(){
    String on = "ON";
    String off = "OFF";
  if (Firebase.getString(firebaseData, "Hasil/Button")) {                           
    Serial.println(firebaseData.dataType());
      val = firebaseData.stringData();
      Serial.println(val);
      if(val=="1")                                             
     {
       sendDataToFirebasee(on);
        digitalWrite(relay, HIGH);
        delay(10);
        Serial.println("POMPA ON");
    }
    if (val=="2")                                   
    {         
       sendDataToFirebasee(off);
      //manual tutup pint                             
      digitalWrite(relay, LOW);
      delay(10);
      Serial.println("POMPA OFF");
      sendDataToFirebaseee(4);
      return;
    }
    if (val=="4"){
        float t = dht.readTemperature();
        float h = dht.readHumidity();  
        if(isnan(t) || isnan(h)){
          Serial.println("Failed to read DHT11");
        }else{
          Serial.print("Humidity: ");
          Serial.print(h);
          Serial.print(" %\t");
          Serial.print("Temperature: ");
          Serial.print(t);
          Serial.println(" *C");
          delay(3000);
          sendDataToFirebase(h);
        
          if (h <= 30){
            digitalWrite(relay, HIGH);
            Serial.println("POMPA MENYALA");
            sendDataToFirebasee(on);
            return;
          }
          else{
            digitalWrite(relay, LOW);
            Serial.println("POMPA MATI");
            sendDataToFirebasee(off);
            return;
          }
        }
     }
  }
   else {
    Serial.println(firebaseData.errorReason());
  }
}
