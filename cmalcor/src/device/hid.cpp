#include "hid.hpp"
#include <cstring>
#include <iterator>
#include <mutex>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <hidsdi.h>
#include <setupapi.h>
#include <shlobj.h>
#include <windows.h>
#else
#include <hidapi/hidapi.h>
#include <sys/file.h>
#include <unistd.h>
#endif

auto HidDevice::Open() const -> IoHandle
{
    if(*this)
    {
#ifdef _WIN32
        return OpenWin32();
#else
        return OpenPosix();
#endif
    }
    return IoHandle();
}

#ifdef _WIN32
auto HidDevice::OpenWin32() const -> IoHandle
{
    static_assert(std::is_same<::HANDLE, IoHandle::FdHandle>::value, "");
    static_assert(std::is_same<::HANDLE, IoHandle::HidHandle>::value, "");

    wchar_t lockpath[MAX_PATH];
    wchar_t devid[64];

    if(SUCCEEDED(SHGetFolderPathW(nullptr,
                                  CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
                                  NULL, SHGFP_TYPE_DEFAULT, lockpath)))
    {
        if(swprintf(devid, std::size(devid), L"/device_hid_%X_%X.lock", vendor,
                    product)
                   > 0
           && wcscat_s(lockpath, std::size(lockpath), devid) == 0)
        {
            HANDLE fdLock = CreateFileW(lockpath, GENERIC_READ | GENERIC_WRITE,
                                        0 /*noshare*/, nullptr, OPEN_ALWAYS,
                                        FILE_FLAG_DELETE_ON_CLOSE, nullptr);
            if(fdLock != INVALID_HANDLE_VALUE)
            {
                HANDLE hidDevice = CreateFileW(this->device_path.c_str(), 0,
                                               FILE_SHARE_READ
                                                       | FILE_SHARE_WRITE,
                                               nullptr, OPEN_EXISTING, 0, 0);
                if(hidDevice != INVALID_HANDLE_VALUE)
                    return IoHandle(hidDevice, fdLock);

                CloseHandle(fdLock);
            }
        }
    }

    return IoHandle();
}
#else
auto HidDevice::OpenPosix() const -> IoHandle
{
    static_assert(std::is_same<int, IoHandle::FdHandle>::value, "");
    static_assert(std::is_same<hid_device*, IoHandle::HidHandle>::value, "");

    char lockpath[256];

    if(int written = snprintf(lockpath, std::size(lockpath),
                              "/tmp/device_hid_%X_%X.lock", vendor, product);
       written >= 0 && written < std::size(lockpath))
    {
        if(int fdLock = open(lockpath, O_RDWR | O_CREAT,
                             S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
           fdLock != -1)
        {
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;

            if(fcntl(fdLock, F_SETLKW, &lock) == 0)
            {
                if(hid_device* hidDevice = hid_open_path(
                           this->device_path.c_str());
                   hidDevice != nullptr)
                    return IoHandle(hidDevice, fdLock);
            }

            close(fdLock);
        }
    }

    return IoHandle();
}
#endif

auto HidDevice::ScanForDevice(uint16_t vendor, uint16_t product) -> HidDevice
{
#ifdef _WIN32
    return ScanForDeviceWin32(vendor, product);
#else
    return ScanForDevicePosix(vendor, product);
#endif
}

#ifdef _WIN32
auto HidDevice::ScanForDeviceWin32(uint16_t vendor, uint16_t product)
        -> HidDevice
{
    HidDevice output;
    std::vector<uint8_t> buffer;
    GUID guid;

    HidD_GetHidGuid(&guid);

    HDEVINFO hDevInfo = SetupDiGetClassDevsW(
            &guid, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if(hDevInfo != INVALID_HANDLE_VALUE)
    {
        for(uint32_t i = 0; !output; ++i)
        {
            DWORD required_size;
            SP_DEVICE_INTERFACE_DATA data;
            data.cbSize = sizeof(data);

            if(!SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &guid, i, &data))
                break;

            if(!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &data, nullptr, 0,
                                                 &required_size, nullptr)
               && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if(buffer.size() < required_size)
                    buffer.resize(required_size);

                auto& detail
                        = *reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(
                                buffer.data());
                detail.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

                if(SetupDiGetDeviceInterfaceDetailW(hDevInfo, &data, &detail,
                                                    buffer.size(), nullptr,
                                                    nullptr))
                {
                    HANDLE hDev = CreateFileW(
                            detail.DevicePath, 0,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                            OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
                    if(hDev != INVALID_HANDLE_VALUE)
                    {
                        HIDD_ATTRIBUTES attrs;
                        attrs.Size = sizeof(attrs);

                        if(HidD_GetAttributes(hDev, &attrs))
                        {
                            if(attrs.VendorID == vendor
                               && attrs.ProductID == product)
                            {
                                output = HidDevice(static_cast<wchar_t*>(
                                                           detail.DevicePath),
                                                   attrs.VendorID,
                                                   attrs.ProductID);
                            }
                        }

                        CloseHandle(hDev);
                    }
                }
            }
        }

        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    return output;
}
#else
auto HidDevice::ScanForDevicePosix(uint16_t vendor, uint16_t product)
        -> HidDevice
{
    static std::once_flag hid_init_flag;
    std::call_once(hid_init_flag, [] {
        hid_init();
        std::atexit([] { hid_exit(); });
    });

    HidDevice output;
    hid_device_info* hidenum = hid_enumerate(vendor, product);

    if(hidenum != nullptr)
    {
        output = HidDevice(hidenum->path == nullptr ? "" : hidenum->path,
                           hidenum->vendor_id, hidenum->product_id);
        hid_free_enumeration(hidenum);
    }

    return output;
}
#endif

HidDevice::IoHandle::~IoHandle()
{
#ifdef _WIN32
    CloseWin32();
#else
    ClosePosix();
#endif
}

#ifdef _WIN32
void HidDevice::IoHandle::CloseWin32()
{
    if(hidDevice != NO_HID_HANDLE)
    {
        CloseHandle(hidDevice);
        hidDevice = NO_HID_HANDLE;
    }

    if(fdLock != NO_FD_HANDLE)
    {
        CloseHandle(fdLock);
        fdLock = NO_FD_HANDLE;
    }
}
#else
void HidDevice::IoHandle::ClosePosix()
{
    if(hidDevice != NO_HID_HANDLE)
    {
        hid_close(hidDevice);
        hidDevice = NO_HID_HANDLE;
    }

    if(fdLock != NO_FD_HANDLE)
    {
        struct flock lock;
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_UNLCK;

        fcntl(fdLock, F_SETLKW, &lock);

        close(fdLock);
        fdLock = NO_FD_HANDLE;
    }
}
#endif

HidDevice::IoHandle::operator bool() const
{
    return (hidDevice != NO_HID_HANDLE);
}

bool HidDevice::IoHandle::GetFeature(void* buffer, size_t size) const
{
#ifdef _WIN32
    return !!HidD_GetFeature(hidDevice, buffer, size);
#else
    return hid_get_feature_report(hidDevice, reinterpret_cast<uint8_t*>(buffer),
                                  size)
           != -1;
#endif
}

bool HidDevice::IoHandle::SetFeature(const void* buffer, size_t size) const
{
#ifdef _WIN32
    return !!HidD_SetFeature(hidDevice, (PVOID)(buffer), size);
#else
    return hid_send_feature_report(
                   hidDevice, reinterpret_cast<const uint8_t*>(buffer), size)
           != -1;
#endif
}