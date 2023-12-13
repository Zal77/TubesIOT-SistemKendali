#include <PubSubClient.h>
#include <AutoConnectCore.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ultrasonik.h"
#include "Dht.h"
#include "LDR.h"
#include "aktuator.h"

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig Config;

WiFiClient espClient;
PubSubClient client(espClient);

const char* mqtt_server = "broker.hivemq.com";
const char* topic = "TubesIOT/kelompok/1";
/*const char* topicjarak = "TubesIOT/kelompok/1/jarak";
const char* topictemp = "TubesIOT/kelompok/1/dht/temperatur";
const char* topichumi = "TubesIOT/kelompok/1/dht/humidity";
const char* topicLDR = "TubesIOT/kelompok/1/LDR";*/

Ultrasonik SensorUltrasonik(4, 5);  // Menggunakan pin 4 sebagai TRIG dan pin 5 sebagai ECHO
dhtsensor SensorDHT(2); //Menggunakan pin 2
ldrsensor SensorLDR(18); //Menggunakan pin 18
aktuator Aktuator(33, 25, 26, 27); //Menggunakan pin 26(Servo) 27(Pompa) 28(LED) 29(Fan)

String dataDHT;
String datagabungan;

char datatemp[50];
char datahumi[50];
char dataLDR[50];
char datajarak[50];

void rootPage() {
  char content[] = "KAMI KELOMPOK SATU";
  Server.send(200, "kelompok1/IOT", content);
}

void callback(char* topic, byte* payload, unsigned int length) {
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "KELOMPOK-SATU";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void server_setup(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println();

  Server.on("/", rootPage);

  Config.apid = "KELOMPOK-SATU";
  Config.hostName = "KELOMPOK-SATU";
  Config.title = "KELOMPOK-SATU";
  Config.autoReconnect = true;
  Config.reconnectInterval = 6;  // Seek interval time is 180[s].
  Portal.config(Config);

  Portal.begin();
  Serial.println("Web Server started:" + WiFi.localIP().toString());
}

String getDataUltra(){
  float jarak = SensorUltrasonik.bacaJarak();
  sprintf(datajarak, "%.2f", jarak);
  Serial.print("Publish message jarak: ");
  Serial.print(datajarak);
  //client.publish(topicjarak, datajarak);
  delay(500);
  return datajarak;
}

String getDataDHT(){
  float temp = SensorDHT.bacasuhu();
  float humi = SensorDHT.bacakelembaban();
  if(isnan(humi) || isnan(temp)){
    Serial.print("Gagal membaca sensor");
  }
  sprintf(datatemp, "%.2f", temp);
  Serial.print("Cm\tPublish message temp: ");
  Serial.print(datatemp);
  sprintf(datahumi, "%.2f", humi);
  Serial.print("°C\tPublish message humi: ");
  Serial.print(datahumi);  
  //client.publish(topictemp, datatemp);
  //client.publish(topichumi, datahumi);
  dataDHT = String(datatemp) + "#" + String(datahumi);
  return dataDHT;
}

String getDataLDR(){
  int nilaiLDR = SensorLDR.bacaNilai();
  sprintf(dataLDR, "%.2f", nilaiLDR);
  Serial.print("%\tPublish message LDR: ");
  Serial.println(dataLDR);
  //client.publish(topicLDR, dataLDR);
  return dataLDR;
}

void mergeData(){
  datagabungan = getDataUltra() + "#" + getDataDHT() + "#" + getDataLDR();
  client.publish(topic, datagabungan.c_str());
  delay(1000);
}

void DataPompa(){
  String datajarak = getDataUltra();
  float jarak = atof(datajarak.c_str());
  if(jarak > 10){
    digitalWrite(Aktuator.PompaPin, LOW);
  }
  else if(jarak < 10){
    digitalWrite(Aktuator.PompaPin, HIGH);
  }
}
void Dataservo(){
  String datajarak = getDataUltra();
  float jarak = atof(datajarak.c_str());
  if(jarak > 10){
    Aktuator.ServoPos(90);
  }
}

void DataLed(){
  String dataLDR = getDataLDR();
  float LDR = atof(dataLDR.c_str());
  if(LDR < 500){
    digitalWrite(Aktuator.LedPin, HIGH);
  }
  else{
    digitalWrite(Aktuator.LedPin, LOW);
  }
}

void DataFan(){
  String data = getDataDHT();

  if (!data.isEmpty()) {
    // Pisahkan data suhu dan kelembaban
    int separatorIndex = data.indexOf("#");
    
    if (separatorIndex != -1) {
      String suhuString = data.substring(0, separatorIndex);

      // Ubah string ke tipe data float
      float suhu = suhuString.toFloat();

      if(suhu > 33.5){
        digitalWrite(Aktuator.FanPin, HIGH);
      }
      else{
        digitalWrite(Aktuator.FanPin, LOW);
      }

    }
  }
}

void getDataAll() {
  Portal.handleClient();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  mergeData();
  getDataUltra();
  getDataDHT();
  getDataLDR();
  /*Dataservo();
  DataPompa();
  DataLed();
  DataFan();*/
}