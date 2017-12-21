# PSTOmni
Omnidirectional treadmill for VR - ESIEA PST project

This is the repository for the software that will power the platform.


# Compiling

This project is intended to run on a raspberry PI.

It needs my fork of the vl6180x_pi https://github.com/Ybalrid/vl6180_pi and it the mma8451_pi https://github.com/Ybalrid/mma8451_pi libraries installed on the system. 

Theses two repository contain a makefile, you just need to do

```bash
make
sudo make install
```

to install them. If this project cannot find them, you need to configure ld to find the libraries in the local directory (not enabled by default on Raspbian)

```bash
sudo echo /usr/local/lib > /etc/ld.conf.so.d/local.conf
sudo ldconfig
```


# Hardware setup

Using the Raspberry Pi GPIO bus, 2 polulu ST VL6180x sensors, and one Adafruit MMA8451 plugged into the I2C bus.

 - The MMA8451 has it's address pin jumped to ground
 - On of the ST_VL6180x has it's chip enable pin pulled to ground, plugged into the GPIO4 pin of the Raspberry Pi

The creation of a `sensorArray` object intialize all the sensors.

Plans of the wooden platform will be released later.









