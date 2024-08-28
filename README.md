# VL53L5CX_Sensor

The ST Micro VL53L5CX is a Time-of-Flight (TOF) sensor used as a proximity sensor for the HID Gate Controller.

The VL53L5CX has an I2C interface and connects to a computer running Windows through a FT260Q chip from FTDI that provides a USB-to-I2C bridge.

The solution here provides a C++ DLL that hides all the low level details for controlling the VL53L5CX sensor and comes with sample command line clients written in both C++ and .NET.

## Requirements:
* Visual Studio 2022
* .NET 8 Runtime must be installed on hosted system. See: [.NET 8 Runtime Installer:](https://dotnet.microsoft.com/en-us/download/dotnet/thank-you/runtime-desktop-8.0.2-windows-x64-installer)
* LibFT260.dll from FTDI Chip. A copy is included here but the original can be obtained from [[FTDI directly](https://ftdichip.com/products/ft260q/)]

## API

The VL53L5CX_Sensor.dll provides the following API for user programs:

```
extern "C" SENSOR_API void* Instantiate(uint8_t i2c_address);

extern "C" SENSOR_API void Conclude(VL53L5CXSensor* t);

extern "C" SENSOR_API bool startRanging(VL53L5CXSensor* t);

extern "C" SENSOR_API bool stopRanging(VL53L5CXSensor* t);

extern "C" SENSOR_API bool isDataReady(VL53L5CXSensor* t);

extern "C" SENSOR_API double getRange(VL53L5CXSensor* t);
```
