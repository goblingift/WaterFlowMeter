#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define SENSOR  27

const int signalPin = 27;
unsigned int  literPerHour;
float  literPerMinute, literPerMillis, passedWaterInLiter;
unsigned long currentTime, loopTime, passedMillis;
volatile byte pulseCount;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {
   pinMode(signalPin, INPUT);
   Serial.begin(9600); 

   u8g2.begin();
   
   u8g2.setFont(u8g2_font_6x10_tf);
   u8g2.clearBuffer();
   u8g2.setCursor(0, 0);
   u8g2.drawStr(0,15,"Starting Goblingift");
   u8g2.drawStr(0,40,"Waterflow-Sensor...");
   u8g2.sendBuffer();

   delay(3000);
   u8g2.setFont(u8g2_font_fur20_tf);
   
   attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, RISING);                                  
   currentTime = millis();
   loopTime = currentTime;
   passedMillis = 0;
   passedWaterInLiter = 0.0;
}

void loop ()    
{
   currentTime = millis();
   if(currentTime >= (loopTime + 1000))
   {
      passedMillis = currentTime - loopTime;
      
      loopTime = currentTime;
      literPerHour = pulseCount * 6.6;
      Serial.print("Pulsecount:");
      Serial.println(pulseCount);
      
      literPerMinute = (pulseCount / 6.6);
      Serial.print(literPerMinute, 2);
      Serial.println(" Liter/min");

      literPerMillis = literPerMinute / 60.0 / 1000.0;
      Serial.print(literPerMillis, 6);
      Serial.println(" Liter/millis");

      passedWaterInLiter = passedWaterInLiter + (literPerMillis * passedMillis);

      // write labels
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_fur17_tf);
      u8g2.drawStr(80, 22, " L/m");
      u8g2.drawStr(100, 60, " L");

      // write values
      u8g2.setFont(u8g2_font_fur20_tf);
      
      String convertedLitres = String(literPerMinute, 1);
      u8g2.setCursor(0, 22);
      u8g2.print(convertedLitres);
            
      String strPassedWaterInLiter = String(passedWaterInLiter, 3);
      u8g2.setCursor(0, 60);
      u8g2.print(strPassedWaterInLiter);
      
      u8g2.sendBuffer();
      
      pulseCount = 0;
   }
}
