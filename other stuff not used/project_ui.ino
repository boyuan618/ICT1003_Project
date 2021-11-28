#include <TinyScreen.h>
#include <Wire.h>

TinyScreen display = TinyScreen(TinyScreenDefault);

void setup(void) {
  Wire.begin();//initialize I2C before we can initialize TinyScreen- not needed for TinyScreen+
  display.begin();
  //setBrightness(brightness);//sets main current level, valid levels are 0-15
  display.setBrightness(10);
  
}

void loop() {
  writeSteps(20);
  writeSMSsent();
}


void writeSteps(int steps){
  String steps_text = String(steps,DEC);
  String counter_text = String("Steps: " + steps_text);
  display.clearScreen();
  //setFont sets a font info header from font.h
  //information for generating new fonts is included in font.h
  display.setFont(thinPixel7_10ptFontInfo);
  //setCursor(x,y);//set text cursor position to (x,y)-
  display.setCursor(25,32);
  //fontColor(text color, background color);//sets text and background color
  display.fontColor(TS_8b_Green,TS_8b_Black);
  display.print(counter_text);
  delay(1000);
}

void writeSMSsent(){
  display.clearScreen();
  int width=display.getPrintWidth("Fall SMS sent!");
  //setCursor(x,y);//set text cursor position to (x,y)
  display.setCursor(48-(width/2),10);
  display.print("Fall SMS sent!");
  delay(1000);
}
