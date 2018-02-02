#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <vector>

#include <vl6180_pi/vl6180_pi.h>
#include <mma8451_pi/mma8451_pi.h>

#include <thread>
#include <chrono>

//TODO use the proper filesystem interface to use the GPIO
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

        virtual void asyncUpdateStart(){};
        virtual bool asyncUpdateAvailable(){return true;};
        virtual void readResultFromSensor(){};

	protected:
		const std::string sensorName;
};

class distanceSensor : public sensor
{
	public:
		distanceSensor(const std::string& name, unsigned char addr = VL6180_DEFAULT_ADDR) : sensor(name)
		{
			handle = vl6180_initialise_address(1, addr);
		}

		void changeAddress(int address)
		{
			std::cout << "changing address of distance sensor " << sensorName <<
				" to 0x" << std::hex << address << std::dec << '\n'; 
			vl6180_change_addr(handle, address);
		}

		void update() override
		{
            asyncUpdateStart();
            counter = 0;
            while(!asyncUpdateAvailable())
            {
                if(counter++ > 100)
                {
                    std::cerr << "Distance updat for sensor " << sensorName << " tiemouted\n";
                    break;
                }
            }

            readResultFromSensor();
		}

        void asyncUpdateStart() override
        {
            vl6180_start_range(handle);
        }

        bool asyncUpdateAvailable() override
        {
            return vl6180_is_range_available(handle);
        }

        void readResultFromSensor() override
        {
            distance = vl6180_read_distance_result(handle);
        }
            

		int getDistance() const
		{
			return distance;
		}

	private:
		vl6180 handle;
		int distance;
        char counter;
};

class accelerationSensor : public sensor
{
    public:
        accelerationSensor(const std::string& name) : sensor(name)
        {
            handle = mma8451_initialise(1, MMA8451_PULLDOWN_ADDR);
        }

        void update() override
        {
            //this sensor get data instantaneously
            readResultFromSensor();
        }

        mma8451_vector3 getAcceleration() const
        {
            return acceleration;
        }

        void readResultFromSensor() override
        {
            mma8451_get_acceleration(&handle, &acceleration);
        }
    private:
        mma8451 handle;
        mma8451_vector3 acceleration;

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
            auto accelerometer = std::make_unique<accelerationSensor>("accelerometer");

			//xEnable.low();
			auto ySensor = std::make_unique<distanceSensor>("Ydist");
			ySensor->changeAddress(0x27);
			//xEnable.high();
			//auto xSensor = std::make_unique<distanceSensor>("Xdist");
 			
            //sensors.push_back(std::move(xSensor));
			sensors.push_back(std::move(ySensor));
            sensors.push_back(std::move(accelerometer));
		}

		void updateAll()
		{
            //Start reading for every sensors
			for(auto& sensor : sensors)
                sensor->asyncUpdateStart();

            //Wait for all data to be available
            bool waiting;
            do
            {
                waiting = false;
                for(auto& sensor : sensors)
                {
                    if(!sensor->asyncUpdateAvailable())
                    {
                        waiting = true; 
                        break;
                    }
                }
            }
            while(waiting);
            
            //record state of every sensors
            for(auto& sensor : sensors)
                sensor->readResultFromSensor();

		}

		sensor* get(size_t index)
		{
			return sensors[index].get();
		}

        vector2 getDistanceReadout()
        {
            return {
                static_cast<distanceSensor*>(get(0))->getDistance(),
                //removed X static_cast<distanceSensor*>(get(1))->getDistance()
                static_cast<distanceSensor*>(get(0))->getDistance()
            };
        }

        mma8451_vector3 getAccelerationReadout()
        {
            //return static_cast<accelerationSensor*>(get(2))->getAcceleration();
            return static_cast<accelerationSensor*>(get(1))->getAcceleration();
        }

	private:
		std::vector<std::unique_ptr<sensor>> sensors;
		outputGPIO xEnable;
};

