#include <cmalcor.h>
#include "device/io_alcor.hpp"
#include "default_settings.hpp"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" CMALCOR_API
void CALLBACK CmAlcor_EpEnableCustomLED(HWND, HINSTANCE, LPCSTR, int);

#endif

// Address of the settings in the mouse flash memory (not complete, only necessary area).
static const uintptr_t addr_settings_begin = 0xD800;
static const uintptr_t addr_settings_end   = 0xD8FF;

// Double check!
static_assert(addr_settings_begin == 0xD700+0x100, "");
static_assert(addr_settings_end == 0xD700+0x200-1, "");

/*
 * Helpers
 *
 */

static inline void set_error(int* error, int value)
{
    if(error) *error = value;
}

static inline void clear_error(int* error)
{
    set_error(error, CMALCOR_ERROR_NOERROR);
}

static IoAlcorFirmware GetFirmware(int* error, bool safety_checks = true)
{
    auto device = HidDevice::ScanForDevice(0x2516, 0x2D);
    if(device)
    {
        IoAlcorFirmware firmware(device);
        if(firmware)
        {
            uint32_t num2;
            
            // safety check is useful to verify if the mouse isn't in a bad state (such as expecting MemoryRead/FlashWrite IO).
            if(!safety_checks || (firmware.GetNumber2(num2) && num2 == 2))
            {
                return firmware;
            }
            else
                set_error(error, CMALCOR_ERROR_BADSTATE);
        }
        else
            set_error(error, CMALCOR_ERROR_DEVICELOCKED);
    }
    else
        set_error(error, CMALCOR_ERROR_NODEVICE);

    return IoAlcorFirmware();
}


/*
 * C API
 *
 *  For documentation see <cmalcor.h>
 *
 */

CMALCOR_API
int CmAlcor_LibraryVersion()
{
    return CMALCOR_CAPI_VERSION;
}

CMALCOR_API
int CmAlcor_IsMousePresent()
{
    return HidDevice::ScanForDevice(0x2516, 0x2D)? 1 : 0;
}

CMALCOR_API
int CmAlcor_GetFirmwareVersion(int* error)
{
    uint16_t version;
    clear_error(error);

    if(auto alcor = GetFirmware(error, false)) // false is ok on GetFirmwareVersion
    {
        if(alcor.GetVersion(version))
            return version;
        else
            set_error(error, CMALCOR_ERROR_IOERROR);
    }
    return 0;
}

CMALCOR_API
int CmAlcor_MemoryRead(int* error, uint32_t begin, uint32_t end_inclusive, void* output)
{
    const auto& end = end_inclusive;
    clear_error(error);
    
    if(begin > end || output == nullptr || (begin % 4) != 0 || (end % 4) != 3)
    {
        set_error(error, CMALCOR_ERROR_INVALIDPARAM);
        return 0;
    }

    if(auto alcor = GetFirmware(error))
    {
        IoAlcorFirmware::UnsafeGuard guard(alcor);
        if(guard)
        {
            if(alcor.MemoryRead(begin, end, output))
            {
                return 1;
            }
            else
                set_error(error, CMALCOR_ERROR_BADFLASHREAD);
        }
        else
            set_error(error, CMALCOR_ERROR_BADUNSAFEGUARD);
    }

    return 0;
}

CMALCOR_API
int CmAlcor_FlashErasePages(int* error, uint32_t begin, uint32_t end_inclusive)
{
    const auto& end = end_inclusive;
    clear_error(error);

    if(begin > end || (begin % 4) != 0 || (end % 4) != 3)
    {
        set_error(error, CMALCOR_ERROR_INVALIDPARAM);
        return 0;
    }

    if(auto alcor = GetFirmware(error))
    {
        IoAlcorFirmware::UnsafeGuard guard(alcor);
        if(guard)
        {
            if(alcor.FlashErasePages(begin, end))
            {
                return 1;
            }
            else
                set_error(error, CMALCOR_ERROR_BADFLASHERASE);
        }
        else
            set_error(error, CMALCOR_ERROR_BADUNSAFEGUARD);
    }

    return 0;
}

CMALCOR_API
int CmAlcor_EnableCustomLED(int* error)
{
    clear_error(error);

    if(CmAlcor_HasLEDConfig(error))
    {
        if(auto alcor = GetFirmware(error))
        {
            if(alcor.DoUnk84_0() && alcor.DoUnk82())
            {
                return 1;
            }
            else
                set_error(error, CMALCOR_ERROR_IOERROR);
        }
    }
    else
    {
        if(error && *error == CMALCOR_ERROR_NOERROR)
        {
            set_error(error, CMALCOR_ERROR_NOLEDCONFIG);
        }
    }

    return 0;
}

CMALCOR_API
int CmAlcor_DisableCustomLED(int* error)
{
    clear_error(error);

    if(auto alcor = GetFirmware(error))
    {
        if(alcor.DoUnk82(false) && alcor.DoUnk84_1())
            return 1;
        else
            set_error(error, CMALCOR_ERROR_IOERROR);
    }

    return 0;
}

CMALCOR_API
int CmAlcor_SetLED(int* error, int mode, int brightness, int red, int green, int blue)
{
    clear_error(error);

    if(!(mode >= 0 && mode <= 3)
    || !(brightness >= 0 && brightness <= 10)
    || !(red >= 0 && red <= 255)
    || !(green >= 0 && green <= 255)
    || !(blue >= 0 && blue <= 255))
    {
        set_error(error, CMALCOR_ERROR_INVALIDPARAM);
        return 0;
    }

    if(auto alcor = GetFirmware(error))
    {
        uint8_t settings[256];
        uint16_t host_checksum, device_checksum;
        std::copy(std::begin(default_settings), std::end(default_settings), settings);

        settings[0x70 + 0] = uint8_t(mode & 0x0F) | uint8_t((brightness & 0x0F) << 4);
        settings[0x70 + 1] = uint8_t(red);
        settings[0x70 + 2] = uint8_t(green);
        settings[0x70 + 3] = uint8_t(blue);

        IoAlcorFirmware::UnsafeGuard guard(alcor);
        if(guard)
        {
            if(alcor.FlashErasePages(addr_settings_begin, addr_settings_end))
            {
                static_assert(sizeof(settings) == (addr_settings_end - addr_settings_begin) + 1, "");
                if(alcor.FlashProgram(addr_settings_begin, addr_settings_end, settings, host_checksum))
                {
                    if(alcor.Checksum(addr_settings_begin, addr_settings_end, device_checksum))
                    {
                        if(device_checksum == host_checksum)
                        {
                            if(alcor.FlashTellSuccessProgramming())
                            {
                                return 1;
                            }
                            else
                                set_error(error, CMALCOR_ERROR_BADFLASHTELL);
                        }
                        else
                            set_error(error, CMALCOR_ERROR_BADCHECKSUM);
                    }
                    else
                        set_error(error, CMALCOR_ERROR_BADFLASHCHECKSUM);
                }
                else
                    set_error(error, CMALCOR_ERROR_BADFLASHPROGRAM);
            }
            else
                set_error(error, CMALCOR_ERROR_BADFLASHERASE);
        }
        else
            set_error(error, CMALCOR_ERROR_BADUNSAFEGUARD);
    }

    return 0;
}

CMALCOR_API
int CmAlcor_GetLED(int* error, int* mode, int* brightness, int* red, int* green, int* blue)
{
    clear_error(error);

    if(auto alcor = GetFirmware(error))
    {
        IoAlcorFirmware::UnsafeGuard guard(alcor);
        if(guard)
        {
            uint8_t settings[256];

            static_assert(sizeof(settings) == (addr_settings_end - addr_settings_begin) + 1, "");
            if(alcor.MemoryRead(addr_settings_begin, addr_settings_end, settings))
            {
                if(settings[0] == 0xFF && settings[1] == 0xFF && settings[2] == 0xA5 && settings[3] == 0xA5)
                {
                    size_t offset = uint16_t(settings[0x12]) | uint16_t((settings[0x13] << 8) & 0xFF00);
                    
                    if(offset + 3 < sizeof(settings))
                    {
                        if(mode)
                        {
                            *mode = settings[offset + 0] & 0x0F;
                            *mode = (*mode >= 0 && *mode <= 3? *mode : 1);
                        }

                        if(brightness)
                        {
                            *brightness = (settings[offset + 0] >> 4) & 0x0F;
                            *brightness = (*brightness >= 0 && *brightness <= 10? *brightness : 10);
                        }
                        
                        if(red) *red = settings[offset + 1];
                        if(green) *green = settings[offset + 2];
                        if(blue) *blue = settings[offset + 3];

                        return 1;
                    }
                    else
                        set_error(error, CMALCOR_ERROR_BADSETTING);
                }
                else
                {
                    // this is not a error, but there's no custom led configuration flashed on the device.
                    return 0;
                }
            }
            else
                set_error(error, CMALCOR_ERROR_BADFLASHREAD);
        }
        else
            set_error(error, CMALCOR_ERROR_BADUNSAFEGUARD);
    }

    return 0;
}

CMALCOR_API
int CmAlcor_HasLEDConfig(int* error)
{
    clear_error(error);

    if(auto alcor = GetFirmware(error))
    {
        IoAlcorFirmware::UnsafeGuard guard(alcor);
        if(guard)
        {
            uint8_t magic[4];
            if(alcor.MemoryRead(addr_settings_begin, addr_settings_begin + 3, magic))
            {
                return (magic[0] == 0xFF && magic[1] == 0xFF && magic[2] == 0xA5 && magic[3] == 0xA5);
            }
            else
                set_error(error, CMALCOR_ERROR_BADFLASHREAD);
        }
        else
            set_error(error, CMALCOR_ERROR_BADUNSAFEGUARD);
    }

    return 0;
}

#ifdef _WIN32
CMALCOR_API
void CALLBACK CmAlcor_EpEnableCustomLED(HWND, HINSTANCE, LPCSTR, int)
{
    CmAlcor_EnableCustomLED(nullptr);
}
#endif


template<size_t N>
static constexpr std::pair<const char*, size_t> SzArray(const char (&p)[N])
{
    return std::make_pair(p, N);
}

CMALCOR_API
int CmAlcor_ErrorToString(int error, char* buffer, size_t max_size)
{
    // find: \/\/\/ (.*)\n#define ([_A-Z0-9]+).*
    // replace: case $2:\n    msg = "$1";\n    break;

    std::pair<const char*, size_t> msg(nullptr, 0);

    switch(error)
    {
        case CMALCOR_ERROR_NOERROR:
            msg = SzArray("No error.");
            break;
        case CMALCOR_ERROR_UNKNOWN:
            msg = SzArray("Unknown error.");
            break;
        case CMALCOR_ERROR_INVALIDPARAM:
            msg = SzArray("An invalid parameter was passed to the function.");
            break;
        case CMALCOR_ERROR_NODEVICE:
            msg = SzArray("Device is not connected on the system.");
            break;
        case CMALCOR_ERROR_DEVICELOCKED:
            msg = SzArray("Device is currently locked (doing some other firmware operation) by someone else.");
            break;
        case CMALCOR_ERROR_BADSTATE:
            msg = SzArray("Device firmware IO is in a bad state.");
            break;
        case CMALCOR_ERROR_IOERROR:
            msg = SzArray("Generic communication error with the device.");
            break;
        case CMALCOR_ERROR_BADUNSAFEGUARD:
            msg = SzArray("Failed to trigger the unsafe operations flag.");
            break;
        case CMALCOR_ERROR_BADFLASHERASE:
            msg = SzArray("Failed to flash erase the device.");
            break;
        case CMALCOR_ERROR_BADFLASHPROGRAM:
            msg = SzArray("Failed to program something into the device memory.");
            break;
        case CMALCOR_ERROR_BADFLASHCHECKSUM:
            msg = SzArray("Failed to retrieve the checksum, from the device, for a write operation.");
            break;
        case CMALCOR_ERROR_BADCHECKSUM:
            msg = SzArray("Checksum mismatch between the device and the host.");
            break;
        case CMALCOR_ERROR_BADFLASHTELL:
            msg = SzArray("Failed to tell the device about the success of a flash programming operation.");
            break;
        case CMALCOR_ERROR_BADFLASHREAD:
            msg = SzArray("Failed to read from the device memory.");
            break;
        case CMALCOR_ERROR_NOLEDCONFIG:
            msg = SzArray("Couldn't complete operation because there's no LED configuration programmed on the device.");
            break;
        case CMALCOR_ERROR_BADSETTING:
            msg = SzArray("The settings on the mouse are ill-formed (most likely offsets out of range).");
            break;
    }

    if(msg.first)
    {
        if(buffer == nullptr)
        {
            return msg.second;
        }
        else
        {
            if(msg.second < max_size)
            {
                std::memcpy(buffer, msg.first, msg.second); // includes null terminator
                return (int) msg.second;
            }
        }
    }

    return 0;
}
