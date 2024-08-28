using System.Reflection.Metadata.Ecma335;
using System.Runtime.InteropServices;

namespace TOF_Client_donet
{
    internal class Program
    {
        byte I2C_Address = (0x52 >> 1);

        bool runDistanceService()
        {
            bool ret = true;
            double distance = 0;

            #region callDllMethods
            try
            {
                IntPtr sensor = WrapperClass.Instantiate(I2C_Address);

                WrapperClass.startRanging(sensor);
                distance = WrapperClass.getRange(sensor);   // throw away the first entry after turn on 

                for (int i = 0; i < 10; i++)
                {
                    distance = WrapperClass.getRange(sensor);
                    if (distance == 0)
                    {
                        Console.WriteLine("Nothing Present");
                    }
                    else
                    {
                        Console.WriteLine("Avg distance: " + distance + " mm");
                    }
                    string sreply = distance.ToString();

                }
                Console.WriteLine("Loop ends");

                WrapperClass.Conclude(sensor);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception: " + ex.Message);
                ret = false;
                distance = 0;
            }
            #endregion

            return ret;
        }


        static void Main(string[] args)
        {
            Console.WriteLine("TOF Sensor start");
            Program program = new Program();

            while (true)
            {
                Console.WriteLine("Start Sensor Polling");

                if (!program.runDistanceService())
                {
                    Console.WriteLine("Disance servicce complete");
                    break;
                    Thread.Sleep(1000);
                }
            }

            Console.WriteLine("End of ULD demo");
        }
    }
}
