#include <SoftSerial.h>
#include <TinyPinChange.h>

#include "api/pwm_controller_commands.h"

//#define DEBUG

#ifdef DEBUG
#define DBGLOG(msg) serial.println(msg)
#else
#define DBGLOG(msg) do {} while (0)
#endif

#define LED_PIN 0
#define PWM_PIN 4
#define BTN_PIN 2
#define RX_PIN 1
#define TX_PIN 3

int value = 0;
const int step = 10;

// We receive from the serial either commands or values,
// because we allow user to directly specify a value of PWM level.
enum CommandMode
{
	ReceiveCommand = 0,
	ReceiveValue
};

// This flag controls the current serial interpretation:
// a command or a value.
static CommandMode mode = ReceiveCommand;

static SoftSerial serial(RX_PIN, TX_PIN);

void setup()
{
  serial.begin(BAUD_RATE);
  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, value);
	
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

static int blink_count = -1;
static int blink_interval = 0;

static void process_blink()
{
  static char state = LOW;
  static unsigned long prev_blink = millis();

  if ((blink_interval == 0) || (blink_count == 0)) return;

  if (millis() > prev_blink + blink_interval)
  {
    digitalWrite(LED_PIN, state);
    state ^= HIGH;
    if (blink_count > 0) blink_count -= state;
  }
}

static ButtonState button_state = ButtonUnpressed;

static void process_button()
{
  static int buttonState = HIGH;
  static int previousButtonState = HIGH;
  static unsigned long buttonPressTime = 0;
  static const unsigned long longPressDuration = 4000;  // msec

  // Read the state of the button
  buttonState = digitalRead(BTN_PIN);

  // Check if button is pressed (LOW), and was not pressed before.
  if (buttonState == LOW && previousButtonState == HIGH)
  {
    buttonPressTime = millis();  // Record the current time
  }
  // Check if button in released (HIGH), and was pressed before.
  else if (buttonState == HIGH && previousButtonState == LOW)
  {
    // Check for short press (less than longPressDuration)
    if (millis() - buttonPressTime < longPressDuration)
    {
      button_state = ButtonPressed;
      DBGLOG("Short press");
    }
    else // Check for long press (greater than or equal to longPressDuration)
    {
      button_state = ButtonLongPressed;
      DBGLOG("Long press");
    }
  }

  // Store the current state for the next iteration
  previousButtonState = buttonState;
}

static void receive_value(char rc)
{
  DBGLOG("receive_value()");
  value = *reinterpret_cast<unsigned char*>(&rc);
  analogWrite(PWM_PIN, value);
}

static void receive_blink_mode(char mode)
{
  DBGLOG("receive_blink_mode()");
  switch(mode)
  {
  case BlinkOff :
    blink_interval = 0;
    digitalWrite(LED_PIN, HIGH);
    break;
  case BlinkOn :
    blink_interval = 0;
    digitalWrite(LED_PIN, LOW);
    break;
  case BlinkSlow1s :
    blink_interval = 1000;
    blink_count = 1;
    break;
  case BlinkSlow2s :
    blink_interval = 1000;
    blink_count = 2;
    break;
  case BlinkSlow3s :
    blink_interval = 1000;
    blink_count = 3;
    break;
  case BlinkSlowInfinite :
    blink_interval = 1000;
    blink_count = -1;
    break;
  case BlinkFast1 :
    blink_interval = 500;
    blink_count = 1;
    break;
  case BlinkFast2 :
    blink_interval = 500;
    blink_count = 2;
    break;
  case BlinkFast3 :
    blink_interval = 500;
    blink_count = 3;
    break;
  case BlinkFastInfinite :
    blink_interval = 500;
    blink_count = -1;
    break;
  }
}

static void (*callback)(char);

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
        // Handle the received value with appropriate callback.
        callback = &receive_value;
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
			case CommandBlink :
				// Switch serial interpretation to value mode.
				mode = ReceiveValue;
        // Handle the received value with appropriate callback.
        callback = &receive_blink_mode;
				break;
			case CommandReboot :
				// TODO
				break;
			case CommandGetState :
        // Report the button state
        serial.write('s');
				serial.write(button_state);
				break;
      case CommandClearState :
        // Clear the button state
        button_state = ButtonUnpressed;
        break;
			}
			break;
		case ReceiveValue:
			(*callback)(rc);
			// Value received; switch serial back to command mode.
			mode = ReceiveCommand;
			break;
		}
	}

  process_blink();
  process_button();
}
