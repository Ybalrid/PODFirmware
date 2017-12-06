#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>

#include <vl6180_pi/vl6180_pi.h>

class outputGPIO
{
	public:
		outputGPIO(int ID) : 
			pin{ID}
		{
			//init via filesystem interface:
			std::stringstream exportCMD;
			exportCMD << echo << pin << gpioPath << "export" << redirectToNull;
			system(exportCMD.str().c_str());
			exportCMD.str("");
			exportCMD << echo << "out" << gpioPath << "gpio" << pin << "/direction" << redirectToNull;
			system(exportCMD.str().c_str());
			exportCMD.str("");

			//initia
			exportCMD << echo << "0" << gpioPath << "gpio" << pin << "/value" << redirectToNull;
			writeLowCMD = exportCMD.str();
			exportCMD.str("");
			exportCMD << echo << "1" << gpioPath << "gpio" << pin << "/value" << redirectToNull;
			writeHighCMD = exportCMD.str();
		}

		void high()
		{
			system(writeHighCMD.c_str());
		}

		void low()
		{
			system(writeLowCMD.c_str());
		}

	private:
		void waitABit()
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}

		const int pin;
		const std::string echo = "echo ";
		const std::string redirectToNull = " 2> /dev/null";
		const std::string gpioPath = " > /sys/class/gpio/";
		std::string writeHighCMD;
		std::string writeLowCMD;
};

class sensor
{
	public:
		sensor(const std::string& name) :
			sensorName{name}
		{
			std::cout << "Sensor " << sensorName << " created\n";
		}

		virtual void update() = 0;

	protected:
		const std::string sensorName;
};

class distanceSensor : public sensor
{
	public:
		distanceSensor(const std::string& name) : sensor(name)
		{
			handle = vl6180_initialise_address(1, VL6180_DEFAULT_ADDR);
		}

		void changeAddress(int address)
		{
			std::cout << "changing address of distance sensor " << sensorName <<
				" to 0x" << std::hex << address << std::dec << '\n'; 
			vl6180_change_addr(handle, address);
		}

		void update() override
		{
			distance = get_distance(handle);
		}

		int getDistance() const
		{
			return distance;
		}

	private:
		vl6180 handle;
		int distance;
};

struct vector2
{
    vector2(int a, int b) : x(a), y(b) {}
    int x, y;
};

std::ostream& operator<< (std::ostream& out, const vector2& vect)
{
    out << "vector2(" << vect.x << ", " << vect.y << ")";
    return out;
}

class sensorArray
{
	public:
		sensorArray() : 
			xEnable(4)
		{
			xEnable.low();
			auto ySensor = std::make_unique<distanceSensor>("Ydist");
			ySensor->changeAddress(0x27);
			xEnable.high();
			auto xSensor = std::make_unique<distanceSensor>("Xdist");

			sensors.push_back(std::move(xSensor));
			sensors.push_back(std::move(ySensor));
		}

		void updateAll()
		{
			for(auto& sensor : sensors)
			{
				sensor->update();
			}
		}

		sensor* get(size_t index)
		{
			return sensors[index].get();
		}

        vector2 getDistanceReadout()
        {
            return {
                static_cast<distanceSensor*>(get(0))->getDistance(),
                static_cast<distanceSensor*>(get(1))->getDistance()
            };
        }

	private:
		std::vector<std::unique_ptr<sensor>> sensors;
		outputGPIO xEnable;
};

