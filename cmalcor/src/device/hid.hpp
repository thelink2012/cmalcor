#pragma once
#include <hidapi/hidapi.h>
#include <cassert>
#include <cstdint>
#include <string>

///
/// Represents the identification for a HID Device.
///
class HidDevice
{
public:
    class IoHandle;

    HidDevice() = default;
    HidDevice(const HidDevice&) = delete;
    HidDevice(HidDevice&&) = default;
    HidDevice& operator=(HidDevice&&) = default;
    HidDevice& operator=(const HidDevice&) = delete;

    /// Whether this object represents a device.
    explicit operator bool() const
    {
        return !device.empty();
    }

    /// Locks the device access (but only for users of this library) and returns a I/O handle to it.
    IoHandle Open() const;

    /// Finds a HID Device with the specified vendor and product id connected on the system.
    static HidDevice ScanForDevice(uint16_t vendor, uint16_t product);

private:
    std::string device;
    uint16_t vendor, product;

    explicit HidDevice(std::string device_path, uint16_t vendor, uint16_t product) :
        device(std::move(device_path)), vendor(vendor), product(product)
    {
        assert(!!*this);
    }

public:
    /// Open device ready for I/O.
    class IoHandle
    {
    public:
        IoHandle() : hDev(0), hLock(0)
        {}

        IoHandle(const IoHandle&) = delete;
        IoHandle& operator=(const IoHandle&) = delete;

        IoHandle(IoHandle&& rhs)
        {
            *this = std::move(rhs);
        }

        IoHandle& operator=(IoHandle&& rhs)
        {
            this->hDev = rhs.hDev;
            this->hLock = rhs.hLock;
            rhs.hDev = 0;
            rhs.hLock = 0;
            return *this;
        }

        ~IoHandle();

        /// Whether this is a valid (actually open) I/O handle.
        explicit operator bool() const;

        /// Returns a feature report from the device into `buffer+1`.
        /// *(u8*)(buffer+0) must contain a report id.
        bool GetFeature(void* buffer, size_t size) const;

        /// Sets a feature report into the device, taking it from `buffer+1`.
        /// *(u8*)(buffer+0) must contain a report id.
        bool SetFeature(const void* buffer, size_t size) const;

    protected:
        friend class HidDevice;
        #ifdef _WIN32
        using HANDLE = void*;
        #else
        using HANDLE = int;
        #endif

        hid_device *hDev;
        HANDLE hLock;

        explicit IoHandle(hid_device *hDev, HANDLE hLock) :
            hDev(hDev), hLock(hLock)
        {}
    };
    
    // Singleton for hid library
	class HidLibSingle
	{
	
		HidLibSingle();
	
	public:
		static HidLibSingle &hidlib();
		~HidLibSingle();
	};
};
