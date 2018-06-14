#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

const char* gpioLed = "18";
const char* gpioButton = "17";

int setDirection(const char* pin, std::string direction)
{
	std::ofstream setdirgpio(std::string("/sys/class/gpio/gpio") + pin + "/direction");
	
	if (setdirgpio.rdstate() & std::ofstream::failbit)
	{
		std::cout << "Unable to set direction GPIO" << pin << std::endl;
		return -1;
	}
	
	setdirgpio << direction;
	setdirgpio.close();
	
	return 0;
}

int main(int argc, char* argv[])
{
	// Export 
	
	std::ofstream exportgpio("/sys/class/gpio/export");

	if (exportgpio.rdstate() & std::ofstream::failbit)
	{
		std::cout << "Unable to export GPIO" << std::endl;
		return -1;
	}
	
	exportgpio << gpioLed; 

	exportgpio.close();

	exportgpio.open("/sys/class/gpio/export");

	if (exportgpio.rdstate() & std::ofstream::failbit)
	{
		std::cout << "Unable to export GPIO" << std::endl;
		return -1;
	}
	exportgpio << gpioButton;
	exportgpio.close();
	
	// Set directions
	
	if( setDirection(gpioLed, "out") ||  setDirection(gpioButton, "in"))
		return -1;
	
	// Perform
	
	bool butVal = false, prevButVal = false, ledVal = false;
	
	while(true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	
		std::ifstream getvalgpio(std::string("/sys/class/gpio/gpio") + gpioButton + "/value");
		
		if (getvalgpio.rdstate() & std::ifstream::failbit)
		{
			std::cout << "Unable to get value of GPIO" << gpioButton << std::endl;
			return -1;
		}
		
		getvalgpio >> butVal;
		getvalgpio.close();

		std::cout << butVal << std::endl;
		
		if(butVal && prevButVal) // Exit on long press (1 sec)
		{
			break;
		}
		else if(butVal)
		{
			ledVal = !ledVal;

			prevButVal = true;
			
			std::ofstream setvalgpio(std::string("/sys/class/gpio/gpio") + gpioLed + "/value" );
			
			if (setvalgpio.rdstate() & std::ofstream::failbit)
			{
				std::cout << "Unable to set value for GPIO" << std::endl;
				return -1;
			}
			
			setvalgpio << ledVal;
			setvalgpio.close();
		}
		else
		{
			prevButVal = false;
		}
		
	}
	
	// Unexport
	
	std::ofstream unexportgpio("/sys/class/gpio/unexport");
	if (unexportgpio.rdstate() & std::ofstream::failbit)
	{
		std::cout << "Unable to unexport GPIO" << std::endl;
		return -1;
	}
	
	unexportgpio << gpioLed;	
	unexportgpio.close();
	
	unexportgpio.open("/sys/class/gpio/unexport");
	
	unexportgpio << gpioButton;	
	unexportgpio.close();

	return 0;
}
