#include <thread>
#include <chrono>
#include <vl6180_pi/vl6180_pi.h>
#include <iostream>

using std::cout;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

/*
 * sensors are:
 * Xdst : vl6180 sensor with chip enable pin tied to the PI's GPIO4
 * Ydst : another vl6180 sensor, CE pin *not connected*
 * acc : a X accelerometer, at another I2C address*/

const int X_RST = 4; //GPIO ID for reseting the X distance sensor.
const int defaultADDR = 0x29;
bool initGPIO()
{
    system("echo 4 > /sys/class/gpio/export 2>/dev/null");
    system("echo out > /sys/class/gpio/gpio4/direction 2>/dev/null");
    system("echo 0 > /sys/class/gpio/gpio4/value");
    return true;
}

void deactivateX()
{
    cout << "Deactivating X sensor...\n";
    system("echo 0 > /sys/class/gpio/gpio4/value");
}

void activateX()
{
    cout << "Activating X sensor...\n";
    system("echo 1 > /sys/class/gpio/gpio4/value");
}

int main()
{
    cout << "Initialize GPIO...\n";
    auto status = initGPIO();

    if(status) cout << "Pin " << X_RST << " set to output\n";
    else return 1;

    deactivateX();
	sleep_for(milliseconds(100));

    vl6180 handle = vl6180_initialise(1, defaultADDR);
    cout << "debug : handle : " << handle << '\n';
    vl6180_change_addr(handle, 0x27);

    cout << "debug : handle : " << handle << '\n';
    auto Ydst = handle;
    cout << "Y sensor initialized\n";

    activateX();
    sleep_for(milliseconds(100));

    handle = vl6180_initialise(1, defaultADDR);
    cout << "debug : handle : " << handle << '\n';
    auto Xdst = handle;
    cout << "X sensor initialized\n";

    cout << "Sensor initialized!\n";
    while(true)
    {
        cout << "distance x: " << get_distance(Xdst) << "mm";
        cout << " y: " << get_distance(Ydst) << "mm\n";
    }
}
