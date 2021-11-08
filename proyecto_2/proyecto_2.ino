/*******************************
//Universidad del Valle de Guatemala
//BE3015: Electrónica Digital 2
//Andrea Palencia
//Proyecto 2

//*******************************

//Definición de pines 
#define tx PD_7
#define rx PD_6
#define boton1 PD_4
#define boton2 PD_0




void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);

}

void loop() {
 
  int EstadoBI = digitalRead(BtnI);
  int EstadoBD = digitalRead(BtnD);

  if(EstadoBI == LOW)
  {
    DCLedG++;
    delay(50);
    if (DCLedG > 254)
    {
      DCLedG == 0;
    }
  }
  
  if(EstadoBD == LOW)
  {
    DCLedG--;
    delay(50);
    if (DCLedG < 1)
    {
      DCLedG == 253;
    }
  }
}
