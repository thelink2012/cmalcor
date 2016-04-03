using System;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace CmAlcorGUI
{
    public class FirmwareException : Exception
    {
        public FirmwareException(int error)
            : base(String.Format("Firmware call failed with error code: {0}.", error))
        {
        }
    }
    
    /// <summary>
    /// Communication with the CM Storm Alcor Firmware.
    /// 
    /// This is a wrapper around the unmanaged cmalcor.dll code.
    /// </summary>
    public class Firmware
    {
        public enum LedMode
        {
            AlwaysOff = 0,
            AlwaysOn  = 1,
            RapidFire = 2,
            Breathing = 3,
        }

        /// <summary>
        /// Gets the version of the unmanaged library (cmalcor.dll) that we're using to communicate with the firmware.
        /// </summary>
        public static int LibraryVersion()
        {
            return CmAlcor_LibraryVersion();
        }

        /// <summary>
        /// Checks whether the mouse is connected on the computer.
        /// </summary>
        public static bool IsAlcorPresent()
        {
            return CmAlcor_IsMousePresent() != 0;
        }

        /// <summary>
        /// Gets the version of the firmware currently flashed on the mouse.
        /// </summary>
        /// <exception cref="FirmwareException"></exception>
        public static int GetVersion()
        {
            int error;
            int version = CmAlcor_GetFirmwareVersion(out error);
            if(error != 0) throw new FirmwareException(error);
            return version;
        }

        /// <summary>
        /// Flashes a new configuration into the mouse with the settings in the parameters.
        /// </summary>
        /// <param name="brightness">Between 0 and 10 (inclusive).</param>
        /// <exception cref="FirmwareException"></exception>
        public static void SetLED(LedMode mode, int brightness, byte red, byte green, byte blue)
        {
            int error;
            CmAlcor_SetLED(out error, (int) mode, brightness, red, green, blue);
            if(error != 0) throw new FirmwareException(error);
        }

        /// <summary>
        /// SetLED(...) may block because erasing and programming flash memory is slow.
        /// Thus, there's an async version of it.
        /// 
        /// See SetLED(...) for details.
        /// </summary>
        /// <exception cref="FirmwareException"></exception>
        public static async Task SetLEDAsync(LedMode mode, int brightness, byte red, byte green, byte blue)
        {
            await Task.Run(() => SetLED(mode, brightness, red, green, blue));
        }

        /// <summary>
        /// Gets the LED configuration currently flashed on the mouse.
        /// Does no change to the ref parameters when no configuration is flashed on the device.
        /// </summary>
        /// <returns>Whether there's any LED configuration flashed on the device.</returns>
        /// <exception cref="FirmwareException"></exception>
        public static bool GetLED(ref LedMode mode, ref int brightness, ref byte red, ref byte green, ref byte blue)
        {
            int error, currMode, currBright, currRed, currGreen, currBlue;
            if(CmAlcor_GetLED(out error, out currMode, out currBright, out currRed, out currGreen, out currBlue) != 0)
            {
                mode = (LedMode) currMode;
                brightness = currBright;
                red = (byte)(currRed & 0xFF);
                green = (byte)(currGreen & 0xFF);
                blue = (byte)(currBlue & 0xFF);
                return true;
            }
            else
            {
                if(error != 0) throw new FirmwareException(error);
                return false;
            }
        }

        /// <summary>
        /// Enables the use of custom LED configurations flashed on the mouse.
        /// </summary>
        /// <exception cref="FirmwareException"></exception>
        public static void EnableCustomLED()
        {
            int error;
            CmAlcor_EnableCustomLED(out error);
            if(error != 0) throw new FirmwareException(error);
        }

        /// <summary>
        /// Disables the use of custom LED configurations flashed on the mouse.
        /// </summary>
        /// <exception cref="FirmwareException"></exception>
        public static void DisableCustomLED()
        {
            int error;
            CmAlcor_DisableCustomLED(out error);
            if(error != 0) throw new FirmwareException(error);
        }




        #region Unmanaged Code

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CmAlcor_LibraryVersion();

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CmAlcor_IsMousePresent();

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CmAlcor_GetFirmwareVersion(out int error);

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CmAlcor_SetLED(out int error, int mode, int brightness, int red, int green, int blue);

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CmAlcor_GetLED(out int error, out int mode, out int brightness, out int red, out int green, out int blue);

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int  CmAlcor_EnableCustomLED(out int error);

        [DllImport("cmalcor.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int  CmAlcor_DisableCustomLED(out int error);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int Delegate_CmAlcor_LibraryVersion();

        #endregion
    }
}
