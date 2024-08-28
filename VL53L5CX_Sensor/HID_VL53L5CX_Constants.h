#pragma once
#ifndef __HID_VL53L5CX_Constants__
#define __HID_VL53L5CX_Constants__

#include <stdint.h>
#include <string>

// Macro for invoking the callback if the function pointer is valid
#define SAFE_CALLBACK(cb, code, value) \
    if (cb != nullptr)                 \
    {                                  \
        cb(code, value);               \
    }

// Constants declarations

const uint8_t DEFAULT_I2C_ADDR = 0x52;
const uint8_t REVISION_ID = 0x02;
const uint8_t DEVICE_ID = 0xf0;
const uint32_t UNKNOWN_ERROR_VALUE = 0xffffffff;
const uint16_t FT260_Vid = 0x0403;
const uint16_t FT260_Pid = 0x6030;

#define I2C_100KHZ  100
#define I2C_400KHZ  400


const std::string sFT260Status[] =
{
    "FT260_OK",
    "FT260_INVALID_HANDLE",
    "FT260_DEVICE_NOT_FOUND",
    "FT260_DEVICE_NOT_OPENED",
    "FT260_DEVICE_OPEN_FAIL",
    "FT260_DEVICE_CLOSE_FAIL",
    "FT260_INCORRECT_INTERFACE",
    "FT260_INCORRECT_CHIP_MODE",
    "FT260_DEVICE_MANAGER_ERROR",
    "FT260_IO_ERROR",
    "FT260_INVALID_PARAMETER",
    "FT260_NULL_BUFFER_POINTER",
    "FT260_BUFFER_SIZE_ERROR",
    "FT260_UART_SET_FAIL",
    "FT260_RX_NO_DATA",
    "FT260_GPIO_WRONG_DIRECTION",
    "FT260_INVALID_DEVICE",
    "FT260_OTHER_ERROR"
};

// Error enumeration for callback usage
enum class SF_VL53L5CX_ERROR_TYPE : uint8_t
{
    VL53_NO_ERROR,
    I2C_INITIALIZATION_ERROR,
    I2C_NOT_RESPONDING,
    DEVICE_INITIALIZATION_ERROR,
    INVALID_DEVICE,
    INVALID_FREQUENCY_SETTING,
    INVALID_RANGING_MODE,
    CANNOT_CHANGE_I2C_ADDRESS,
    CANNOT_START_RANGING,
    CANNOT_STOP_RANGING,
    CANNOT_GET_DATA_READY,
    CANNOT_GET_RESOLUTION,
    CANNOT_GET_RANGING_DATA,
    CANNOT_SET_RESOLUTION,
    CANNOT_SET_POWER_MODE,
    CANNOT_GET_POWER_MODE,
    DEVICE_NOT_ALIVE,
    INVALID_INTEGRATION_TIME,
    CANNOT_SET_INTEGRATION_TIME,
    CANNOT_GET_INTEGRATION_TIME,
    INVALID_SHARPENER_VALUE,
    CANNOT_SET_SHARPENER_VALUE,
    CANNOT_GET_SHARPENER_VALUE,
    CANNOT_SET_TARGET_ORDER,
    CANNOT_GET_TARGET_ORDER,
    INVALID_TARGET_ORDER,
    UNKNOWN_ERROR
};

// ERROR values will be reported back upon retrieving a specific setting from the sensor
// if this setting is invalid. Code will trap and default to a non-error condition if the user
// tries to pass an ERROR enum member to any function which accepts it as an argument

enum class SF_VL53L5CX_RANGING_MODE : uint8_t
{
    CONTINUOUS,
    AUTONOMOUS,
    VL53_NO_ERROR
};

enum class SF_VL53L5CX_RANGING_RESOLUTION : uint8_t
{
    RES_4X4 = 16,
    RES_8X8 = 64,
    VL53_NO_ERROR
};

enum class SF_VL53L5CX_POWER_MODE : uint8_t
{
    SLEEP,
    WAKEUP,
    VL53_NO_ERROR
};

enum class SF_VL53L5CX_TARGET_ORDER : uint8_t
{
    CLOSEST,
    STRONGEST,
    VL53_NO_ERROR
};

#endif  // __HID_VL53L5CX_Constants__