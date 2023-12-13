class ldrsensor{
  private:
    int ldr;
  
  public:
    ldrsensor(int ldrPin) : ldr(ldrPin){
      pinMode(ldr, INPUT);
    }

  int bacaNilai(){
    return analogRead(ldr);
  }
};