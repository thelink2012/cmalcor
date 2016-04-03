#define WIN32_LEAN_AND_MEAN
#include "hid.hpp"
#include <vector>
#include <windows.h>
#include <hidsdi.h>
#include <setupapi.h>
#include <shlobj.h>

auto HidDevice::Open() const -> IoHandle
{
    static_assert(std::is_same<::HANDLE, IoHandle::HANDLE>::value, "");

    if(*this)
    {
        wchar_t lockpath[MAX_PATH];
        wchar_t devid[64];

        if(SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_DEFAULT, lockpath)))
        {
            if(swprintf(devid, std::size(devid), L"/device_hid_%X_%X.lock", vendor, product) > 0
                && wcscat_s(lockpath, std::size(lockpath), devid) == 0)
            {
                HANDLE hLock = CreateFileW(lockpath, GENERIC_READ | GENERIC_WRITE, 0 /*noshare*/, nullptr, OPEN_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, nullptr);
                if(hLock != INVALID_HANDLE_VALUE)
                {
                    HANDLE hDev = CreateFileW(this->device.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, 0);
                    return IoHandle(hDev, hLock);
                }
            }
        }
    }
    return IoHandle();
}

auto HidDevice::ScanForDevice(uint16_t vendor, uint16_t product) -> HidDevice
{
    HidDevice output;
    std::vector<uint8_t> buffer;
    GUID guid;

    HidD_GetHidGuid(&guid);

    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&guid, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if(hDevInfo != INVALID_HANDLE_VALUE)
    {
        for(uint32_t i = 0; !output; ++i)
        {
            DWORD required_size;
            SP_DEVICE_INTERFACE_DATA data;
            data.cbSize = sizeof(data);

            if(!SetupDiEnumDeviceInterfaces(hDevInfo, nullptr, &guid, i, &data))
                break;

            if(!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &data, nullptr, 0, &required_size, nullptr)
                && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if(buffer.size() < required_size)
                    buffer.resize(required_size);

                auto& detail = *reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buffer.data());
                detail.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

                if(SetupDiGetDeviceInterfaceDetailW(hDevInfo, &data, &detail, buffer.size(), nullptr, nullptr))
                {
                    HANDLE hDev = CreateFileW(detail.DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
                    if(hDev != INVALID_HANDLE_VALUE)
                    {
                        HIDD_ATTRIBUTES attrs;
                        attrs.Size = sizeof(attrs);

                        if(HidD_GetAttributes(hDev, &attrs))
                        {
                            if(attrs.VendorID == vendor && attrs.ProductID == product)
                            {
                                output = HidDevice(detail.DevicePath, attrs.VendorID, attrs.ProductID);
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

HidDevice::IoHandle::~IoHandle()
{
    if(*this)
    {
        CloseHandle(hDev);

        if(hLock != 0 && hLock != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hLock);
        }
    }
}

HidDevice::IoHandle::operator bool() const
{
    return (hDev != 0 && hDev != INVALID_HANDLE_VALUE);
}

bool HidDevice::IoHandle::GetFeature(void* buffer, size_t size) const
{
    return !!HidD_GetFeature(hDev, buffer, size);
}

bool HidDevice::IoHandle::SetFeature(const void* buffer, size_t size) const
{
    return !!HidD_SetFeature(hDev, (PVOID)(buffer), size);
}
