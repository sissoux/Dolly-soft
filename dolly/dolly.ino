#include "define.h"
#include <EEPROM.h>

#include <ClickEncoder.h>
#include <TimerOne.h>
#include <LiquidCrystal.h>



ClickEncoder *encoder;
int16_t last, value = 10000;

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
int LCDBrightness = 150;
boolean LCDupdate = false;
unsigned long Time = 0;
unsigned long counter = 0;
int dir = 1;
unsigned int delayValue = 0;
unsigned int exposure = 10;
unsigned int shootDuration = 5400;
unsigned int waitDuration = 1;

  unsigned int NSteps ;
  unsigned int StepVal ;

char ModeName[3][17] = {
  "Move Shoot Move ",
  "Video travelling",
  "  Manual Mode   "
};

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
  lcd.begin(16, 2);
  initKnob();
  
  digitalWrite(EN,HIGH);

  lcd.clear();
  delay(250);
  for (int i = 0; i<LCDBrightness ; i++)
  {
    analogWrite(LCD_BL,i);
    delay(5);
  }
  lcd.setCursor(0,0);
  lcd.print("Motorized dolly ");
  lcd.setCursor(0,1);
  lcd.print("07/08/2014 V1.01"); 
  delay(1000);
  LCDupdate = true;
  
  lcd.clear();
    lcd.setCursor(0,0);
  lcd.print("Exposure time :");
  last = exposure-1;
  value = exposure;
  while (digitalRead(BSW))
  {
    
    updateKnob();
    if (!value)
    {
      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.setCursor(0,1);
      lcd.print(" DSLR selection ");
    }
    else
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(value);
    }
    myDelay(100);
  }
  exposure = value;
  
  myDelay(500);
  
  lcd.clear();
    lcd.setCursor(0,0);
  lcd.print("Shoot duration :");
  last = shootDuration-1;
  value = shootDuration;
  while (digitalRead(BSW))
  {
  updateKnob();
    lcd.setCursor(0,1);
    lcd.print("     ");
    lcd.setCursor(0,1);
    lcd.print(value);
    myDelay(100);
  }
  shootDuration = value;
  
  myDelay(500);
  
  lcd.clear();
    lcd.setCursor(0,0);
  lcd.print("Wait duration :");
  last = waitDuration-1;
  value = waitDuration;
  while (digitalRead(BSW))
  {
  updateKnob();
    lcd.setCursor(0,1);
    lcd.print("     ");
    lcd.setCursor(0,1);
    lcd.print(value);
    myDelay(100);
  }
  waitDuration = value;
  
  myDelay(500);
  
  NSteps = shootDuration / (exposure + waitDuration);
  StepVal = MAXSTEPS/NSteps;
  
  
  lcd.clear();
  lcd.print("Start ?");
  while (digitalRead(RSW))
  {
  }
  
  
  myDelay(500);
    
  
  
  lcd.clear();
  lcd.print("Move Shoot Move");
  
  analogWrite(LCD_BL,10);
  
  digitalWrite(EN,LOW);

}

void loop()
{
  
  myDelayMicro(waitDuration/2);   //Wait before shot for stabilization
  
  digitalWrite(DIR,dir);          //Choose direction
  digitalWrite(EN,LOW);           //Enable Stepper driver
  
  pinMode(SHOOT,OUTPUT);          //Start shoot 
  digitalWrite(SHOOT,LOW);
  
  if (!exposure)                  //Wait at least 200ms to be sure DSLR shot
  {
    mydelay(200);
  }
  
  myDelay(exposure * 1000);       //Wait exposure time (0 if not in bulb state)
  
  pinMode(SHOOT, INPUT);          //Stop shoot (Open drain output ==> Open circuit)
  digitalWrite(SHOOT, HIGH);
  
  myDelayMicro(waitDuration/2);   //Wait before move to allow DSLR to shoot
  
  for (int x = 0; x<StepVal ; x++)//Start moving sequence
  {
  oneStep();
  counter++;
  myDelayMicro(200);
  }
  /*
  if (dir)
  {
    counter++;
  }
  else 
  {
    counter--;
  }*/
  
  if (counter > MAXSTEPS)
  {
    lcd.clear();
    lcd.print("THE END");
    while(true){};
    counter--;
    dir = 0;
  }

}

void updateKnob()
{
  value += encoder->getValue();
  value = constrain(value,0,15000);
  if (value != last) 
  {
    last = value;
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
  pinMode(EN,OUTPUT);
  digitalWrite(EN,LOW);
  pinMode(LCD_BL,OUTPUT);
  digitalWrite(LCD_BL,LOW);

  pinMode(BSW,INPUT_PULLUP);
  pinMode(RSW,INPUT_PULLUP);
  pinMode(KNOBA,INPUT_PULLUP);
  pinMode(KNOBB,INPUT_PULLUP);
  pinMode(KNOBSW,INPUT_PULLUP);

  pinMode(ENDSWA,INPUT_PULLUP);
  pinMode(ENDSWB,INPUT_PULLUP);

  pinMode(VIN,INPUT);
}

void initKnob()
{
  encoder = new ClickEncoder(KNOBA, KNOBB, KNOBSW);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  last = 10000;
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
  if (Nsteps > ACCELTHRESH)
  {
    for (int i = 0; i< 10; i++)
    {
      oneStep();
      myDelay(MINSTEPTIME*10/(10-i));
    }
    for (int i = 0; i< Nsteps - 20 ; i++)
    {
      oneStep();
      myDelay(MINSTEPTIME);
    }
    for (int i = 0; i< 10; i++)
    {
      oneStep();
      myDelay(MINSTEPTIME*10/(i+1));
    }
  }
}

void oneStep()
{
  digitalWrite(STEP,HIGH);
  myDelayMicro(100);
  digitalWrite(STEP,LOW);
}

void myDelay(int time)
{
  Time = millis();
  while (millis()-Time < time);
}

void myDelayMicro(int time)
{
  Time = micros();
  while (micros()-Time < time);
}


