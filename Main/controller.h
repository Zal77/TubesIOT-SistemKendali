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
const char* topic = "TubesIOT/kelompok1/sensor";
const char* topic2 = "TubesIOT/kelompok1/aktuator";
/*const char* topicjarak = "TubesIOT/kelompok/1/jarak";
const char* topictemp = "TubesIOT/kelompok/1/dht/temperatur";
const char* topichumi = "TubesIOT/kelompok/1/dht/humidity";
const char* topicLDR = "TubesIOT/kelompok/1/LDR";*/

Ultrasonik SensorUltrasonik(4, 5);  // Menggunakan pin 4 sebagai TRIG dan pin 5 sebagai ECHO
dhtsensor SensorDHT(18); //Menggunakan pin 2
ldrsensor SensorLDR(34); //Menggunakan pin 18
aktuator Aktuator(33, 25, 26, 27); //Menggunakan pin 33(Servo) 25(Pompa) 26(LED) 27(Fan)

String datagabungan;
String buffData;

char datatemp[50];
char datahumi[50];
char dataLDR[50];
char datajarak[50];

void rootPage() {
  char content[] = "KAMI KELOMPOK SATU";
  Server.send(200, "kelompok1/IOT", content);
}

void callback(char* topic2, byte* payload, unsigned int length) {
  buffData = "";
  for (int i = 0; i < length; i++) {
    buffData += (char)payload[i];
  }

  int separatorIndex1 = buffData.indexOf('#');
  String ultrasonik = buffData.substring(0, separatorIndex1);
  buffData.remove(0, separatorIndex1 + 1);

  int separatorIndex2 = buffData.indexOf('#');
  String dht = buffData.substring(0, separatorIndex2);
  buffData.remove(0, separatorIndex2 + 1);

  // Sisanya adalah nilai LDR
  String ldr = buffData;
  
  Serial.print(topic2);
  Serial.print(" ==> ");
  Serial.print("Data aktuator"+buffData);
  Serial.print(String() + "| Nilai PUMP: " + ultrasonik);
  Serial.print(String() + "\tNilai FAN: " + dht);
  Serial.println(String() + "\tNilai LED: " + ldr);


  if (ultrasonik == "PUMPON") {
    digitalWrite(Aktuator.PompaPin, HIGH);
    Serial.print(Aktuator.PompaPin);
  } else if (ultrasonik == "PUMPOFF") {
    digitalWrite(Aktuator.PompaPin, LOW);
    Serial.print(Aktuator.PompaPin);
  }
  if (dht == "FANON") {
    digitalWrite(Aktuator.FanPin, HIGH);
    Serial.print(Aktuator.FanPin);
  } else if (dht == "FANOFF") {
    digitalWrite(Aktuator.FanPin, LOW);
    Serial.print(Aktuator.FanPin);
  }
  if (ldr == "LEDON") {
    digitalWrite(Aktuator.LedPin, HIGH);
    Serial.print(Aktuator.LedPin);
  } else if (ldr == "LEDOFF") {
    digitalWrite(Aktuator.LedPin, LOW);
    Serial.print(Aktuator.LedPin);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "KELOMPOK-SATU";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic2);
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
  String dataDHT;
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

void getDataAll() {
  Portal.handleClient();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  mergeData();
  callback((char*)topic2, (byte*)buffData.c_str(), buffData.length());
}