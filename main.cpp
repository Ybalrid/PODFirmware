#include <thread>
#include <chrono>
#include <vl6180_pi/vl6180_pi.h>
#include <wiringPi.h>
#include <iostream>

using std::cout;

/*
 * sensors are:
 * Xdst : vl6180 sensor with chip enable pin tied to the PI's GPIO4
 * Ydst : another vl6180 sensor, CE pin *not connected*
 * acc : a X accelerometer, at another I2C address*/

const int X_RST = 4; //GPIO ID for reseting the X distance sensor.
const int defaultADDR = 0x29;
bool initGPIO()
{
    /*auto status = wiringPiSetup();
    if(status == -1) return false; //TODO handle error reporting here.

    pinMode(X_RST, OUTPUT);
    return true;*/


    system("echo 4 > /sys/class/gpio/export 2>/dev/null");
    system("echo out > /sys/class/gpio/gpio4/direction 2>/dev/null");
    system("echo 0 > /sys/class/gpio/gpio4/value");
    return true;
}

void deactivateX()
{
    cout << "Deactivating X sensor...\n";
//    digitalWrite(X_RST, 0);
    system("echo 0 > /sys/class/gpio/gpio4/value");
}

void activateX()
{
    cout << "Activating X sensor...\n";
    //digitalWrite(X_RST, 1);
    system("echo 1 > /sys/class/gpio/gpio4/value");
}

int main()
{
    cout << "Initialize GPIO...\n";
    auto status = initGPIO();

    if(status) cout << "Pin " << X_RST << " set to output\n";
    else return 1;

    deactivateX();
    delay(100);

    vl6180 handle = vl6180_initialise(1, defaultADDR);
    std::cout << "debug : handle : " << handle << '\n';
    vl6180_change_addr(handle, 0x27);

    std::cout << "debug : handle : " << handle << '\n';
    auto Ydst = handle;
    cout << "Y sensor initialized\n";

    activateX();
    delay(100);

    handle = vl6180_initialise(1, defaultADDR);
    std::cout << "debug : handle : " << handle << '\n';
    auto Xdst = handle;
    cout << "X sensor initialized\n";

    cout << "Sensor initialized!\n";
    while(true)
    {
        cout << "distance x: " << get_distance(Xdst) << "mm";
        cout << " y: " << get_distance(Ydst) << "mm\n";
    }
}
