/*
  This is a library written for the ST VL53L5CX Time-of-flight sensor
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/18642

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, October 22nd, 2021
  This file implements the VL53L5CX core library class.

  This library uses ST's VL53L5CX driver and parts of Simon Levy's VL53L5CX
  Arduino library available at https://github.com/simondlevy/VL53L5/tree/main/src/st

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include "HID_VL53L5CX.h"
#include "HID_VL53L5CX_IO.h"
#include "vl53l5cx_api.h"
#include <stdexcept>
#include <iostream>

void HID_VL53L5CX::clearErrorStruct()
{
    // Set last error struct to no-error condition
    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::VL53_NO_ERROR;
    lastError.lastErrorValue = 0;
}

// Constructor
//   initialize FT260 DLL, connect to FT260 over USB and initialize the sensor device 
// throws an exception on error
HID_VL53L5CX::HID_VL53L5CX(uint8_t address)
{
    FT260_STATUS ftStatus = FT260_OTHER_ERROR;
    FT260_HANDLE mhandle = INVALID_HANDLE_VALUE;

    clearErrorStruct();

    // create platform device configuration structure
    Dev = new VL53L5CX_Configuration(); 

    Dev->platform.VL53L5CX_i2c = &VL53L5CX_i2c;

    uint8_t result = 0;
    uint8_t deviceId = 0;
    uint8_t revisionId = 0;
    uint8_t isAlive = 0;

    uint8_t i2cstatus = VL53L5CX_i2c.getI2CStatus();
    if (I2CM_IDLE(i2cstatus)) {
        printf("I2C is idle\n");
    }

    /* (Optional) Check if there is a VL53L5CX sensor connected */
    result = vl53l5cx_is_alive(Dev, &isAlive);
    //printf("result = %u, isAlive = %u\n", result, isAlive);
    if (!isAlive || result)
    {
        printf("VL53L5CX sensor was not detected\n");
        throw std::runtime_error("VL53L5CX sensor is not present");
    }

    // See if we can get the current resolution or we need to initialize the sensor
    // the return can be either 4x4 (16) or 8x8 (64)
    uint8_t imageResolution = getResolution();
    if ( (imageResolution != 16) && (imageResolution != 64) ) {
        /* power on sensor and init */
        printf("Initializing VL53L5CX sensor. Downloading firmware, please wait\n");
        result = vl53l5cx_init(Dev);
        printf("vl53l5cx_init() returns: %u", result);
        if (result)
        {
            printf("VL53L5CX ULD Loading failed: %d\n", result);
            throw std::runtime_error("vl53l5cx_init fails: " + std::to_string(result));
        }
    }

    printf("VL53L5CX ULD ready ! (Version : %s)\n", VL53L5CX_API_REVISION);
}

HID_VL53L5CX::~HID_VL53L5CX()
{
    std::cout << "HID_VL53L5CX() destructor called" << std::endl;
    delete(Dev);
}

void HID_VL53L5CX::setErrorCallback(void (*_errorCallback)(SF_VL53L5CX_ERROR_TYPE errorCode, uint32_t errorValue))
{
    errorCallback = _errorCallback;
}

bool HID_VL53L5CX::setRangingFrequency(uint8_t newFrequency)
{
    clearErrorStruct();

    uint8_t result = vl53l5cx_set_ranging_frequency_hz(Dev, newFrequency);
    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_FREQUENCY_SETTING;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

uint8_t HID_VL53L5CX::getRangingFrequency()
{
    clearErrorStruct();

    uint8_t frequency = 0;
    uint8_t result = vl53l5cx_get_ranging_frequency_hz(Dev, &frequency);
    if (result == 0)
    {
        return frequency;
    }
    else
    {
        lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_FREQUENCY_SETTING;
        lastError.lastErrorValue = static_cast<uint32_t>(result);
        SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
        return 0;
    }
}

bool HID_VL53L5CX::setRangingMode(SF_VL53L5CX_RANGING_MODE rangingMode)
{
    clearErrorStruct();

    uint8_t result;

    if (rangingMode == SF_VL53L5CX_RANGING_MODE::AUTONOMOUS)
        result = vl53l5cx_set_ranging_mode(Dev, VL53L5CX_RANGING_MODE_AUTONOMOUS);
    else
        result = vl53l5cx_set_ranging_mode(Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_RANGING_MODE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

SF_VL53L5CX_RANGING_MODE HID_VL53L5CX::getRangingMode()
{
    clearErrorStruct();

    uint8_t rangeMode = 0;
    uint8_t result = vl53l5cx_get_ranging_mode(Dev, &rangeMode);
    if (result == 0)
    {
        if (rangeMode == 0x01)
            return SF_VL53L5CX_RANGING_MODE::CONTINUOUS;
        else
            return SF_VL53L5CX_RANGING_MODE::AUTONOMOUS;
    }

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_RANGING_MODE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return SF_VL53L5CX_RANGING_MODE::VL53_NO_ERROR;
}

bool HID_VL53L5CX::startRanging()
{
    clearErrorStruct();

    uint8_t result = vl53l5cx_start_ranging(Dev);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_START_RANGING;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

bool HID_VL53L5CX::stopRanging()
{
    clearErrorStruct();

    uint8_t result = vl53l5cx_stop_ranging(Dev);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_STOP_RANGING;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

bool HID_VL53L5CX::isDataReady()
{
    clearErrorStruct();
    uint8_t dataReady = 0;

    uint8_t result = vl53l5cx_check_data_ready(Dev, &dataReady);
    if (result == 0)
        return dataReady != 0;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_DATA_READY;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

uint8_t HID_VL53L5CX::getResolution()
{
    clearErrorStruct();

    uint8_t resolution = 0;
    uint8_t result = vl53l5cx_get_resolution(Dev, &resolution);
    if (result == 0)
    {
        if (resolution == 64)
            return (uint8_t)SF_VL53L5CX_RANGING_RESOLUTION::RES_8X8;
        else
            return (uint8_t)SF_VL53L5CX_RANGING_RESOLUTION::RES_4X4;
    }

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_RESOLUTION;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return (uint8_t)SF_VL53L5CX_RANGING_RESOLUTION::VL53_NO_ERROR;
}

bool HID_VL53L5CX::setResolution(uint8_t resolution)
{
    clearErrorStruct();

    uint8_t result = vl53l5cx_set_resolution(Dev, resolution);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_SET_RESOLUTION;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

bool HID_VL53L5CX::getRangingData(VL53L5CX_ResultsData* pRangingData)
{
    clearErrorStruct();

    uint8_t result = vl53l5cx_get_ranging_data(Dev, pRangingData);
    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_RANGING_DATA;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

bool HID_VL53L5CX::setPowerMode(SF_VL53L5CX_POWER_MODE powerMode)
{
    clearErrorStruct();

    uint8_t powerModeValue;
    if (powerMode == SF_VL53L5CX_POWER_MODE::SLEEP)
        powerModeValue = VL53L5CX_POWER_MODE_SLEEP;
    else
        powerModeValue = VL53L5CX_POWER_MODE_WAKEUP;

    uint8_t result = vl53l5cx_set_power_mode(Dev, powerModeValue);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_SET_POWER_MODE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

SF_VL53L5CX_POWER_MODE HID_VL53L5CX::getPowerMode()
{
    clearErrorStruct();

    uint8_t powerModeValue;
    uint8_t result = vl53l5cx_get_power_mode(Dev, &powerModeValue);

    if (result == 0)
    {
        switch (powerModeValue)
        {
        case 0:
            return SF_VL53L5CX_POWER_MODE::SLEEP;
            break;

        case 1:
            return SF_VL53L5CX_POWER_MODE::WAKEUP;
            break;

        default:
            return SF_VL53L5CX_POWER_MODE::VL53_NO_ERROR;
            break;
        }
    }

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_POWER_MODE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return SF_VL53L5CX_POWER_MODE::VL53_NO_ERROR;
}

bool HID_VL53L5CX::setIntegrationTime(uint32_t timeMsec)
{
    clearErrorStruct();

    // Does not allow invalid values to be sent to the device
    if (timeMsec < 2 || timeMsec > 1000)
    {
        lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_INTEGRATION_TIME;
        lastError.lastErrorValue = UNKNOWN_ERROR_VALUE;
        SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
        return false;
    }

    uint8_t result = vl53l5cx_set_integration_time_ms(Dev, timeMsec);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_SET_INTEGRATION_TIME;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

uint32_t HID_VL53L5CX::getIntegrationTime()
{
    clearErrorStruct();
    uint32_t integrationTime;
    uint8_t result = vl53l5cx_get_integration_time_ms(Dev, &integrationTime);

    if (result == 0)
    {
        return integrationTime;
    }

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_INTEGRATION_TIME;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return result;
}

bool HID_VL53L5CX::setSharpenerPercent(uint8_t percent)
{
    clearErrorStruct();

    if (percent > 99)
    {
        lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::INVALID_SHARPENER_VALUE;
        lastError.lastErrorValue = UNKNOWN_ERROR_VALUE;
        SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    }

    uint8_t result = vl53l5cx_set_sharpener_percent(Dev, percent);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_SET_SHARPENER_VALUE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

uint8_t HID_VL53L5CX::getSharpenerPercent()
{
    clearErrorStruct();

    uint8_t percent;
    uint8_t result = vl53l5cx_get_sharpener_percent(Dev, &percent);

    if (result == 0)
        return percent;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_SHARPENER_VALUE;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return 0xff;
}

bool HID_VL53L5CX::setTargetOrder(SF_VL53L5CX_TARGET_ORDER order)
{
    clearErrorStruct();

    uint8_t orderValue;
    if (order == SF_VL53L5CX_TARGET_ORDER::CLOSEST)
        orderValue = VL53L5CX_TARGET_ORDER_CLOSEST;
    else
        orderValue = VL53L5CX_TARGET_ORDER_STRONGEST;

    uint8_t result = vl53l5cx_set_target_order(Dev, orderValue);

    if (result == 0)
        return true;

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_SET_TARGET_ORDER;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return false;
}

SF_VL53L5CX_TARGET_ORDER HID_VL53L5CX::getTargetOrder()
{
    clearErrorStruct();

    uint8_t orderValue;
    uint8_t result = vl53l5cx_get_target_order(Dev, &orderValue);

    if (result == 0)
    {
        if (orderValue == VL53L5CX_TARGET_ORDER_CLOSEST)
            return SF_VL53L5CX_TARGET_ORDER::CLOSEST;
        else
            return SF_VL53L5CX_TARGET_ORDER::STRONGEST;
    }

    lastError.lastErrorCode = SF_VL53L5CX_ERROR_TYPE::CANNOT_GET_TARGET_ORDER;
    lastError.lastErrorValue = static_cast<uint32_t>(result);
    SAFE_CALLBACK(errorCallback, lastError.lastErrorCode, lastError.lastErrorValue);
    return SF_VL53L5CX_TARGET_ORDER::VL53_NO_ERROR;
}
