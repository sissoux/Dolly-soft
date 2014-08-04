#include "define.h"


#include <ClickEncoder.h>
#include <TimerOne.h>

ClickEncoder *encoder;
int16_t last, value;


void timerIsr()
{
  encoder->service();      //Call encoder service each 1ms
}

void setup()
{
  Serial.begin(115200);
  encoder = new ClickEncoder(KNOBA, KNOBB, KNOBSW);
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  
  last = -1;
}

void loop()
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
