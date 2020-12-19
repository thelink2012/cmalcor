#define WIN32_LEAN_AND_MEAN
#include "io_alcor.hpp"
#include <algorithm>
#include <thread> // Sleep replacement std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds

bool IoAlcorFirmware::GetVersion(uint16_t& out_version)
{
    const Operation inst(Op::Version);
    Operation outbuf;

    if(IoRequest(inst, outbuf) && IoWaitRequestOutput4(inst, outbuf))
    {
        out_version = outbuf.Out0<uint16_t>();
        return true;
    }
    return false;
}

bool IoAlcorFirmware::GetNumber2(uint32_t& out_2)
{
    const Operation inst(Op::Return2);
    Operation outbuf;

    if(IoRequest(inst, outbuf) && IoWaitRequestOutput4(inst, outbuf))
    {
        out_2 = outbuf.Out0<uint32_t>();
        return true;
    }
    return false;
}

bool IoAlcorFirmware::DoUnk82(bool enable_led)
{
    const Operation inst(Op::Unk82, true, (enable_led? 1 : 0), 0);
    return IoPerform(inst);
}

bool IoAlcorFirmware::DoUnk84_0()
{
    const Operation inst(Op::Unk84, true, 0, 0);
    return IoPerform(inst);
}

bool IoAlcorFirmware::DoUnk84_1()
{
    const Operation inst(Op::Unk84, true, 1, 0);
    return IoPerform(inst);
}

bool IoAlcorFirmware::Checksum(uint32_t begin, uint32_t end, uint16_t& out_checksum)
{
    const Operation inst_checksum(Op::Checksum, begin, end);
    Operation outbuf;

    // Ensure range is correct.
    if(begin > end)
        return false;

    // Ensure word alignment on begin, and one byte before alignment on end.
    if((begin % 4) != 0 || (end % 4) != 3)
        return false;

    if(IoRequest(inst_checksum, outbuf) && IoWaitRequestOutput2(inst_checksum, outbuf))
    {
        if((outbuf.out[0] & outbuf.out[1]) == success_request)
        {
            out_checksum = outbuf.Out2<uint16_t>();
            return true;
        }
    }

    return false;
}

bool IoAlcorFirmware::FlashErasePages(uint32_t begin, uint32_t end)
{
    const Operation inst_erase(Op::Erase, begin, end);
    Operation outbuf;

    // Ensure range is correct.
    if(begin > end)
        return false;

    // Ensure word alignment on begin, and one byte before alignment on end.
    if((begin % 4) != 0 || (end % 4) != 3)
        return false;

    // For security reasons only allow flashing the user settings area.
    if(begin >= 0xD800 && end <= 0x1F7FF)
    {
        if(IoRequest(inst_erase, outbuf) && IoWaitRequestOutput4(inst_erase, outbuf))
        {
            return ((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == success_request);
        }
    }

    return false;
}

bool IoAlcorFirmware::MemoryRead(uint32_t begin, uint32_t end, void* dataoutv)
{
    const Operation inst_trigger(Op::ReadBytes, begin, end);
    Operation outbuf;
    uint8_t* dataout = (uint8_t*)(dataoutv);

    // Ensure range is correct.
    if(begin > end)
        return false;

    // Ensure word alignment on begin, and one byte before alignment on end.
    if((begin % 4) != 0 || (end % 4) != 3)
        return false;

    // Ensure the memory won't segfault during writing, if it is to segfault when reading from
    // the mouse memory, bad things may happen.
    std::memset(dataoutv, 0, (end - begin) + 1);

    if(IoRequest(inst_trigger, outbuf) && IoWaitRequestOutput4(inst_trigger, outbuf))
    {
        if((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == success_request)
        {
            static_assert(bytes_step + 1 == feature_size, "");

            for(uint32_t addr = begin; addr < end; addr += bytes_step)
            {
                // We need a temporary buffer because of report id on the first byte
                uint8_t buffer[feature_size];
                if(!IoBytesOutput(buffer, feature_size))
                    return false;

                auto left = (end - addr) + 1;
                std::memcpy(&dataout[addr - begin], &buffer[1], (std::min)(left, bytes_step));
            }

            return true;
        }
    }
    return false;
}

bool IoAlcorFirmware::FlashProgram(uint32_t begin, uint32_t end, void* datav, uint16_t& out_datav_checksum)
{
    const Operation inst_program(Op::Program, false, begin, end);
    Operation outbuf;
    uint8_t* data = (uint8_t*)(datav);

    out_datav_checksum = 0;

    // Ensure range is correct.
    if(begin > end)
        return false;

    // Ensure word alignment on begin, and one byte before alignment on end.
    if((begin % 4) != 0 || (end % 4) != 3)
        return false;

    // For security reasons only allow flashing the user settings area.
    if(begin >= 0xD800 && end <= 0x1F7FF)
    {
        if(IoRequest(inst_program, outbuf) && IoWaitRequestOutput4(inst_program, outbuf))
        {
            if((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == success_request)
            {
                for(uint32_t addr = begin; addr < end; addr += bytes_step)
                {
                    // We need a temporary buffer because of report id on the first byte
                    uint8_t buffer[feature_size];
                    auto left = (end - addr) + 1;
                    auto it_bytes = (std::min)(left, bytes_step);
                    std::memset(buffer, 0, feature_size); // necessary because next memcpy mayn't copy feature_size due to std::min
                    std::memcpy(&buffer[1], &data[addr - begin], it_bytes);

                    for(uint32_t idsum = 0; idsum < it_bytes; ++idsum)
                    {
                        if((addr + idsum != 0xD800) && (addr + idsum != 0xD801))
                        {
                            out_datav_checksum += buffer[idsum+1]; // (+1 to skip report id)
                        }
                    }

                    if(!IoBytesInput(inst_program, buffer, feature_size))
                        return false;
                }
                return true;
            }
        }
    }

    return false;
}

bool IoAlcorFirmware::FlashTellSuccessProgramming()
{
    const Operation inst_program(Op::Program, true, 0, 0);
    Operation outbuf;

    // XXX weirdly CoolerMaster does [something like] IoRequest in a loop here until it acks instead of IoWait'ing.
    if(IoRequest(inst_program, outbuf) && IoWaitRequestOutput4(inst_program, outbuf))
    {
        return (outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == success_request;
    }
    return false;
}


bool IoAlcorFirmware::IoEnableUnsafe(bool enable)
{
    const Operation inst(Op::EnableUnsafe, uint32_t(enable? 0x29156767 : 0x0));
    Operation outbuf;

    if(IoRequest(inst, outbuf) && IoWaitRequestOutput4(inst, outbuf))
    {
        if(enable)
            return ((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == success_request);
        else
            return ((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == failed_request);
    }

    return false;
}

bool IoAlcorFirmware::IoPerform(const Operation& inst)
{
    static_assert(sizeof(inst) == 65, "");
    return io.SetFeature(&inst, feature_size);
}

bool IoAlcorFirmware::IoRequest(const Operation& inst, Operation& outbuf)
{
    static_assert(sizeof(inst) == 65 && sizeof(outbuf) == 65, "");

    if(io.SetFeature(&inst, feature_size))
    {
        if(IoRequestOutput(inst, outbuf))
            return true;
    }
    return false;
}

bool IoAlcorFirmware::IoRequestOutput(const Operation& inst, Operation& outbuf)
{
    static_assert(sizeof(inst) == 65 && sizeof(outbuf) == 65, "");

    outbuf.Clear(inst);
    if(io.GetFeature(&outbuf, feature_size))
    {
        if(outbuf.op == inst.op && outbuf.sig1 == inst.sig1 && outbuf.sig2 == inst.sig2)
            return true;
    }

    return false;
}

// outbuf must have a state already
bool IoAlcorFirmware::IoWaitRequestOutput4(const Operation& inst, Operation& outbuf)
{
    static_assert(sizeof(inst) == 65 && sizeof(outbuf) == 65, "");

    // Erase should use IoWaitRequestOutput2
    if(inst.op == uint8_t(Op::Checksum))
        return false;

    while((outbuf.out[0] & outbuf.out[1] & outbuf.out[2] & outbuf.out[3]) == made_request)
    {
        if(!IoRequestOutput(inst, outbuf))
            return false;
    }

    return true;
}

// outbuf must have a state already
bool IoAlcorFirmware::IoWaitRequestOutput2(const Operation& inst, Operation& outbuf)
{
    static_assert(sizeof(inst) == 65 && sizeof(outbuf) == 65, "");

    // Erase only should use IoWaitRequestOutput2
    if(inst.op != uint8_t(Op::Checksum))
        return false;
    ////////

    while((outbuf.out[0] & outbuf.out[1]) == made_request)
    {
        if(!IoRequestOutput(inst, outbuf))
            return false;
    }

    return true;
}

bool IoAlcorFirmware::IoBytesOutput(void* outbufv, size_t size)
{
    if(size >= feature_size) // needs to put report_id on it still, so it's not size >= feature_size-1
    {
        uint8_t* outbuf = (uint8_t*)(outbufv);
        std::memset(outbuf, 0, feature_size);
        outbuf[0] = 0; // report id

        if(io.GetFeature(outbuf, feature_size))
            return true;
    }
    return false;
}

bool IoAlcorFirmware::IoBytesInput(const Operation& inst, void* inbufv, size_t size)
{
    if(size >= feature_size) // needs to put report_id on it still, so it's not size >= feature_size-1
    {
        uint8_t* inbuf = (uint8_t*)(inbufv);
        inbuf[0] = 0; // report id

        if(io.SetFeature(inbuf, feature_size))
        {
            Operation outbuf;

            std::this_thread::sleep_for(std::chrono::seconds(2));

            // Wait for input operation to complete
            if(IoRequestOutput(inst, outbuf) && IoWaitRequestOutput4(inst, outbuf))
            {
                return true;
            }
        }
    }
    return false;
}
