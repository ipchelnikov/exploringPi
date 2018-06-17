#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

#include "gpio.hpp"

int main(int argc, char* argv[])
{
	Gpio led(LED_PIN_NUM, OUT);
	Gpio but(BUT_PIN_NUM, IN);
	
	bool butVal = LOW, prevButVal = LOW, ledVal = LOW;
	
	while(true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		but >> butVal;

		if(butVal == HI && prevButVal == HI) // Exit on long press (1 sec)
		{
			break;
		}
		else if(butVal == HI)
		{
			std::cout << "butVal" << butVal << std::endl;			

			ledVal = !ledVal;

			prevButVal = HI;

			led << ledVal;
		}
		else
		{
			prevButVal = LOW;
		}
		
	}
	
	return 0;
}
