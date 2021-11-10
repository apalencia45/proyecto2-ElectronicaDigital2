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
#define boton1 PF_0 //Boton derecho 
#define boton2 PF_4 //Boton izquierdo
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

//Funcion buzzer
void buzzerSave(void);

//extern uint8_t fondo[];
//extern uint8_t heart[];
//extern uint8_t pulse[];

void TFTconf(void);

//INICIALIZACION 
void setup() {
  //Com serial
  Serial.begin(115200); 
  Serial2.begin(115200);


//Boton para iniciar el sensor 
 pinMode(boton1, INPUT_PULLUP);
 //Boton para guardar el dato 
 pinMode(boton2, INPUT_PULLUP);

 


 
//SD
  SPI.setModule(0);
  
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(PA_3, OUTPUT);

  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

 

//Pin del buzzer 
  pinMode (buzzer, OUTPUT);


//Pantalla
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  
  Serial.println("Inicio de la pantalla");
  LCD_Init();
  LCD_Clear(0xc7f4ff);
  LCD_Print("Proyecto 2", 80, 10, 2, 0x0000, 0xc7f4ff);
  LCD_Print("Pulsimetro", 50, 25, 2, 0x0000, 0xc7f4ff);
  LCD_Sprite(60, 90, 32, 32, prueba, 6, 0, 0, 1);
}

//LOOP PRINCIPAL 
void loop() {
  Serial.print("BPM:  ");
  Serial.print(BPM);
  
//Guardar el valor del sensor 
  int EstadoB1 = digitalRead(boton1);
  int EstadoB2 = digitalRead(boton2);
  Serial2.write (medir);
  if(boton2 == LOW){

    medir++;
    if (medir > 1){
      medir = 0;
    }
  }
  
  if (EstadoB2 == LOW){
    save++;
    if (save > 1){
      save = 0;
    }
  }

  if (medir == 1){
    FillRect(0,0,319,239,0xFCD3);
    String text5 = "MIDIENDO";

    LCD_Print(text5, 70,70,2,0x0000FF,0xFCD3);
    String text6 = "...";
    LCD_Print (text6, 100,100,2,0x0000FF,0xFCD3);

    String text8 = "PRESIONE EL BOTON PARA MOSTRAR DATOS ";
    LCD_Print(text8, 35,200,1,0x0000FF,0x20BC);
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
  }

  if(save == 1){
    if (medir ==1){
      medir = 0;

      if (Serial.available()> 0){
        BPM = Serial2.read();
      }
      FillRect(0,0,319,239,0x0000FF);

      String text7 = String(BPM);
      LCD_Print(text7, 65,35,2,0x0000FF,0xBF97);
      String text4 = String("LATIDOS POR MINUTOS");
      LCD_Print (text4, 95,37,1,0x0000FF,0xBF97);
      //LCD_Bitmap(70,65,150,heart);

      buzzerSave();

      
    }
  }
//GUARDAR LOS DATOS EN LA MEMORIA SD

void MemoriaSD(void){
  myFile =SD.open("BPM.txt",FILE_WRITE);

  if (myFile)
  {
    myFile.print("09/11/21");
    myFile.print(",");
    myFile.println(BPM);

    myFile.close();

    Serial.print("SE HAN GUARDADO LOS DATOS");
    Serial.print(BPM)
  }
  else 
  {
    Serial.println("No se pudo abrir el archivo"):
  }
}

//TONOS 
void TonoSD(void)
{
  tone(buzzer, 466.164, 125); //La*
  delay(125);
  tone(buzzer, 523.251, 250); //Do
  delay(250);
  tone(buzzer, 466.164, 125); //La*
  delay(125);
  tone(buzzer, 440.00, 250); //La
  delay(250);
  tone(buzzer, 391.995, 250); //Sol
  delay(250);

  tone(buzzer, 523.251, 250); //Do
  delay(250);
  tone(buzzer, 466.164, 125); //La*
  delay(125);
  tone(buzzer, 440.00, 250); //La
  delay(250);
  tone(buzzer, 391.995, 250); //Sol
  delay(250);

  tone(buzzer, 391.995, 250); //Sol
  delay(250);
  tone(buzzer, 349.228, 250); //Fa
  delay(250);
  tone(buzzer, 311.127, 250); //Re*
  delay(125);
  tone(buzzer, 311.127, 125); //Re*
  delay(125);
  tone(buzzer, 349.228, 250); //Fa
  delay(250);
  tone(buzzer, 293.665, 250); //Re
  delay(250);

  tone(buzzer, 293.665, 250); //Re
  delay(250);
  tone(buzzer, 293.665, 250); //Re
  delay(250);
  tone(buzzer, 293.665, 250); //Re
  delay(250);
  tone(buzzer, 293.665, 250); //Re
  delay(250);

  noTone(buzzer);
}

void TonoDato(void)
{
  tone(buzzer, 1046.50, 200); //Do
  delay(200);
  tone(buzzer, 1046.50, 200); //Do
  delay(200);
  tone(buzzer, 1567.98, 200); //Sol
  delay(200);
  tone(buzzer, 1567.98, 200); //Sol
  delay(200);
  tone(buzzer, 1760.00, 200); //La
  delay(200);
  tone(buzzer, 1760.00, 200); //La
  delay(200);
  tone(buzzer, 1567.98, 400); //Sol
  delay(400);

  tone(buzzer,  1396.91, 200); //Fa
  delay(200);
  tone(buzzer,  1396.91, 200); //Fa
  delay(200);
  tone(buzzer, 1318.51, 200); //Mi
  delay(200);
  tone(buzzer, 1318.51, 200); //Mi
  delay(200);
  tone(buzzer,  1174.66, 200); //Re
  delay(200);
  tone(buzzer,  1174.66, 200); //Re
  delay(200);
  tone(buzzer, 1046.50, 400); //Do
  delay(400);

  noTone(buzzer);
}

//*********************************************
// Función para inicializar LCD
//*********************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //**************
  // Secuencia de Inicialización
  //**************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //**************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //**************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //**************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //**************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //**************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //**************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //**************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //**************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //**************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //**************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //**************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //**************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//*********************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para enviar datos a la LCD - parámetro (dato)
//*********************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//*********************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//*********************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//*********************************************
// Función para borrar la pantalla - parámetros (color)
//*********************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*********************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*********************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//*********************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//*********************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//*********************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//*********************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//*********************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//*********************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//*********************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
