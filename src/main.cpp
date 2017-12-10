#include <thread>
#include <chrono>
#include <vl6180_pi/vl6180_pi.h>
#include <mma8451_pi/mma8451_pi.h>
#include <iostream>

#include "sensor.hpp"


int main()
{
    sensorArray sensors;
    
    for(;;)
    {
        sensors.updateAll();
        std::cout << sensors.getDistanceReadout() << '\n';
        std::cout << sensors.getAccelerationReadout() << '\n';
    }
}
