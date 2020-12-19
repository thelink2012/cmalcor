#define WIN32_LEAN_AND_MEAN
#include "hid.hpp"
#include <vector>
#include <iterator>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#include <SetupAPI.h>
#include <ShlObj.h>
#else
#include <sys/file.h> // flock(2)
#include <unistd.h> // close(2)
constexpr int INVALID_HANDLE_VALUE = -1;
#endif

HidDevice::HidLibSingle::HidLibSingle()
{
	hid_init();
}

HidDevice::HidLibSingle::~HidLibSingle()
{
	hid_exit();
}

HidDevice::HidLibSingle &HidDevice::HidLibSingle::hidlib()
{
	static HidLibSingle single;
	return single;
}

auto HidDevice::Open() const -> IoHandle
{
    //static_assert(std::is_same<::HANDLE, IoHandle::HANDLE>::value, "");
    
    if(*this)
    {
        IoHandle result;
        #ifdef _WIN32
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
                    hid_device *hDev = hid_open_path(this->device.c_str());
                    return IoHandle(hDev, hLock);
                }
            }
        }
        #else
        // Assuming is POSIX
        char lockpath[256] = { 0 };
        if (snprintf(lockpath, std::size(lockpath), "/run/lock/device_hid_%X_%X.lock", vendor, product) > 0)
        {
            int hLock = open(lockpath, O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
            
            // fcntl works over every filesystem
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_WRLCK;
            
            if (fcntl(hLock, F_SETLKW, &lock) == 0)
            {
                hid_device *hDev = hid_open_path(this->device.c_str());
                return IoHandle(hDev, hLock);
            }
        }
        #endif
    }
    return IoHandle();
}

auto HidDevice::ScanForDevice(uint16_t vendor, uint16_t product) -> HidDevice
{
    HidLibSingle::hidlib();
    HidDevice output;
    hid_device_info *hidenum = hid_enumerate(vendor, product);
    
    if (hidenum != nullptr)
    {
        output = HidDevice(hidenum->path == nullptr? "" : hidenum->path, hidenum->vendor_id, hidenum->product_id);
        
        hid_free_enumeration(hidenum);
    }
    
    return output;
}

HidDevice::IoHandle::~IoHandle()
{
    if(*this)
    {
        hid_close(hDev);
        //CloseHandle(hDev);

        if(hLock != 0 && hLock != INVALID_HANDLE_VALUE)
        {
            #ifdef _WIN32
            CloseHandle(hLock);
            #else
            struct flock lock;
            memset(&lock, 0, sizeof(lock));
            lock.l_type = F_UNLCK;
            
            fcntl(hLock, F_SETLKW, &lock);
            
            char path[256] = { 0 };
            
            // Remove lockfile from filesystem
            if (snprintf(path, std::size(path), "/proc/self/fd/%d", hLock) > 0)
            {
                if (readlink(path, path, std::size(path)) > 0)
                {
                    unlink(path);
                }
            } else path[0] = 0;
            
            // Close handle
            close(hLock);
            #endif
        }
    }
}

HidDevice::IoHandle::operator bool() const
{
    return (hDev != nullptr);
}

bool HidDevice::IoHandle::GetFeature(void* buffer, size_t size) const
{
    return hid_get_feature_report(hDev, reinterpret_cast<uint8_t*>(buffer), size) != -1;
}

bool HidDevice::IoHandle::SetFeature(const void* buffer, size_t size) const
{
    return hid_send_feature_report(hDev, reinterpret_cast<const uint8_t*>(buffer), size) != -1;
}
