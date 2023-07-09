#include "luxmart-attiny85.ino"

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

int main(int argc, char* argv[])
{
	std::thread loopThread([]()
	{
		setup();

		while (1)
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
		while (1)
		{
			char ch;
			if (serial.get(&ch))
			{
				if (ch == 'v')
					std::cout << 'v';
				else
					std::cout << "0x" << hex << static_cast<int>(ch) << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	loopThread.join();
	serialPutThread.join();
	serialGetThread.join();

	return 0;
}
