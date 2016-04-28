// uses the sparkfun dangerboard
#include "Arduino.h"

const byte ledCharSet[10] = {
  B00111111,B00000110,B01011011,B01001111,B01100110,B01101101,B01111101,B00000111,B01111111,B01101111
};

#define SLIDER1 0
#define BUZZER 3
#define DATA 4
#define LED1 5
#define LED2 6
#define LATCH 7
#define CLOCK 8
#define BUTTON1 10
#define BUTTON2 11
#define BUTTON3 12

void setup()
{
  Serial.begin(9600);

  pinMode(BUTTON1,INPUT);
  digitalWrite(BUTTON1, HIGH);
  pinMode(BUTTON2,INPUT);
  digitalWrite(BUTTON2, HIGH);
  pinMode(BUTTON3,INPUT);
  digitalWrite(BUTTON3, HIGH);

  pinMode(BUZZER, OUTPUT);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);

  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  Serial.println("Initialized bpm test");
}

int i = 0;

void loop()
{

  if (!(digitalRead(BUTTON1)))
  {
    delay(100); // debounce
    Serial.println("BUTTON1 pressed");

    Serial.println("SLIDER1's value is:");
    Serial.println(analogRead(SLIDER1));
    Serial.println("(should be between 0 and 1023)");
    Serial.println("===================");
  }

  if (!(digitalRead(BUTTON2)))
  {
    delay(100); // debounce
    Serial.println("BUTTON2 pressed");

    Serial.println("BUZZER should beep:");
    for(int x = 0; x < 150; x++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(1);
      digitalWrite(BUZZER, LOW);
      delay(1);
    }
    Serial.println("===================");
  }

  if (!(digitalRead(BUTTON3)))
  {
    delay(100); // debounce
    Serial.println("BUTTON1 pressed");

    Serial.println("7 segment display should show:");
    Serial.println(i);

    // write beat to seven seg display
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, ~(ledCharSet[i]));
    digitalWrite(LATCH, HIGH);

    if (i % 2 == 0) {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      Serial.println("LED1 should be lit, 7seg is even");
    } else {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      Serial.println("LED2 should be lit, 7seg is odd");
    }

    i++;
    if (i > 9) {
      i = 0;
    }
    Serial.println("===================");
  }

} // end loop
