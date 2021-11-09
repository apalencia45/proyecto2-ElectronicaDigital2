//Definición de pines 
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
  Serial.begin(115200); //no funciona con 115200
  
  Serial2.begin(115200);

  
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode (buzzer, OUTPUT);
  
}

void loop() {

int value = digitalRead(BtnSPI);
  if((value == LOW)&&(EstadoAnterior==HIGH))
  {
    X = 1-X;
    TonoDato();
    Serial2.write(X);
  }
  EstadoAnterior=value;

  if(Serial2.available()>0)
  {
    Sensor = Serial2.readStringUntil('\n');
    Serial.print("El valor del sensor es de: ");
    Peso = Sensor.toFloat();
    Serial.println(Peso);
  }
  
//Guardar el valor del sensor 
  int EstadoB2 = digitalRead(boton2);
  if((boton2 == LOW)&&(EstadoB2==HIGH))
  {
    TonoSD(); //Se reproduce el tono en el buzzer
    MemoriaSD(); //Se guardan los datos en la memoria
    Y = 1-Y; 
  }
  EstadoB2=boton2;


   
  
   
    }
    }
