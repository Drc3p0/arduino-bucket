// uses the sparkfun dangerboard

#include <CapSense.h>

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

// globals

boolean is_running = false;
long ms_count = 0;
long ms_offset = 0;
float bpm_min = 70;
// add one to max -- since we cast bpm as an integer, it'll never hit bpm_max
float bpm_max = 191;
long tap_times[8];
int tap_i = 0;
boolean tap_valid = true;
int tap_bpm = 0;
boolean tap_mode = false;
int bpm = 0;

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

  Serial.println("Initialized midi-seq");
}

void loop()
{

  // translate slider range from 0 - 1024 to 0.0 to 1.0
  float slider_pct = analogRead(SLIDER1) / 1024.0;
  float bpm_delta = bpm_max - bpm_min;
  if (tap_mode) {
    bpm = tap_bpm;
  } else {
    bpm = (int) bpm_min + (bpm_delta * slider_pct);
  }
  int ms_per_beat = (int) ((60.0 / (float) bpm) * 1000);
  int max_ms = ms_per_beat * 4;
  int beat = 1 + (ms_count - ms_offset) / ms_per_beat;

  // int my_beat = beat_nr(start_time, ms_per_beat);

  // make BUTTON1 toggle is_running
  if (!(digitalRead(BUTTON1)))
  {
    delay(100); // debounce
    is_running = !is_running;
    Serial.println("current bpm:");
    Serial.println(bpm);
  }

  if (!(digitalRead(BUTTON2)))
  {
    delay(500); // debounce
    tap_mode = !tap_mode;

    if (!tap_mode) {
      for(int z = 0; z < 8; z++)
      tap_times[z] = -1;
    }
  }

  // tap timing functionality
  if (!(digitalRead(BUTTON3)))
  {
    // reset iterator if > 7
    if (tap_i > 7) {
      tap_i = 0;
    }

    tap_times[tap_i] = millis();
    for(int x = 0; x < 50; x++)
    {
      digitalWrite(BUZZER, HIGH);
      delay(1);
      digitalWrite(BUZZER, LOW);
      delay(1);
    }

    long tap_offset = tap_times[0];
    long tap_sum = 0L;
    long lowest = 360000L;
    for(int y = 1; y < 8; y++)
    {
      if (!tap_times[y]) {
        tap_valid = false;
      } else {
        if (tap_times[y] < lowest) {
          lowest = tap_times[y];
        }
      }
    }

    if (tap_valid) {
      for(int y = 1; y < 8; y++)
      {
        if (tap_times[y] != lowest) {
          tap_sum = tap_sum + tap_times[y] - lowest;
        }
      }
      int tap_average = (tap_sum) / 7;
      float divisor = 60000L / tap_average;
      Serial.println(divisor);
      Serial.println(60 / divisor);
      tap_bpm = (60000L / tap_average) * 2;
    }

    tap_valid = true;
    tap_i++;
  }

  if (is_running) {
    ms_count = millis();
    if (ms_count > ms_offset + max_ms) {
      ms_offset = millis();
    }

    Serial.println(ms_count - ms_offset);

    switch (beat) {
      case 1:
        // on-beat, illuminate LED2
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, LOW);
        break;
      case 2:
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
        break;
      case 3:
        // off-beat, illuminate LED1
        digitalWrite(LED2, HIGH);
        digitalWrite(LED1, LOW);
        break;
      case 4:
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
        break;
      // default:
      //   digitalWrite(LED1, LOW);
      //   digitalWrite(LED2, LOW);
      //   break;
    }

    Serial.println(bpm);

    // write beat to seven seg display
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, ~(ledCharSet[beat]));
    digitalWrite(LATCH, HIGH);

  } else { // !is_running
    // <put code here>
  } // end !is_running

} // end loop
