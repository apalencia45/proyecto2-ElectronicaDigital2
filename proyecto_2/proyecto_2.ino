//Definición de pines 
#define tx PD_7
#define rx PD_6
#define boton1 PF_4
#define boton2 PF_0




void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);

}

void loop() {
 
  int EstadoB1 = digitalRead(boton1);
  int EstadoB2 = digitalRead(boton2);

  if(EstadoB1 == LOW)
  {
   int  inicio=1;
   Serial2.write (inicio);
   
    }
    }
