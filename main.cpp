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

bool initGPIO()
{
    auto status = wiringPiSetup();
    if(status == -1) return false; //TODO handle error reporting here.

    pinMode(X_RST, OUTPUT);
    return true;
}

void deactivateX()
{
    cout << "Deactivating X sensor...\n";
    digitalWrite(X_RST, 0);
}

void activateX()
{
    cout << "Activating X sensor...\n";
    digitalWrite(X_RST, 1);
}

int main()
{
    cout << "Initialize GPIO...\n";
    auto status = initGPIO();

    if(status) cout << "Pin " << X_RST << " set to output\n";
    else return 1;

    deactivateX();
    delay(100);
    activateX();
    delay(100);

    vl6180 handle = vl6180_initialise(1);
    cout << "Sensor initialized!\n";
    while(true)
    {
        cout << "distance: " << get_distance(handle) << "mm\n";
    }
}
