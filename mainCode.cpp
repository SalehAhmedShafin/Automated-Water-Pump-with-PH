#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include<LiquidCrystal_I2C.h>


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
LiquidCrystal_I2C lcd2(0x27,16,2);



long duration, inches;
long set_val, percentage;
bool state, pump;


const int analogInPin = A0;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp = 0;


void setup() {

  Serial.begin(9600);

  //LCD
  lcd2.init();
  lcd2.backlight();
  lcd.begin(16, 2);
  lcd.print("WATER pH");
  lcd.setCursor(0, 1);
  lcd.print("");

  //led
  pinMode(13,OUTPUT);

  //Buzzer
  pinMode(A2, OUTPUT);

  //UltraSonic
  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  //EEPROM.write(0,15);
  set_val = EEPROM.read(0);
  //if(set_val>150)set_val=150;
  //set_val=150;
}
void loop() {

  digitalWrite(3, LOW);
  delayMicroseconds(2);
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);
  duration = pulseIn(9, HIGH);
  int i = 0;
  for (i = 0; i < 9; i++)
  {
    delayMicroseconds(2);
    digitalWrite(8, HIGH);
    delayMicroseconds(10);
    digitalWrite(8, LOW);
    duration += pulseIn(9, HIGH);
    //Serial.println(duration);
  }
  duration /= 10;
  inches = microsecondsToInches(duration);

  //Serial.println(duration);
  //Serial.println(inches);
  //Serial.println(set_val);
  //Serial.println(set_val - inches);

  lcd2.setCursor(0, 0);
  lcd2.print("WATER LEVEL:");
  lcd2.setCursor(0, 1);
  lcd2.print("PUMP:OFF MANUAL");
  //percentage=(set_val-inches)*100/set_val;
  percentage = (((set_val) - inches) * 100) / (set_val);
  //Serial.println(percentage);
  //Serial.println();


  lcd2.setCursor(12, 0);
  if (percentage < 0)percentage = 0;
  if (percentage >= 100)percentage = 99;
  lcd2.print(percentage);
  //lcd.setCursor(15, 0);
  lcd2.print("%   ");

  if (percentage < 30 & digitalRead(11))pump = 1;
  if (percentage > 79 & digitalRead(11))pump = 0;
  digitalWrite(12, !pump);

  lcd2.setCursor(5, 1);
  if (pump == 1) {
    lcd2.print("ON ");
    digitalWrite(13,HIGH);
    

  }

  if (pump == 0) {
    lcd2.print("OFF");
    digitalWrite(13,LOW);
  }

  lcd2.setCursor(9, 1);
  if (!digitalRead(11))lcd2.print("MANUAL");
  else lcd2.print("AUTO   ");

  if (!digitalRead(10) & !state & digitalRead(11)) {
    state = 1;
    //set_val=inches;
    EEPROM.write(0, set_val);
  }

  if (!digitalRead(10) & !state & !digitalRead(11)) {
    state = 1;
    pump = !pump;

  }
  Serial.println(digitalRead(10));
  if (digitalRead(10))state = 0;


  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(analogInPin);
    Serial.println(buf[i]);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];

  float pHVol = (float)avgValue * 5.0 / 1024 / 6;//4.3
  float phValue = -5.70 * pHVol + 29.5 ;
  //float phValue=pHVol*3.5;
  Serial.println(phValue);
  phValue = 14.2 - phValue;

  if(phValue>10 || phValue<5)
  {
    tone(A2, 1000);
    //noTone(10);
  }
  else
  noTone(A2);
  //tone(A2, 1000);
  
 //delay(3000);
 
 //lcd.clear();
 lcd.setCursor(0,0);  
 lcd.print(" WATER pH VALUE");
 lcd.setCursor(6,1);  
 lcd.print(phValue);
 


  delay(250);
}
long microsecondsToInches(long microseconds) {
  //return microseconds / 74 / 2;
  return microseconds * 0.034 / 2;
}