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

## Operation

The VL53L5CX is configured to operate in 4x4 mode which provides 16 separate "zones" that provide distance information detected in that zone.

Using the sensor as a simple proxmimity sensor, we really only care about detecting someone directly in front of the sensor.
The sensor provides a 60 degree Field of View (FoV) so to restrict the detection to somthing directly in front of the sensor 
we can examine the 4 zones in the middle of the sensor. Namely zones: 5, 6, 9, 10. 

We can further validate the sensor data by examing the zone status and the range returned so we reject any readings that are valid 
and also reject any readings that are not in the range of 10 mm to 1219 mm (approximately 4 feet).

If you run the sample, tof_client_cpp.exe, program it will intialize the sensor and beging polling it continously. 
During each polling loop, the inner 4 zones are examined and if they are valid as described above and average is taken 
for the final range result and returned as a distance in mm. 

This is shown below from a Windows command prompt or Powershell window:

```
PS C:\Public\TOF_Sensor> .\tof_client_cpp.exe
TOF Sensor startStart Sensor polling...TOF Sensor Service Loop start
VL53L5CXSensor() Constructor called
HID_VL53L5CX_IO() constructor
I2C is idle
Initializing VL53L5CX sensor. Downloading firmware, please wait
vl53l5cx_init() returns: 0VL53L5CX ULD ready ! (Version : VL53L5CX_1.3.11)

Avg Distance for 4 valid values: 750.5 mm
Avg distance: 750.5 mm
Zone :   5, Status :   5, Distance :  889 mm
Zone :   6, Status :   5, Distance :  834 mm
Zone :   9, Status :   5, Distance :  885 mm
Zone :  10, Status :   5, Distance :  815 mm

Avg Distance for 4 valid values: 855.75 mm
Avg distance: 855.75 mm
Zone :   5, Status :   5, Distance :  844 mm
Zone :   6, Status :   5, Distance :  803 mm
Zone :   9, Status :   5, Distance :  882 mm
Zone :  10, Status :   5, Distance :  771 mm

Avg Distance for 4 valid values: 825 mm
Avg distance: 825 mm
Zone :   5, Status :   5, Distance :  779 mm
Zone :   6, Status :   5, Distance : 1625 mm
Zone :   9, Status :   5, Distance :  783 mm
Zone :  10, Status :   5, Distance :  707 mm

Avg Distance for 3 valid values: 756.333 mm
Avg distance: 756.333 mm
`
