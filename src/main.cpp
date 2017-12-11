#include <chrono>
#include <iostream>

#include "sensor.hpp"


int main()
{
    sensorArray sensors;
    
    auto last = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();

    for(;;)
    {
        sensors.updateAll();
        std::cout << sensors.getDistanceReadout() << '\n';
        std::cout << sensors.getAccelerationReadout() << '\n';

        last = now;
        now = std::chrono::system_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count() << " ms\n";
    }
}
