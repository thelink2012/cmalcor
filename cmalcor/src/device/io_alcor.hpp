#pragma once
#include "hid.hpp"
#include <cstring> // std::memset/std::memcpy

class IoAlcorFirmware
{
private:
    enum class Op : uint8_t;
    struct Operation;

    HidDevice::IoHandle io;

    static const uint32_t feature_size = 65;
    static const uint32_t bytes_step = feature_size - 1; // minus report id

    static const uint8_t made_request = 0xFB;
    static const uint8_t success_request = 0xFA;
    static const uint8_t failed_request = 0xFE;

public:
    struct Unsafeguard;

    explicit IoAlcorFirmware()
        : io()
    {
    }

    explicit IoAlcorFirmware(const HidDevice& device)
        : io(device.Open())
    {
    }

    IoAlcorFirmware(IoAlcorFirmware&& rhs) = default;
    IoAlcorFirmware& operator=(IoAlcorFirmware&& rhs) = default;
    IoAlcorFirmware(const IoAlcorFirmware&) = delete;
    IoAlcorFirmware& operator=(const IoAlcorFirmware&) = delete;

    /// Whether this firmware object contains a valid I/O handle.
    explicit operator bool()
    {
        return !!io;
    }

    /// Gets the version of the firmware in the device.
    ///
    /// Returns false on failure.
    bool GetVersion(uint16_t& out_version);

    /// Gets the number 2 (yes, this is a in-device operation).
    ///
    /// The actual purpose of this operation is unknown.
    ///
    /// Returns false on failure.
    bool GetNumber2(uint32_t& out_2);

    /// The actual purpose of this operation is unknown.
    ///
    /// Returns false on failure.
    bool DoUnk82(bool enable_led = true); // TODO remove default param later

    /// The actual purpose of this operation is unknown.
    ///
    /// Returns false on failure.
    bool DoUnk84_0();

    /// The actual purpose of this operation is unknown.
    ///
    /// Returns false on failure.
    bool DoUnk84_1();

    /// Gets the checksum of the last write operation.
    ///
    /// Returns false on failure.
    bool Checksum(uint32_t begin, uint32_t end, uint16_t& out_checksum);

    /// Erases the pages in range `begin`-`end` (inclusive).
    ///
    /// Returns false on failure.
    bool FlashErasePages(uint32_t begin, uint32_t end);

    /// Reads the device memory (Flash, SRAM, AMBA, PPB) from `begin` to `end` (inclusive) into `dataoutv`.
    ///
    /// Returns false on failure.
    bool MemoryRead(uint32_t begin, uint32_t end, void* dataoutv);

    /// Programs data into the device flash from `begin` to `end` (inclusive) from `datav`.
    ///
    /// Additionally, outputs the host checksum (calculated by us during the write) into `out_datav_checksum`.
    /// To verify for a mismatch with the device, see `Checksum(...)`.
    ///
    /// Returns false on failure.
    bool FlashProgram(uint32_t begin, uint32_t end, void* datav, uint16_t& out_datav_checksum);

    /// Tells the device that the last programming operation was successful.
    /// That is, no checksum mismatch between the device and the host happened.
    ///
    /// Returns false on failure.
    bool FlashTellSuccessProgramming();

protected:

    bool IoEnableUnsafe(bool enable);

    bool IoPerform(const Operation& inst);

    bool IoRequest(const Operation& inst, Operation& outbuf);

    bool IoRequestOutput(const Operation& inst, Operation& outbuf);

    // outbuf must have a state already
    bool IoWaitRequestOutput4(const Operation& inst, Operation& outbuf);

    // outbuf must have a state already
    bool IoWaitRequestOutput2(const Operation& inst, Operation& outbuf);

    bool IoBytesOutput(void* outbufv, size_t size);

    bool IoBytesInput(const Operation& inst, void* inbufv, size_t size);

public:
    /// The firmware will allow unsafe operations (those that deal with erasing/reading/writing directly to the device)
    /// in the mean time this guard is in scope.
    ///
    /// **Note 1:** The caller must ensure the firmware guarded lives as much as this guard.
    /// **Note 2:** The caller must ensure that there's no more than one guard for a specific firmware in scope.
    struct UnsafeGuard
    {
        IoAlcorFirmware& fw;
        bool result;

        /// Creates a context for unsafe operations in `fw`.
        /// Do note `fw` must live longer than `*this`.
        UnsafeGuard(IoAlcorFirmware& fw) :
            fw(fw)
        {
            this->result = fw.IoEnableUnsafe(true);
        }

        ~UnsafeGuard()
        {
            if(*this) fw.IoEnableUnsafe(false);
        }

        /// Whether the guard was created successfully.
        explicit operator bool()
        {
            return this->result;
        }
    };

private:
    /// CM Alcor Firmware Operations.
    enum class Op : uint8_t
    {
        KernelMode = 0x00,
        Return2 = 0x01,
        EnableUnsafe = 0x02,
        Version = 0x03,
        Erase = 0x04,
        Program = 0x05,
        Checksum = 0x06,
        UserMode = 0x07,
        ReadBytes = 0x08,
        Profile = 0x80,
        IsMizar = 0x81,
        Unk82 = 0x82,
        Unk84 = 0x84,
    };

    /// Describes a operation input/output.
    struct alignas(1) Operation
    {
    public:
        explicit Operation(Op opcode)
            : Operation()
        {
            this->op = (uint8_t)(opcode);
        }

        explicit Operation(Op opcode, uint32_t a, uint32_t b)
            : Operation()
        {
            this->op = (uint8_t)(opcode);
            this->ArgN(a, 0);
            this->ArgN(b, 4);
        }

        explicit Operation(Op opcode, uint32_t a)
            : Operation()
        {
            this->op = (uint8_t)(opcode);
            this->ArgN(a, 0);
        }

        explicit Operation(Op opcode, bool boolean_, uint32_t a, uint32_t b)
            : Operation(opcode, a, b)
        {
            this->boolean = boolean_? 1 : 0;
        }

        /// Get output at out+0x0
        template<typename T>
        T Out0()
        {
            T v;
            this->OutN(v, 0);
            return v;
        }

        /// Get output at out+0x2
        template<typename T>
        T Out2()
        {
            static_assert(sizeof(T) != 4, ""); // most likely we only want to use this with u8 and u16
            T v;
            this->OutN(v, 2);
            return v;
        }

        /// Copies main properties from `inst` into `*this` but clears out `this->arg`/`this->out`.
        void Clear(const Operation& inst)
        {
            static_assert(sizeof(Operation) == feature_size, "");
            static_assert(sizeof(this->arg) == 60, "");

            std::memset(this->arg, 0, sizeof(this->arg));
            this->report_id = inst.report_id;
            this->op = inst.op;
            this->sig1 = inst.sig1;
            this->sig2 = inst.sig2;
            this->boolean = inst.boolean;
        }

    protected:
        friend class IoAlcorFirmware;

        Operation()
            : report_id(0), op(0xFF), sig1(0xAA), sig2(0x55), boolean(0)
        {
            static_assert(sizeof(Operation) == feature_size, "");
            static_assert(sizeof(this->arg) == 60, "");

            std::memset(this->arg, 0, sizeof(this->arg));
        }

        void OutN(uint16_t& v, size_t n)
        {
            v = (uint16_t(out[n + 0]) | (uint16_t(out[n + 1]) << 8));
        }

        void OutN(uint32_t& v, size_t n)
        {
            v = (uint32_t(out[0]) | (uint32_t(out[1]) << 8) | (uint32_t(out[2]) << 16) | (uint32_t(out[3]) << 24));
        }

        void ArgN(uint32_t i, size_t n)
        {
            arg[n + 0] = uint8_t((i & 0x000000FF) >> 0);
            arg[n + 1] = uint8_t((i & 0x0000FF00) >> 8);
            arg[n + 2] = uint8_t((i & 0x00FF0000) >> 16);
            arg[n + 3] = uint8_t((i & 0xFF000000) >> 24);
        }

    protected:
        uint8_t report_id;
        uint8_t op;                 // inbuf+0x0 (excluding report_id)
        uint8_t sig1;               // inbuf+0x1
        uint8_t sig2;               // inbuf+0x2
        uint8_t boolean;            // inbuf+0x3
        union {
            uint8_t arg[60];        // inbuf+0x4 - inbuf+0x40
            uint8_t out[60];        // inbuf+0x4 - inbuf+0x40
        };
    };
};
