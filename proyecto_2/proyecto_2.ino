///Definición de pines 
#define tx PD_7
#define rx PD_6
#define boton1 PF_4
#define boton2 PF_0
#define buzzer PA_7

#define note_cc 261
#define note_dd 294

//Librerias 


// Variables globales 
int inicio=0;

//Funciones 


void setup() {
  Serial.begin(9600); //no funciona con 115200
  
  Serial2.begin(115200);

  
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode (buzzer, OUTPUT);
  
}

void loop() {
 
  int EstadoB1 = digitalRead(boton1);
  int EstadoB2 = digitalRead(boton2);

  Serial2.write(inicio);
  
  Serial.println("funciona chika");
  
  if(EstadoB1 == LOW)
  {
   inicio=1;
   //Serial2.write (inicio);

  


   //if (Serial2.available()>0){
    tone(buzzer, note_cc,100);
    delay(113);
    tone(buzzer,note_dd,100);
    delay(113);
  // }
   
    }
    }
