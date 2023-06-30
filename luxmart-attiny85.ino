#include <SoftSerial.h>
#include <TinyPinChange.h>

#define PWM_PIN 4
#define RX_PIN 1
#define TX_PIN 3

int value = 0;
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
            case 'w':
            value += step;
            value = min(255, value);
            analogWrite(PWM_PIN, value);

            int newValue = serial.read();
            if(0 < newValue && newValue < 255)
                value = newValue;

            analogWrite(PWM_PIN, value);
            break;

            case 'r':
            serial.write('w');
            serial.write(value);
            break;
        }
    }
}