/*
  This is a library written for the ST VL53L5CX Time-of-flight sensor
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/18642

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, October 22nd, 2021
  This file implements the VL53L5CX I2C driver class.

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
#include "HID_VL53L5CX_IO.h"
#include "HID_VL53L5CX_Constants.h"
#include <stdexcept>
#include <string>
#include <iostream>

//#define DEBUG	1
#ifdef DEBUG
#define D(x)   x
#else
#define D(x)
#endif

#define highByte(x)		( ((x) >> (8)) & 0xFF )
#define lowByte(x)		( (x) & 0xFF )


HID_VL53L5CX_IO::HID_VL53L5CX_IO(uint8_t address)
{
    FT260_STATUS ftStatus = FT260_OTHER_ERROR;
    FT260_HANDLE mhandle = INVALID_HANDLE_VALUE;
    
    std::cout << "HID_VL53L5CX_IO() constructor" << std::endl;
    _address = address;

    // Open device by VID/PID
    ftStatus = FT260_OpenByVidPid(FT260_Vid, FT260_Pid, 0, &mhandle);
    if (FT260_OK != ftStatus)
    {
        //printf("Open device by VID/PID failed: %s\n", FT260StatusToString(ftStatus));
        std::string error(FT260StatusToString(ftStatus));
        std::cerr << "FT260_OpenByVidPid fails: " << error << std::endl;
        throw std::runtime_error("FT260 Open fails error: " + error);
    }

    _handle = mhandle;  // save the handle

    //Initialize as an I2C Master, read/write to an I2C slave
    ftStatus = FT260_I2CMaster_Init(mhandle, I2C_100KHZ);
    if (ftStatus != FT260_OK) {
        printf("FT260_I2CMaster_Init() returns: %d\n", ftStatus);
        std::string error(FT260StatusToString(ftStatus));
        throw std::runtime_error("FT260_I2CMaster_Init fails error: " + error);
    }

}

HID_VL53L5CX_IO::~HID_VL53L5CX_IO()
{
    D(std::cout << "HID_VL53L5CX_IO() destructor called" << std::endl; )
    std::cout << "HID_VL53L5CX_IO() destructor called" << std::endl;
    FT260_Close(_handle);
}

uint8_t HID_VL53L5CX_IO::getI2CStatus()
{
    uint8_t status = 0;
    FT260_STATUS ftStatus = FT260_OTHER_ERROR;
    ftStatus = FT260_I2CMaster_GetStatus(_handle, &status);
    return status;
}

const char* HID_VL53L5CX_IO::FT260StatusToString(FT260_STATUS status)
{
    switch (status)
    {
    case  0:
        return sFT260Status[0].c_str();
    case  1:
        return sFT260Status[1].c_str();
    case  2:
        return sFT260Status[2].c_str();
    case  3:
        return sFT260Status[3].c_str();
    case  4:
        return sFT260Status[4].c_str();
    case  5:
        return sFT260Status[5].c_str();
    case  6:
        return sFT260Status[6].c_str();
    case  7:
        return sFT260Status[7].c_str();
    case  8:
        return sFT260Status[8].c_str();
    case  9:
        return sFT260Status[9].c_str();
    case 10:
        return sFT260Status[10].c_str();
    case 11:
        return sFT260Status[11].c_str();
    case 12:
        return sFT260Status[12].c_str();
    case 13:
        return sFT260Status[13].c_str();
    case 14:
        return sFT260Status[14].c_str();
    case 15:
        return sFT260Status[15].c_str();
    case 16:
        return sFT260Status[16].c_str();
    case 17:
        return sFT260Status[17].c_str();
    default:
        return "Not a valid FT260 status";
    }
}

// Must be able to write 32,768 bytes at a time
uint8_t HID_VL53L5CX_IO::writeMultipleBytes(uint16_t registerAddress, uint8_t* buffer, uint16_t bufferSize)
{
    FT260_STATUS ftStatus = FT260_OK;
    DWORD writeLength = 0;
    DWORD numBytesToWrite = 0;
    uint32_t bytesToSend = bufferSize;
    uint8_t adrbuffer[3] = {};

    D(printf("Write %u bytes starting at address: 0x%04X\n", bytesToSend, registerAddress); )

    // first write register address
    numBytesToWrite = sizeof(uint16_t);
    adrbuffer[0] = highByte(registerAddress);
    adrbuffer[1] = lowByte(registerAddress);
    ftStatus = FT260_I2CMaster_Write(_handle, _address, FT260_I2C_START, adrbuffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write setting register fails: %d\n", ftStatus);
        return ftStatus;
    }

    // followed by the data 
    numBytesToWrite = bufferSize;
    ftStatus = FT260_I2CMaster_Write(_handle, _address, FT260_I2C_STOP, buffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write data write multiple fails: %d wrote: %d\n", ftStatus, writeLength);
        return ftStatus;
    }

    return ftStatus;
}

uint8_t HID_VL53L5CX_IO::readMultipleBytes(uint16_t registerAddress, uint8_t* buffer, uint16_t bufferSize)
{
    FT260_STATUS ftStatus = FT260_OK;
    DWORD readLength = 0;
    DWORD writeLength = 0;
    DWORD numBytesToRead = 0;
    DWORD numBytesToWrite = 0;
    uint8_t adrbuffer[3] = {};

    D(printf("Reading %u bytes starting at address: 0x%04X\n", bufferSize, registerAddress); )

    // first write register to read
    numBytesToWrite = sizeof(uint16_t);
    adrbuffer[0] = highByte(registerAddress);
    adrbuffer[1] = lowByte(registerAddress);
    ftStatus = FT260_I2CMaster_Write(_handle, _address, FT260_I2C_START_AND_STOP, adrbuffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write setting register fails: %d\n", ftStatus);
        return ftStatus;
    }

    // Now lets read the byte specified
    numBytesToRead = bufferSize;
    ftStatus = FT260_I2CMaster_Read(_handle, _address, FT260_I2C_START_AND_STOP, buffer, numBytesToRead, &readLength, 5000);
    D(printf("FT260_I2C_Read  ftStatus : % d  Read Length : %d\n", ftStatus, readLength); )

        return ftStatus;
}

uint8_t HID_VL53L5CX_IO::readSingleByte(uint16_t registerAddress, uint8_t& value)
{
    FT260_STATUS ftStatus = FT260_OK;
    DWORD readLength = 0;
    DWORD writeLength = 0;
    DWORD numBytesToRead = 0;
    DWORD numBytesToWrite = 0;
    uint8_t buffer[3] = {};

    D(printf("Read 1 byte at address: 0x%04X\n", registerAddress); )

    // first write register to read
    numBytesToWrite = sizeof(uint16_t);
    buffer[0] = highByte(registerAddress);	// (RegisterAdress >> 8) & 0xFF;
    buffer[1] = lowByte(registerAddress);	//(RegisterAdress & 0xFF);
    ftStatus = FT260_I2CMaster_Write(_handle, _address, FT260_I2C_START_AND_STOP, buffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write setting register fails: %d\n", ftStatus);
        return ftStatus;
    }

    // Now lets read the byte specified
    numBytesToRead = 1;
    value = 0;
    ftStatus = FT260_I2CMaster_Read(_handle, _address, FT260_I2C_START_AND_STOP, buffer, numBytesToRead, &readLength, 5000);
    if ((ftStatus != FT260_OK) || (readLength != numBytesToRead))
    {
        printf("FT260_I2CMaster_Read() fails: %d\n", ftStatus);
        return ftStatus;
    }
    value = buffer[0];
    D(printf("FT260_I2C_Read  ftStatus : % d  Read Length : %d, value: 0x%02X\n", ftStatus, readLength, value); )

    return ftStatus;
}

uint8_t HID_VL53L5CX_IO::writeSingleByte(uint16_t registerAddress, uint8_t const value)
{
    FT260_STATUS ftStatus = FT260_OK;
    DWORD writeLength = 0;
    DWORD numBytesToWrite = 0;
    uint8_t buffer[3] = {};

    D(printf("Write 1 byte (0x%02X) at address: 0x%04X\n", value, registerAddress); )

    // first write register to write
    buffer[0] = highByte(registerAddress);
    buffer[1] = lowByte(registerAddress);
    buffer[2] = value;
    numBytesToWrite = 3;
    ftStatus = FT260_I2CMaster_Write(_handle, _address, FT260_I2C_START_AND_STOP, buffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write setting register fails: %d\n", ftStatus);
        return ftStatus;
    }

    return ftStatus;
}
