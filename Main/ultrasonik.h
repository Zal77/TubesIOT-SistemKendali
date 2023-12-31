class Ultrasonik {
  private:
    int trigPin;
    int echoPin;
  
  public:
    Ultrasonik(int trig, int echo) : trigPin(trig), echoPin(echo) {
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

  float bacaJarak() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    float wadah = 12;
    unsigned long duration = pulseIn(echoPin, HIGH);
    float jarak = duration / 58.0; // Konversi dari durasi ke jarak dalam sentimeter
    float selisih = wadah - jarak;
    jarak = ((wadah - selisih) / wadah) * 100;
    return jarak;
  }
};

