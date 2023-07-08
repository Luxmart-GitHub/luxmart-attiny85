#include <SoftSerial.h>
#include <TinyPinChange.h>

#define PWM_PIN 4
#define RX_PIN 1
#define TX_PIN 3

int value = 0;
const int step = 10;

SoftSerial serial(RX_PIN, TX_PIN);

void setup()
{
  serial.begin(9600);
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, value);
}

void loop()
{
  if (serial.available())
  {
    char rc = serial.read();
    switch (rc)
    {
      // Keys for incrementing and decrementing by a fixed step
    case 'u' :
      value += step;
      value = min(255, value);
      analogWrite(PWM_PIN, value);
      break;
    case 'd' :
      value -= step;
      value = max(0, value);
      analogWrite(PWM_PIN, value);
      break;
      // Key for setting a specific value
    case 'w':
      value = serial.read();
      value = *(reinterpret_cast<unsigned char*>(&value));
      analogWrite(PWM_PIN, value);
      break;
      // Key for reading the value
    case 'r':
      serial.write('w');
      serial.write(value);
      break;
      // Keys for setting to minimum and maximum value
    case 'm' :
      value = 255;
      analogWrite(PWM_PIN, value);
      break;
    case '0' :
      value = 0;
      analogWrite(PWM_PIN, value);
      break;
    }
  }
}