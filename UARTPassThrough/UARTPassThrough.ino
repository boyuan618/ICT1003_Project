//-------------------------------------------------------------------------------
//  From TinyCircuits ST BLE TinyShield UART Example Sketch
//  Which was written by Ben Rose, TinyCircuits http://tinycircuits.com
//-------------------------------------------------------------------------------

#include <SPI.h>
#include <STBLE.h>
#include <TinyScreen.h>
#include <Wire.h>
#include <RTCZero.h>
#include <String.h>

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

//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif
//for UART logs/debugging

uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
uint8_t amtcolors=7;
uint8_t colors[]={BLACK,BLUE,RED,GREEN,WHITE,DGREEN,YELLOW};
uint8_t i=0;
uint8_t nextColor(){
  if(i>=amtcolors)i=0;
  return colors[i++];
}


TinyScreen display = TinyScreen(TinyScreenDefault);
RTCZero rtc;
bool clearscreen = false;
byte buff[100];

void setup() {
  Wire.begin();  //initialize I2C before we can initialize TinyScreen- not needed for TinyScreen+
  display.begin();
  display.setBrightness(10);  //setBrightness(brightness);//sets main current level, valid levels are 0-15
  rtc.begin();
  BLEsetup();
}


void loop() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  ble_rx_buffer_len = 0;//clear afer reading
  display.clearScreen();
  if (display.getButtons(TSButtonLowerLeft)){ //Update time
    lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"1", (uint8_t)2);
    delay(1000);
    updatetime();
  } else if (display.getButtons(TSButtonLowerRight)){ //calendar
    lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"2", (uint8_t)2);
    delay(1000);
    calendar();
  } else if (display.getButtons(TSButtonUpperRight)){ //SOS
    lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)"3", (uint8_t)2);
    char *text = "Help is on the way!";
    int width=display.getPrintWidth(text);
    display.setCursor(48-(width/2),10);    //setCursor(x,y);//set text cursor position to (x,y)
    display.print(text);
    delay(1000);
  } else if (display.getButtons(TSButtonUpperLeft)){
    clearscreen = clearscreen^true;
    if (clearscreen) {display.setBrightness(0);
    } else {display.setBrightness(10); }
  }
  if (!clearscreen) {showTime(); } //clears screen in fn 
  delay(1000);
}

void updatetime(){
  if (ble_rx_buffer) {
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
    rtc.setTime(atoi(hour), atoi(minute), atoi(second));
    delay(1000);
  }
}

void calendar(){
  if (ble_rx_buffer) {
    display.clearScreen();
    char *text = (char*)ble_rx_buffer;
    int width=display.getPrintWidth(text);
    display.setCursor(48-(width/2),10);    //setCursor(x,y);//set text cursor position to (x,y)
    display.print(text);
    delay(1000);
  }
}

void showTime() {
  display.clearScreen();
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
  display.setFont(thinPixel7_10ptFontInfo);   //setFont sets a font info header from font.h
  display.setCursor(25,32);    //setCursor(x,y);//set text cursor position to (x,y)-
  display.fontColor(TS_8b_Green,TS_8b_Black);  //fontColor(text color, background color);//sets text and background color
  display.print(str);
}
