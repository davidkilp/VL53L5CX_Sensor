// tof_client_cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

// VL53LCX Sensor Interface (.dll) 
#include "VL53L5CXSensor.h"


// VL53L5CS I2C address
//const uint8_t I2C_SLAVE_ADDR = (0x52 >> 1);
#define I2C_SLAVE_ADDR  (0x52 >> 1)

// VL53L5CS ranging poll rate in msec
const uint8_t SensorPollRate = 10;

//! Create a custom convert function, remove the tailing zeros if necessary.  
template<typename T>
std::string tostring(const T& n) {
    std::ostringstream oss;
    oss << n;
    std::string s = oss.str();
    size_t dotpos = s.find_first_of('.');
    if (dotpos != std::string::npos) {
        size_t ipos = s.size() - 1;
        while (s[ipos] == '0' && ipos > dotpos) {
            --ipos;
        }
        s.erase(ipos + 1, std::string::npos);
    }
    return s;
}


static int runDistanceService()
{
    int ret = 1;
    double distance = 0;
    std::cout << "TOF Sensor Service Loop start" << std::endl;

    try {
        //VL53L5CXSensor *vl53_sensor = new VL53L5CXSensor(I2C_SLAVE_ADDR);
        VL53L5CXSensor* vl53_sensor = (VL53L5CXSensor *) Instantiate(I2C_SLAVE_ADDR);

        //vl53_sensor.setErrorCallback(&sensorErrorCallback);

        startRanging(vl53_sensor);
        distance = getRange(vl53_sensor); // throw away the first entry after enabling

        //while (true) {
        for (int i = 0; i < 10; i++) {
            //  Do some 'work'
            distance = getRange(vl53_sensor);
            if (distance == 0) {
                std::cout << "Nothing present" << std::endl;
            }
            else {
                std::cout << "Avg distance: " << distance << " mm" << std::endl;

            }
            std::string sreply = tostring(distance);

        }
        std::cout << "Loop ends" << std::endl;

        Conclude(vl53_sensor);

    }
    catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        ret = 0;
    }


    return ret;
}

int main()
{
    std::cout << "TOF Sensor start";

    // The service should run forever 
    while (1)
    {
        std::cout << "Start Sensor polling...";

        if ( !runDistanceService() )
        {
            std::cout << "FT260 USB device is NOT present" << std::endl;
            std::cout << "We should wait for device to be attached " << std::endl;
            //waitForFT260();
            Sleep(1000);

        }
    }

    std::cout << "End of ULD demo" << std::endl;

    return(0);

}
