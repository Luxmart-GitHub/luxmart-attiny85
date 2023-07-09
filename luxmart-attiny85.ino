#include <SoftSerial.h>
#include <TinyPinChange.h>

#define PWM_PIN 4
#define RX_PIN 1
#define TX_PIN 3

int value = 0;
const int step = 10;

enum Command
{
  CommandStepUp = 'u',
  CommandStepDown = 'd',
  CommandSetValue = 'w',
  CommandGetValue = 'r',
  CommandSetMax = 'm',
  CommandSetMin = '0',
};

// We receive from the serial either commands or values,
// because we allow user to directly specify a value of PWM level.
enum CommandMode
{
	ReceiveCommand = 0,
	ReceiveValue
};

// This flag controls the current serial interpretation:
// a command or a value.
CommandMode mode = ReceiveCommand;

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
		switch (mode)
		{
		case ReceiveCommand:
			switch (rc)
			{
				// Keys for incrementing and decrementing by a fixed step
			case CommandStepUp :
				value += step;
				value = min(255, value);
				analogWrite(PWM_PIN, value);
				break;
			case CommandStepDown :
				value -= step;
				value = max(0, value);
				analogWrite(PWM_PIN, value);
				break;
				// Key for setting a specific value
			case CommandSetValue :
				// Switch serial interpretation to value mode.
				mode = ReceiveValue;
				break;
				// Key for reading the value
			case CommandGetValue :
				// Indicate to the user that a value is coming.
				serial.write('v');
				serial.write(value);
				break;
				// Keys for setting to minimum and maximum value
			case CommandSetMax :
				value = 255;
				analogWrite(PWM_PIN, value);
				break;
			case CommandSetMin :
				value = 0;
				analogWrite(PWM_PIN, value);
				break;
			}
			break;
		case ReceiveValue:
			value = *(reinterpret_cast<unsigned char*>(&rc));
			analogWrite(PWM_PIN, value);
			// Value received; switc serial back to command mode.
			mode = ReceiveCommand;
			break;
		}
	}
}
