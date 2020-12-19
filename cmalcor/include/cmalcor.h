#ifndef CMALCOR_CAPI_H_
#define CMALCOR_CAPI_H_
#pragma once
#include <cstdint>
#include <cstring> // std::memset/std::memcpy/size_t

// Uses C99/C++ comments, can't bother to replace them.

#if defined(CMALCOR_COMPILING) || defined(CMALCOR_COMPILING_CLI)
#define CMALCOR_CAPI_VERSION            0x010101
#define CMALCOR_CAPI_VERSION_STRING     "1.1.1"
#endif

/*
 * Define CMALCOR_API based on platform/compiler
 */
#ifdef _WIN32
#   ifdef CMALCOR_STATIC
#       define CMALCOR_API
#   else
#       ifdef CMALCOR_COMPILING
#           define CMALCOR_API __declspec(dllexport)
#       else
#           define CMALCOR_API __declspec(dllimport)
#       endif
#   endif
#else
#   define CMALCOR_API
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*
 * LED Modes
 */
#define CMALCOR_LEDMODE_ALWAYSOFF       (0)
#define CMALCOR_LEDMODE_ALWAYSON        (1)
#define CMALCOR_LEDMODE_RAPIDFIRE       (2)
#define CMALCOR_LEDMODE_BREATHING       (3)

/*
 * Errors
 */
/// No error.
#define CMALCOR_ERROR_NOERROR           (0)
/// Unknown error.
#define CMALCOR_ERROR_UNKNOWN           (-1)
/// An invalid parameter was passed to the function.
#define CMALCOR_ERROR_INVALIDPARAM      (-2)
/// Device is not connected on the system.
#define CMALCOR_ERROR_NODEVICE          (-3)
/// Device is currently locked (doing some other firmware operation) by someone else.
#define CMALCOR_ERROR_DEVICELOCKED      (-4)
/// Device firmware IO is in a bad state.
#define CMALCOR_ERROR_BADSTATE          (-5)
/// Generic communication error with the device.
#define CMALCOR_ERROR_IOERROR           (-6)
/// Failed to trigger the unsafe operations flag.
#define CMALCOR_ERROR_BADUNSAFEGUARD    (-7)
/// Failed to flash erase the device.
#define CMALCOR_ERROR_BADFLASHERASE     (-8)
/// Failed to program something into the device memory.
#define CMALCOR_ERROR_BADFLASHPROGRAM   (-9)
/// Failed to retrieve the checksum, from the device, for a write operation.
#define CMALCOR_ERROR_BADFLASHCHECKSUM  (-10)
/// Checksum mismatch between the device and the host.
#define CMALCOR_ERROR_BADCHECKSUM (-11)
/// Failed to tell the device about the success of a flash programming operation.
#define CMALCOR_ERROR_BADFLASHTELL      (-12)
/// Failed to read from the device memory.
#define CMALCOR_ERROR_BADFLASHREAD      (-13)
/// Couldn't complete operation because there's no LED configuration programmed on the device.
#define CMALCOR_ERROR_NOLEDCONFIG       (-14)
/// The settings on the mouse are ill-formed (most likely offsets out of range).
#define CMALCOR_ERROR_BADSETTING        (-15)


/*
 * Library Compilation Flags
 */
#define CMALCOR_LIBFLAGS_DEBUG          (0x1)
#define CMALCOR_LIBFLAGS_MIZARPATCH     (0x4)


/*
 * Functions
 */

///
/// Gets the current version of this library.
///
/// The version is a 32 bit value 0x00AABBCC, where AA is the major version, BB the minor and CC the revision.
///
CMALCOR_API
int CmAlcor_LibraryVersion();

///
/// Gets the compilation flags of this library.
///
/// See `CMALCOR_LIBFLAGS_*`.
///
CMALCOR_API
int CmAlcor_LibraryFlags();

///
/// Transforms a CMALCOR_ERROR_* into a string.
/// 
/// If `buffer` is null, returns the number of bytes required to store the string (including null terminator).
/// If `buffer` isn't null, copies the message string into it and returns the number of bytes copied.
///
/// Returns 0 either if the buffer is too small or if `error` isn't a error constant.
///
CMALCOR_API
int CmAlcor_ErrorToString(int error, char* buffer, size_t max_size);

///
/// Returns 1 if the CM Storm Alcor mice is connected on the system, 0 otherwise.
///
CMALCOR_API
int CmAlcor_IsMousePresent();

///
/// Gets the version of the firmware in the mouse.
///
/// The version is a 16 bit value 0xABCD, where A is the major version, B is the minor, C the revision and D seems to be always 0.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
///
CMALCOR_API
int CmAlcor_GetFirmwareVersion(int* error);

///
/// Reads range `begin`-`end` (inclusive) from the memory of the mouse into `output`.
/// Do note that `begin` must be 4 byte aligned and `end` 3 byte aligned (due to it being inclusive).
///
/// This is able to read the flash memory, the SRAM memory, and even the AMBA and PPB memory region.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
/// The function additionally returns 1 on success and 0 on failure.
///
CMALCOR_API
int CmAlcor_MemoryRead(int* error, uint32_t begin, uint32_t end_inclusive, void* output);

///
/// Erases the flash memory in range `begin`-`end` (inclusive).
/// Do note that `begin` must be 4 byte aligned and `end` 3 byte aligned (due to it being inclusive).
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
/// The function additionally returns 1 on success and 0 on failure.
///
CMALCOR_API
int CmAlcor_FlashErasePages(int* error, uint32_t begin, uint32_t end_inclusive);

///
/// Enables the LED configuration currently programmed on memory (most likely previously set with `CmAlcor_SetLED`).
///
/// It's an error to call this without any setting programmed, the function will fail with `CMALCOR_ERROR_NOLEDCONFIG`.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
/// The function additionally returns 1 on success and 0 on failure.
///
CMALCOR_API
int CmAlcor_EnableCustomLED(int* error);

///
/// Disables the LED configuration currently programmed on memory, going back to the default behaviour.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
/// The function additionally returns 1 on success and 0 on failure.
///
CMALCOR_API
int CmAlcor_DisableCustomLED(int* error);

///
/// Setups a LED configuration into the mouse memory.
///
/// For `mode` values, see `CMALCOR_LEDMODE_*`.
/// The `brightness` value must be in the 0-10 (inclusive) range.
/// The `red`, `green` and `blue` values must be in the 0-255 (inclusive) range.
///
/// This does not enable the LED configuration, use `CmAlcor_EnableCustomLED` for that.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
/// The function additionally returns 1 on success and 0 on failure.
///
CMALCOR_API
int CmAlcor_SetLED(int* error, int mode, int brightness, int red, int green, int blue);

///
/// Reads the LED configuration currently programmed on the mouse memory.
/// Any of the output parameters may be a null pointer to not retrieve such setting.
/// 
/// See `CmAlcor_SetLED` for the range of the output values.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
///
/// The function returns 1 on success retrieving the settings, and 0 if no setting is programmed on the device OR on failure.
/// For the **return 0** case, the `error` variable will have `CMALCOR_ERROR_NOERROR` if no setting is programmed on the device.
///
CMALCOR_API
int CmAlcor_GetLED(int* error, int* mode, int* brightness, int* red, int* green, int* blue);

/// Returns 1 if there's any setting programmed on the mouse memory, 0 otherwise.
///
/// The `error` variable is set, if not null, to a `CMALCOR_ERROR_*` value to specify the success/failure of the function.
///
CMALCOR_API
int CmAlcor_HasLEDConfig(int* error);


} // extern "C"


#endif
