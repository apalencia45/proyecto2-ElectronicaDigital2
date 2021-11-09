//Universidad del Valle de Guatemala
//Electronica Digital 2
//Andrea Palencia 
//TIVA C
//Proyecto 

//Librerias de SD
#include <SPI.h>
#include <SD.h>

//File
File myFile;

//Librerias para la pantalla TFT
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"



//Definición de pines 
#define tx PD_7
#define rx PD_6
#define boton1 PF_4
#define boton2 PF_0
#define buzzer PA_7

#define note_cc 261
#define note_dd 294


#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};


// Variables globales 
int medir = 0;
int BPM = 0;
int save = 0;

//Funciones 

//Funciones de la pantalla
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//Funciones de SD
void MemoriaSD(void);
void TonoSD(void);
void TonoDato(void);

//Funcion Buzzer
void buzzerSave(void);

extern uint8_t fondo[];
extern uint8_t heart[];
extern uint8_t pulse[];

void TFTconf(void);

//INICIALIZACION 
void setup() {
  //Com serial
  Serial.begin(115200); 
  Serial2.begin(115200);

//SD
  SPI.setModule(0);
  
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(, OUTPUT);

  if (!SD.begin(32)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  root = SD.open("/");

  printDirectory(root, 0);

  //Botones
  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode (buzzer, OUTPUT);
  
}

void loop() {

int EstadoB1 = digitalRead(Boton1);
  if((boton1 == LOW)&&(ultimoestado1==HIGH))
  {
    X = 1-X;
    TonoDato();
    Serial2.write(X);
  }
  ultimoestado1=boton1;

  //if(Serial2.available()>0)
 // {
    Sensor = Serial2.readStringUntil('\n');
    Serial.print("El valor del sensor es de: ");
    Peso = Sensor.toFloat();
    Serial.println(Peso);
  //}
  
//Guardar el valor del sensor 
  int EstadoB2 = digitalRead(boton2);
  if((boton2 == LOW)&&(EstadoB2==HIGH))
  {
    TonoSD(); //Se reproduce el tono en el buzzer
    MemoriaSD(); //Se guardan los datos en la memoria
    abrir(2);
    Y = 1-Y; 
  }
  EstadoB2=boton2;
  
   }
    
    }
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
        Serial.println(entry.size(), DEC);
        var++;
        if(var > 4){
          if (var < 8){
            // se estan imprimiendo los nombres de los archivos
            // enumerarlos
            numero++;
            Serial.println("");
            Serial.print("imagen ");
            Serial.print(numero);
            Serial.println(":");
          }
        }
     }
     entry.close();
   }
}

void abrir(int num){
    if (num == 1){
        myFile = SD.open("archivo.txt");
        if (myFile) {
          Serial.print("archivo.txt:");
      
           var = 0;  // reiniciar el indice del array
            while (highscore3[var - 1] != 10) { // leer los caracteres hasta el enter
              highscore3[var] = modo2.read();
              if ((highscore3[var] < 48) && (highscore3[var] != 10)) {
                highscore3[var] = 48;
              }
              var++;
            }
            high3 = (((int)highscore3[0] - 48) * 100) + (((int)highscore3[1] - 48) * 10) + ((int)highscore3[2] - 48);
 
          myFile.close();
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
        }
    } else  if (num == 2){
      
    SD.remove("archivo.txt");
        modo2 = SD.open("archivo.txt", FILE_WRITE);
    // if the file opened okay, write to it:
   
      itoa(high2, snum, 10);  //  determiar cuantos digitos tiene el high2
      if (high2 > 99) {  
        modo2.print(snum[0]);
        modo2.print(snum[1]);
        modo2.print(snum[2]);
      } else if (high2 > 9) {
        modo2.print("0");
        modo2.print(snum[0]);
        modo2.print(snum[1]);
      } else {
        modo2.print("0");
        modo2.print("0");
        modo2.print(snum[0]);
      }
      modo2.print("\n");      // escribir el enter


      // close the file:
      modo2.close();
      Serial.println("done.");
    } else {
      // si es un numero distinto, imprimir que no se encontro la imagen
      Serial.println("Imagen no encontrada");
    }
}
