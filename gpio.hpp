#ifndef GPIO_HPP 
#define GPIO_HPP

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

const std::string EXPORT_PATH	= "/sys/class/gpio/export";
const std::string UNEXPORT_PATH	= "/sys/class/gpio/unexport";
const std::string GPIO_PATH	= "/sys/class/gpio/gpio";

const std::string DIRECT_PATH(const std::string& pin);
const std::string VALUE_PATH(const std::string& pin);

const std::string IN	= "in";
const std::string OUT	= "out";

const std::string LED_PIN_NUM = "18";
const std::string BUT_PIN_NUM = "17";

const bool HI	= 1;
const bool LOW	= 0;

class Gpio
{
public:

	Gpio(const std::string& p, const std::string& d);
	~Gpio();

	void operator << (bool val);
	void operator >> (bool& val);
private:
	static int writeToPath(const std::string& path, bool val);
	static int writeToPath(const std::string& path, const std::string&  val);
	static bool readFromPath(const std::string& path);

	const std::string pin;
};

#endif // GPIO_HPP 
