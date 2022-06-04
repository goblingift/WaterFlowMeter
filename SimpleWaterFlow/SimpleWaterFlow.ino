#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define SENSOR  27
#define LED 2

const int signalPin = 27;
const int buttonPin = 34;
const int resetCounterSeconds = 5;
unsigned int  literPerHour;
float  literPerMinute, literPerMillis, passedWaterInLiter;
unsigned long currentTime, loopTime, passedMillis, lastReset;
volatile byte pulseCount;
int buttonState = 0;
boolean resetInProcess = false;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {

  Serial.begin(115200);

  pinMode(signalPin, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(buttonPin, INPUT_PULLDOWN);
  buttonState = digitalRead(buttonPin);


  u8g2.begin();

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 0);
  u8g2.drawStr(0, 15, "Starting Goblingift");
  u8g2.drawStr(0, 40, "Waterflow-Sensor...");
  u8g2.sendBuffer();

  delay(3000);
  u8g2.setFont(u8g2_font_fur20_tf);

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, RISING);
  currentTime = millis();
  loopTime = currentTime;
  lastReset = currentTime;
  passedMillis = 0;
  passedWaterInLiter = 0.0;
}

void loop ()
{

  // if enough time passed, check for reset by button again
  if ((millis() - lastReset) > 5000 && isButtonPressed()) {
    startReset();
  }

  if (resetInProcess) {
    handleResetLogic();
  } else {
    calculateWaterFlowAndPrint();
  }
}

boolean isButtonPressed() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == 1) {
    return true;
  } else {
    return false;
  }
}

void startReset() {
  Serial.println("Reset triggered!");
  resetInProcess = true;
  lastReset = millis();
}

void handleResetLogic() {

  unsigned long passedTimeSinceReset = millis() - lastReset;
  
  if ((passedTimeSinceReset / 1000) >= resetCounterSeconds) {
    // reset done, back to normal
    resetInProcess = false;
    passedWaterInLiter = 0.0;
    Serial.println("Reset done!");
  } else {
    int secondsLeft = resetCounterSeconds - (passedTimeSinceReset / 1000);
    updateScreenWithResetCounter(String(secondsLeft));
  }
  
}

void calculateWaterFlowAndPrint() {

  currentTime = millis();
  if (currentTime >= (loopTime + 1000)) {

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

    updateScreenWithStats(literPerMinute, passedWaterInLiter);
    pulseCount = 0;
  }

}

void updateScreenWithResetCounter(String seconds) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fur17_tf);
  u8g2.drawStr(0, 22, " Reset in:");
  u8g2.setCursor(0, 60);
  u8g2.print(seconds);
  u8g2.sendBuffer();
}

void updateScreenWithStats(float literPerMinute, float passedWaterInLiter) {

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
}
