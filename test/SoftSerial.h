#ifndef SOFT_SERIAL_H
#define SOFT_SERIAL_H

#include <stdint.h>

class SoftSerial
{
public :

	SoftSerial(int rx, int tx)
	{
	}

	void begin(int baudrate)
	{
	}
	
	bool available()
	{
		return true;
	}
	
	int read()
	{
		return 0;
	}
	
	void write(uint8_t byte)
	{
	}
};

#endif // SOFT_SERIAL_H

