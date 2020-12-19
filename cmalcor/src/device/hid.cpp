#include "hid.hpp"
#include <vector>
#include <iterator>
#include <cstring>
#include <mutex>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <setupapi.h>
#include <shlobj.h>
#else
#include <sys/file.h>
#include <unistd.h>
#endif

static std::once_flag hid_init_flag;

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

    wchar_t lockpath[MAX_PATH];
    wchar_t devid[64];

    if(SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_DEFAULT, lockpath)))
    {
	if(swprintf(devid, std::size(devid), L"/device_hid_%X_%X.lock", vendor, product) > 0
	    && wcscat_s(lockpath, std::size(lockpath), devid) == 0)
	{
	    HANDLE fdLock = CreateFileW(lockpath, GENERIC_READ | GENERIC_WRITE, 0 /*noshare*/, nullptr, OPEN_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, nullptr);
	    if(fdLock != INVALID_HANDLE_VALUE)
	    {
		hid_device *pDev = hid_open_path(this->device.c_str());
		return IoHandle(pDev, fdLock);
	    }
	}
    }

    return IoHandle();
}
#else
auto HidDevice::OpenPosix() const -> IoHandle
{
    static_assert(std::is_same<int, IoHandle::FdHandle>::value, "");

    char lockpath[256];

    if(int written = snprintf(lockpath, std::size(lockpath), "/tmp/device_hid_%X_%X.lock", vendor, product);
	    written >= 0 && written < std::size(lockpath))
    {
	if(int fdLock = open(lockpath, O_RDWR | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
		fdLock != -1)
	{
	    struct flock lock;
	    memset(&lock, 0, sizeof(lock));
	    lock.l_type = F_WRLCK;
	    
	    if(fcntl(fdLock, F_SETLKW, &lock) == 0)
	    {
		hid_device *pDev = hid_open_path(this->device.c_str());
		return IoHandle(pDev, fdLock);
	    }

	    close(fdLock);
	}
    }

    return IoHandle();
}
#endif

auto HidDevice::ScanForDevice(uint16_t vendor, uint16_t product) -> HidDevice
{
    std::call_once(hid_init_flag, [] {
	hid_init();
	std::atexit([] { hid_exit(); });
    });

    HidDevice output;
    hid_device_info *hidenum = hid_enumerate(vendor, product);
    
    if(hidenum != nullptr)
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
        hid_close(pDev);
        if(fdLock != FdHandle(-1))
        {
            #ifdef _WIN32
	    CloseWin32Lock();
            #else
	    ClosePosixLock();
            #endif
        }
    }
}

#ifdef _WIN32
void HidDevice::IoHandle::CloseWin32Lock()
{
    CloseHandle(fdLock);
    fdLock = FdHandle(-1);
}
#else
void HidDevice::IoHandle::ClosePosixLock()
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_UNLCK;
    
    fcntl(fdLock, F_SETLKW, &lock);
    
    close(fdLock);
    fdLock = -1;
}
#endif

HidDevice::IoHandle::operator bool() const
{
    return (pDev != nullptr);
}

bool HidDevice::IoHandle::GetFeature(void* buffer, size_t size) const
{
    return hid_get_feature_report(pDev, reinterpret_cast<uint8_t*>(buffer), size) != -1;
}

bool HidDevice::IoHandle::SetFeature(const void* buffer, size_t size) const
{
    return hid_send_feature_report(pDev, reinterpret_cast<const uint8_t*>(buffer), size) != -1;
}
