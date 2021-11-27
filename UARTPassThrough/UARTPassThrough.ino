//-------------------------------------------------------------------------------
//  TinyCircuits ST BLE TinyShield UART Example Sketch
//  Last Updated 2 March 2016
//
//  This demo sets up the BlueNRG-MS chipset of the ST BLE module for compatiblity 
//  with Nordic's virtual UART connection, and can pass data between the Arduino
//  serial monitor and Nordic nRF UART V2.0 app or another compatible BLE
//  terminal. This example is written specifically to be fairly code compatible
//  with the Nordic NRF8001 example, with a replacement UART.ino file with
//  'aci_loop' and 'BLEsetup' functions to allow easy replacement. 
//
//  Written by Ben Rose, TinyCircuits http://tinycircuits.com
//
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <STBLE.h>


//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif


uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

#define BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN          0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

uint8_t amtcolors=7;
uint8_t colors[]={BLACK,BLUE,RED,GREEN,WHITE,DGREEN,YELLOW};

uint8_t i=0;
  
uint8_t nextColor(){
  if(i>=amtcolors)i=0;
  return colors[i++];
}

#include <TinyScreen.h>
#include <Wire.h>
#include <RTCZero.h>
#include <String.h>

TinyScreen display = TinyScreen(TinyScreenDefault);
RTCZero rtc;
bool a = false;
bool b = false;
bool c = false;
int hh = 10;
int mm = 10;
int ss = 10;
byte buff[100];

void setup() {
  Wire.begin();//initialize I2C before we can initialize TinyScreen- not needed for TinyScreen+
  display.begin();
  //setBrightness(brightness);//sets main current level, valid levels are 0-15
  display.setBrightness(10);
  BLEsetup();
}


void loop() {
  if (display.getButtons(TSButtonLowerLeft)){
    display.clearScreen();
    a = true;
    b = false;
    c = false;
  }
  if (display.getButtons(TSButtonLowerRight)){
    display.clearScreen();
    a = false;
    b = true;
    c = false;
  }
  if (display.getButtons(TSButtonUpperRight)){
    display.clearScreen();
    a = false;
    b = false;
    c = true;
  }
  if (display.getButtons(TSButtonUpperLeft)){
    display.clearScreen();
    a = false;
    b = false;
    c = false;
  }
  if (a){
    timesync();
  }
  if (b){
    getcalendar();
  }
  if (c){
    sos();
  }
}

void fun(){
  display.setFont(liberationSans_10ptFontInfo);
  display.fontColor(WHITE,BLACK);
  
  for(int i=5;i<20;i++){
    display.setCursor((3000-(i*5))%70,(i*14)%50);
    display.print("Hi");
    display.setCursor((i*7)%70,(6000-(i*6))%50);
    display.print("Hi");
    delay(150);
  }
  for(int i=0;i<5;i++){
    display.setCursor(0,i*12);
    display.print("FASTFASTFASTFAST");
  }
  delay(200);
  
  for(int i=5;i<70;i++){
    display.setCursor((3000-(i*5))%70,(i*14)%50);
    display.fontColor(i|(i<<4),nextColor());
    display.print("color");
    display.setCursor((i*7)%70,(6000-(i*6))%50);
    display.print("COLOR");
  }
  display.fontColor(WHITE,BLACK);
  display.clearWindow(0,0,96,64);
  display.setFont(liberationSans_8ptFontInfo);
  display.setCursor(0,0);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_12ptFontInfo);
  display.setCursor(10,10);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_14ptFontInfo);
  display.setCursor(24,24);
  display.print("SIZE");
  delay(200);
  display.setFont(liberationSans_16ptFontInfo);
  display.setCursor(40,40);
  display.print("SIZE");
  delay(200);
  
  
  display.setCursor(15,20);
  display.fontColor(BLACK,WHITE);
  display.clearWindow(0,0,96,64);
  display.setBrightness(15);
  display.drawRect(0,0,96,64,1,WHITE);
  display.print("BRIGHT!");
  delay(500);
  display.fontColor(WHITE,BLACK);
  display.setCursor(30,20);
  display.clearWindow(0,0,96,64);
  display.setBrightness(0);
  display.print("DIM!");
  delay(500);
  display.setBrightness(5);
  
  
  display.fontColor(YELLOW,BLACK);
  display.setCursor(15,15);
  display.clearWindow(0,0,96,64);
  display.print("Boxes!");
  delay(200);
  for(int i=5;i<800;i++){
    int x=(3000-(i*5))%90;
    int y=(i*14)%60;
    int width=((i*7)%(90-x));
    int height=((6000-(i*6))%(60-y));
    display.drawRect(x,y,constrain(width,5,20),constrain(height,5,15),i&1,nextColor());
    if(i<10)
      delay(40);
    if(i<50)
      delay(40);
  }
  delay(500);
  display.clearWindow(0,0,96,64);
}



int timesync() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  ble_rx_buffer_len = 0;//clear afer reading
  delay(1000);//should catch input
  lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"1", (uint8_t)2);
  Time();
}

int getcalendar() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  ble_rx_buffer_len = 0;//clear afer reading
  delay(1000);//should catch input
  lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"1", (uint8_t)2);
  calendar();
}

int sos() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  ble_rx_buffer_len = 0;//clear afer reading
  delay(1000);//should catch input
  lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"1", (uint8_t)2);
}



void calendar(){
  display.clearScreen();
  char *text = (char*)ble_rx_buffer;
  int width=display.getPrintWidth(text);
  //setCursor(x,y);//set text cursor position to (x,y)
  display.setCursor(48-(width/2),10);
  display.print(text);
  delay(1000);
}

void Time(){
  byte arr[3];
  int arrIndex = 0;
  char *text = (char*)ble_rx_buffer;
  char hour[3];
  char minute[3];
  char second[3];
  hour[3] = '\0';
  minute[3] = '\0';
  second[3] = '\0';
  hour[0] = text[0];
  minute[0] = text[2];
  second[0] = text[4];
  hour[1] = text[1];
  minute[1] = text[3];
  second[1] = text[5];
  display.clearScreen();
  rtc.begin();
  rtc.setTime(atoi(hour), atoi(minute), atoi(second));
  int num = rtc.getHours();
  char snum[5];
  itoa(num, snum, 10);
  int num1 = rtc.getMinutes();
  char snum1[5];
  itoa(num1, snum1, 10);
  int num2 = rtc.getSeconds();
  char snum2[5];
  itoa(num2, snum2, 10);
  char str[80];
  strcpy(str, snum);
  strcat(str, " ");
  strcat(str, snum1);
  strcat(str, " ");
  strcat(str, snum2);
  //setFont sets a font info header from font.h
  //information for generating new fonts is included in font.h
  display.setFont(thinPixel7_10ptFontInfo);
  //setCursor(x,y);//set text cursor position to (x,y)-
  display.setCursor(25,32);
  //fontColor(text color, background color);//sets text and background color
  display.fontColor(TS_8b_Green,TS_8b_Black);
  display.print(str);
  delay(1000);
}
