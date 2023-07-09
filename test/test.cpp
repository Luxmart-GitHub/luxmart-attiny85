// This test is checking the ATTiny85 firmware on a development machine.
// We create a minimum environment for the firmware to emulate the serial
// communication.

#include "luxmart-attiny85.ino"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

const std::string putCommands[] =
{
	"r",
	"a",
	"aa",
	"dd",
	"aaa",
	"m0",
	"0m",
	"wy",
	"wxwywzw\x2a"
	"xyz"
	"r"
};

const char getValues[] = { '\x0', '\x2a' };

int main(int argc, char* argv[])
{
	std::atomic<bool> shutdown = false;

	std::thread loopThread([&shutdown]()
	{
		setup();

		while (!shutdown)
		{
			loop();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	std::thread serialPutThread([]()
	{
		for (auto& cmd : putCommands)
		{
			for (const char& ch : cmd)
			{
				serial.put(ch);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	});

	std::thread serialGetThread([]()
	{
		unsigned int ivalue = 0;

		while (1)
		{
			char ch;
			if (serial.get(&ch))
			{
				if (ch == 'v')
					std::cout << 'v';
				else
				{					
					auto ch_expected = getValues[ivalue++];
					if (ch != ch_expected)
					{
						std::cerr << "Unexpected output value: got " << hex << static_cast<int>(ch) <<
							", expected " << hex << static_cast<int>(ch_expected) << std::endl;
						exit(-2);
					}
					
					std::cout << "0x" << hex << static_cast<int>(ch) << std::endl;

					if (ivalue >= sizeof(getValues)) return;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	// First wait for all inputs to be consumed.
	serialPutThread.join();

	// Next, wait for all expected output.
	serialGetThread.join();
	
	// Finally, shut down the loop.
	shutdown = true;
	loopThread.join();
	return 0;
}
