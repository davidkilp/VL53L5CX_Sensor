#pragma once

#include <cstdint>

#ifdef VL53L5CXSENSOR_EXPORTS
#define SENSOR_API __declspec(dllexport)
#else
#define SENSOR_API __declspec(dllimport)
#endif

class VL53L5CXSensor {
private:
	void* _vl53_sensor;

public:

	VL53L5CXSensor(uint8_t i2c_address);
	~VL53L5CXSensor();

	bool startRanging();
	bool stopRanging();
	bool isDataReady();
	double getRange();
};

// Helper methods for constructor and exported methods
extern "C" SENSOR_API void* Instantiate(uint8_t i2c_address);

extern "C" SENSOR_API void Conclude(VL53L5CXSensor* t);

extern "C" SENSOR_API bool startRanging(VL53L5CXSensor* t);

extern "C" SENSOR_API bool stopRanging(VL53L5CXSensor* t);

extern "C" SENSOR_API bool isDataReady(VL53L5CXSensor* t);

extern "C" SENSOR_API double getRange(VL53L5CXSensor* t);
