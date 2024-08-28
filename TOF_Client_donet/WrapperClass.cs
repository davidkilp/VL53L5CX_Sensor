using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TOF_Client_donet
{
    public static class WrapperClass
    {
        #region dllImports
        // Target your c++ dll here -- add project reference to this dll in C# project also
        //private const string _dllImportPath = @"..\..\..\..\x64\Debug\VL53L5CX_Sensor.dll";
        private const string _dllImportPath = @"VL53L5CX_Sensor.dll";

        // Declare the c++ function(s) you want to call using the DllImport attribute.
        //extern "C" SENSOR_API void* Instantiate(uint8_t i2c_address);
        // Specify the name of the c++ DLL and the calling convention (CallingConvention.Cdecl for C++).
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Instantiate(byte i2c_address);


        //extern "C" SENSOR_API void Conclude(VL53L5CXSensor* t);
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Conclude(IntPtr t);

        //extern "C" SENSOR_API bool startRanging(VL53L5CXSensor* t);
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool startRanging(IntPtr t);

        //extern "C" SENSOR_API bool stopRanging(VL53L5CXSensor* t);
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool stopRanging(IntPtr t);

        //extern "C" SENSOR_API bool isDataReady(VL53L5CXSensor* t);
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool isDataReady(IntPtr t);

        //extern "C" SENSOR_API double getRange(VL53L5CXSensor* t);
        [DllImport(_dllImportPath, CallingConvention = CallingConvention.Cdecl)]
        public static extern double getRange(IntPtr t);

        #endregion

    }
}
