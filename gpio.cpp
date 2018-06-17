#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "gpio.hpp"

const std::string DIRECT_PATH(const std::string& pin) { return GPIO_PATH + pin + "/direction"; }
const std::string VALUE_PATH(const std::string& pin) { return GPIO_PATH + pin + "/value"; }

Gpio::Gpio(const std::string& p, const std::string& d) : pin(p) 
{	
	if ( writeToPath(EXPORT_PATH, pin) != 0 )
		throw;

	int count = 0;

	while ( writeToPath(DIRECT_PATH(pin), d) != 0 )
	{
		if ( count++ == 10) 
			throw;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

Gpio::~Gpio() 
{
	if ( writeToPath(UNEXPORT_PATH, pin) != 0 )
		std::cout << "Unable to unexport GPIO" << pin << std::endl;
}

void Gpio::operator << (bool val)
{
	writeToPath(VALUE_PATH(pin), val);
}

void Gpio::operator >> (bool& val)
{	
	val = readFromPath(VALUE_PATH(pin));
}

int Gpio::writeToPath(const std::string& path, bool val)
{
	std::ofstream ofst_path(path);

	if (ofst_path.rdstate() & std::ofstream::failbit)
		return -1;

	ofst_path << val;
	ofst_path.close();

	return 0;
}


int Gpio::writeToPath(const std::string& path, const std::string& val)
{
	std::ofstream ofst_path(path);

	if (ofst_path.rdstate() & std::ofstream::failbit)
		return -1;

	ofst_path << val;
	ofst_path.close();

	return 0;
}

bool Gpio::readFromPath(const std::string& path)
{
	bool b;

	std::ifstream ifst_path(path);

	if (ifst_path.rdstate() & std::ifstream::failbit)
		throw;
	
	ifst_path >> b;
	ifst_path.close();

	return b;
}
