#include "define.h"
#include <EEPROM.h>

#include <ClickEncoder.h>
#include <TimerOne.h>
#include <LiquidCrystal.h>

ClickEncoder *encoder;
int16_t last, value;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int LCDBrightness = 255;
boolean LCDupdate = false;
unsigned long time = 0;

char ModeName[3][17] = {
  "Move Shoot Move ",
  "Video travelling",
  "  Manual Mode   "};
                        

enum mode{
  MSM,                                                //Move Shoot Move mode, for timelapse
  VT,                                                 //Video Traveling mode
  MAN                                                 //Manual Mode
};

enum state{
  ON,                                                 //Movement ON
  STBY,                                                //No movement
  CONF
};

enum direction{
  AB,                                                 // From point A to B
  BA                                                  // From point B to A
};



mode Mode = MSM;
state State = STBY;
direction Dir = AB;

void timerIsr()
{
  encoder->service();      //Call encoder service each 1ms
}

void setup()
{
  initIO();
  Serial.begin(115200);
  initKnob();
  
  lcd.clear();
  for (int i = 0; i<LCDBrightness ; i++)
  {
    analogWrite(LCD_BL,LCDBrightness);
  }
  lcd.print("Motorized dolly ");
  lcd.setCursor(0,1);
  lcd.print("07/08/2014 V1.00"); 
  delay(1000);
  LCDupdate = true;
  
  
}

void loop()
{
  
  
}

void updateKnob()
{
  value += encoder->getValue();
  if (value != last) 
  {
    last = value;
    Serial.print("Encoder Value: ");
    Serial.println(value);
  }
  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open)
  {

  }
}

void recall()
{
  State = (state)EEPROM.read(STATE_ADR);
  Mode = (mode)EEPROM.read(MODE_ADR);
  Dir = (direction)EEPROM.read(DIR_ADR);
}

void save()
{
  EEPROM.write(STATE_ADR,State);
  EEPROM.write(MODE_ADR,Mode);
  EEPROM.write(DIR_ADR,Dir);
}

void initIO()
{
  pinMode(STEP,OUTPUT);
  pinMode(DIR,OUTPUT);
  pinMode(LCD_BL,OUTPUT);

  pinMode(BSW,INPUT_PULLUP);
  pinMode(RSW,INPUT_PULLUP);
  pinMode(KNOBA,INPUT_PULLUP);
  pinMode(KNOBB,INPUT_PULLUP);
  pinMode(KNOBSW,INPUT_PULLUP);

  pinMode(ENDSWA,INPUT_PULLUP);
  pinMode(ENDSWB,INPUT_PULLUP);

  pinMode(Vin,INPUT);
}

void initKnob()
{
  encoder = new ClickEncoder(KNOBA, KNOBB, KNOBSW);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  last = -1;
}

void LCD_update()
{
    if (LCDupdate)
    {
      LCDupdate = false;
      
      analogWrite(LCD_BL, LCDBrightness);
      
      
      lcd.clear();
      lcd.print(ModeName[Mode]);
    }
}

void move(int Nsteps, int dir)
{
  if (Nsepts > ACCELTHRESH)
  {
    for (int i = 0; i< 10; i++)
    {
      1step();
      myDelay(MINSTEPTIME*10/(10-i);
    }
    for (int i = 0; i< Nsteps - 20 ; i++)
    {
      1step();
      myDelay(MINSTEPTIME);
    }
    for (int i = 0; i< 10; i++)
    {
      1step();
      myDelay(MINSTEPTIME*10/(i+1));
    }
  }
}

void 1step()
{
  digitalWrite(STEP,HIGH);
  myDelay(1);
  digitalWrite(STEP,LOW);
}

void myDelay(int time)
{
  Time = millis();
  while (millis()-Time < time);
}

