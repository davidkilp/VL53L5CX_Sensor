#pragma once
/*
  This is a library written for the ST VL53L5CX Time-of-flight sensor
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/18642

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, October 22nd, 2021
  This file declares the VL53L5CX I2C driver class.

  This library uses ST's VL53L5CX driver and parts of Simon Levy's VL53L5CX
  Arduino library available at https://github.com/simondlevy/VL53L5/tree/main/src/st

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __HID_VL53L5CX_IO__
#define __HID_VL53L5CX_IO__


#include "LibFT260.h"
#include "HID_VL53L5CX_Constants.h"

class HID_VL53L5CX_IO
{
private:
	// I2C instance
	FT260_HANDLE _handle;

	// Sensor address
	uint8_t _address;

public:
	//  constructor needs to know the device I2C address will create FT260 handle
	HID_VL53L5CX_IO(uint8_t address);

	// destructor needs to close the handle and clean up
	~HID_VL53L5CX_IO();

	uint8_t getI2CStatus();

	const char* FT260StatusToString(FT260_STATUS status);

	// Read a single byte from a register.
	uint8_t readSingleByte(uint16_t registerAddress, uint8_t &value);

	// Write a single byte into a register.
	uint8_t writeSingleByte(uint16_t registerAddress, uint8_t value);

	// Read multiple bytes from a register into buffer byte array.
	uint8_t readMultipleBytes(uint16_t registerAddress, uint8_t* buffer, uint16_t bufferSize);

	// Write multiple bytes to register from buffer byte array.
	uint8_t writeMultipleBytes(uint16_t registerAddress, uint8_t* buffer, uint16_t bufferSize);

};

#endif // __HID_VL53L5CX_IO__
