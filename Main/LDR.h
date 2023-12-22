class ldrsensor{
  private:
    int ldr;
  
  public:
    ldrsensor(int ldrPin) : ldr(ldrPin){
    }

  int bacaNilai(){
    int ldrnilai;
    ldrnilai = analogRead(ldr);
    return ldrnilai;
  }
};