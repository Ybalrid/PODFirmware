#include <vl6180_pi/vl6180_pi.h>
#include <iostream>

using std::cout;

int main()
{
    vl6180 handle = vl6180_initialise(1);
    while(true)
    {
        cout << "distance: " << get_distance(handle) << "mm\n";
    }
}
