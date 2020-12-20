#pragma once
#include <cassert>
#include <cstdint>
#include <filesystem>

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
        return !device_path.empty();
    }

    /// Locks the device access (but only for users of this library) and returns a I/O handle to it.
    IoHandle Open() const;

    /// Finds a HID Device with the specified vendor and product id connected on the system.
    static HidDevice ScanForDevice(uint16_t vendor, uint16_t product);

private:
    std::filesystem::path device_path;
    uint16_t vendor, product;

    explicit HidDevice(std::filesystem::path device_path, uint16_t vendor, uint16_t product) :
        device_path(std::move(device_path)), vendor(vendor), product(product)
    {
        assert(!!*this);
    }

    IoHandle OpenWin32() const;
    IoHandle OpenPosix() const;

    static HidDevice ScanForDeviceWin32(uint16_t vendor, uint16_t product);
    static HidDevice ScanForDevicePosix(uint16_t vendor, uint16_t product);

public:
    /// Open device ready for I/O.
    class IoHandle
    {
    public:
        IoHandle() : hidDevice(NO_HID_HANDLE), fdLock(NO_FD_HANDLE)
        {}

        IoHandle(const IoHandle&) = delete;
        IoHandle& operator=(const IoHandle&) = delete;

        IoHandle(IoHandle&& rhs)
        {
            *this = std::move(rhs);
        }

        IoHandle& operator=(IoHandle&& rhs)
        {
            this->hidDevice = rhs.hidDevice;
            this->fdLock = rhs.fdLock;
            rhs.hidDevice = NO_HID_HANDLE;
            rhs.fdLock = NO_FD_HANDLE;
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
        using FdHandle = void*;
        using HidHandle = void*;
        static constexpr FdHandle NO_FD_HANDLE = FdHandle(-1);
        static constexpr HidHandle NO_HID_HANDLE = nullptr;
        #else
        using FdHandle = int;
        using HidHandle = struct hid_device_*;
        static constexpr FdHandle NO_FD_HANDLE = -1;
        static constexpr HidHandle NO_HID_HANDLE = nullptr;
        #endif

        HidHandle hidDevice;
        FdHandle fdLock;

        explicit IoHandle(HidHandle hidDevice, FdHandle fdLock) :
            hidDevice(hidDevice), fdLock(fdLock)
        {}

    private:
	    void CloseWin32();
	    void ClosePosix();
    };
};
