#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier

#include <iostream>
#include "VL53L5CXSensor.h"
#include "HID_VL53L5CX.h"

// VL53L5CS ranging poll rate in msec
const uint8_t SensorPollRate = 10;

// Callback called when I2C communication error with sensor
static void sensorErrorCallback(SF_VL53L5CX_ERROR_TYPE errorCode, uint32_t errorValue)
{
    //std::cout << "I2C communication errorCode: " << errorCode << " errorValue: " << errorValue << std::endl;
    printf("I2C Communication errorCode: %d, errorValue: %d\n", errorCode, errorValue);
    throw std::runtime_error("FT260 I2C communication error: " + std::to_string(errorValue));
}


VL53L5CXSensor::VL53L5CXSensor(uint8_t i2c_address)
{
    std::cout << "VL53L5CXSensor() Constructor called" << std::endl;
    
    // if we call this more than once, get rid of the previous class
    if (_vl53_sensor)
        delete _vl53_sensor;
    // create an HID vl52 sensor class 
	//HID_VL53L5CX vl53_sensor(i2c_address);
	//_vl53_sensor = &vl53_sensor;
    _vl53_sensor = new HID_VL53L5CX(i2c_address);
    std::cout << "_vl53_sensor ptr: " << _vl53_sensor << std::endl;
    HID_VL53L5CX* psensor = (HID_VL53L5CX*)_vl53_sensor;
    psensor->setErrorCallback(&sensorErrorCallback);
}

VL53L5CXSensor::~VL53L5CXSensor()
{
    std::cout << "VL53L5CXSensor() destructor called" << std::endl;
    HID_VL53L5CX *psensor = (HID_VL53L5CX*)_vl53_sensor;
    psensor->~HID_VL53L5CX();
}


bool VL53L5CXSensor::startRanging()
{
	return ((HID_VL53L5CX *)_vl53_sensor)->startRanging();
}

bool VL53L5CXSensor::stopRanging()
{
	return ((HID_VL53L5CX*)_vl53_sensor)->stopRanging();
}

bool VL53L5CXSensor::isDataReady()
{
	return ((HID_VL53L5CX*)_vl53_sensor)->isDataReady();
}

/*
* getRange() -- returns the average distance detected
* 
* In 4x4 mode there are 16 separate zones arranged as follows:
*  
*  | 12 | 13 | 14 | 15 |
*  |  8 |  9 | 10 | 11 |
*  |  4 |  5 |  6 |  7 |
*  |  0 |  1 |  2 |  3 |
*
* If the sensor is arraged as described in the manual.
* 
* For our use we probably only want to look at the inner 
* 4-zones: 5, 6, 9, 10
* which correspond to the active center of the sensor 
*/
double VL53L5CXSensor::getRange()
{
	VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
	double avg = 0;

    //((HID_VL53L5CX *)_vl53_sensor)->startRanging();

    uint8_t loop = 0;
    while (loop < 1)    // if ranging is disabled this needs to be at least 2 
    {
        /* Use polling function to know when a new measurement is ready.
         * Another way can be to wait for HW interrupt raised on PIN A3
         * (GPIO 1) when a new measurement is ready */
        if (((HID_VL53L5CX*)_vl53_sensor)->isDataReady() == true)      // poll on sensor this is non-blocking!
        {
            if (((HID_VL53L5CX*)_vl53_sensor)->getRangingData(&Results))
            {
                double sum = 0;
                uint8_t valid_count = 0;
                /* As the sensor is set in 4x4 mode by default, we have a total
                 * of 16 zones to examine but we only care about center zones: 5, 6, 9, 10
                 */
                int zones[] = { 5, 6, 9, 10 };
                for (int i : zones)
                //for (int i = 0; i < 16; i++)
                {
                    printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
                            i,
                            Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
                            Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);

                    // if status = 5 (distance is valid) 
                    //          AND 
                    // distance is at least 10mm but < 1219 mm (~4ft) then add it to the average value
                    if ((Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i] == 5) &&
                        ((Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i] > 10) &&
                        (Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i] < 1200))
                        )
                    {
                        sum += Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i];
                        ++valid_count;
                    }
                }
                printf("\n");
                if (valid_count) {
                    avg = sum / valid_count;
                    printf("Avg Distance for %u valid values: %g mm\n", valid_count, avg);
                }
                else {
                    printf("Nothing present\n");
                    avg = 0;
                }
                loop++;
            }
        }

        /* Wait a few ms to avoid too high polling */
        Sleep(SensorPollRate);
    }

    //((HID_VL53L5CX *)_vl53_sensor)->stopRanging();

    return(avg);

}

extern "C" SENSOR_API void* Instantiate(uint8_t i2c_address) {
    return (void*) new VL53L5CXSensor(i2c_address);
}

extern "C" SENSOR_API void Conclude(VL53L5CXSensor* t) {
    t->~VL53L5CXSensor();
    return;
}

extern "C" SENSOR_API bool startRanging(VL53L5CXSensor* t) {
    return t->startRanging();
}

extern "C" SENSOR_API bool stopRanging(VL53L5CXSensor* t) {
    return t->stopRanging();
}

extern "C" SENSOR_API bool isDataReady(VL53L5CXSensor* t) {
    return t->isDataReady();
}

extern "C" SENSOR_API double getRange(VL53L5CXSensor* t) {
    return t->getRange();
}
