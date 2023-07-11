#ifndef SOFT_SERIAL_H
#define SOFT_SERIAL_H

#include <cassert>
#include <mutex>
#include <queue>
#include <stdint.h>

class SoftSerial
{
	std::mutex mtx;
	std::queue<uint8_t> input, output;

public :

	SoftSerial(int rx, int tx)
	{
	}

	void begin(int baudrate)
	{
	}
	
	bool available()
	{
		std::scoped_lock lock{mtx};
		return !input.empty();
	}
	
	int read()
	{
		std::scoped_lock lock{mtx};
		assert(!input.empty());
		int result = input.front();
		input.pop();
		return result;
	}
	
	void write(uint8_t byte)
	{
		std::scoped_lock lock{mtx};
		output.push(byte);
	}
	
	void put(int value)
	{
		std::scoped_lock lock{mtx};
		input.push(value);
	}
	
	bool get(char* value)
	{
		std::scoped_lock lock{mtx};
		if (output.empty()) return false;
		
		*value = output.front();
		output.pop();
		return true;
	}
};

#endif // SOFT_SERIAL_H

