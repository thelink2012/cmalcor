#include <cstdio>
#include <stdexcept>
#include <memory>
#include <string>
#include <cmalcor.h>
#include <docopt.h>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

static const char VERSION[] = CMALCOR_CAPI_VERSION_STRING;

static const char USAGE[] =
R"(CM Storm Alcor CLI Software.

   Usage:
      cmalcor is-present
      cmalcor firmware-version
      cmalcor memory-read <begin> <end> [--raw]
      cmalcor flash-erase <begin> <end> [--force]
      cmalcor enable-custom-led
      cmalcor disable-custom-led
      cmalcor set-led <mode> <brightness> <red> <green> <blue>
      cmalcor get-led
      cmalcor is-led-configured
      cmalcor (-h | --help)
      cmalcor --version

   Options:
      -h, --help    Show this screen.
      --version     Show version of this software.

   Copyright (c) 2016 Denilson das Merces Amorim <dma_2012@hotmail.com>
   Licensed under the MIT License.
   https://github.com/thelink2012/cmalcor
)";

struct AppException : std::runtime_error
{
    AppException(const std::string& msg)
        : std::runtime_error(msg)
    {}
};

void verify(int error)
{
    if(error != CMALCOR_ERROR_NOERROR)
    {
        std::string buffer;
        buffer.resize((size_t)(CmAlcor_ErrorToString(error, nullptr, 0)));
        if(buffer.size())
        {
            if(CmAlcor_ErrorToString(error, &buffer[0], buffer.size()))
            {
                buffer.pop_back(); // null terminator
                throw AppException(buffer);
            }
        }
        throw AppException("Unknown error");
    }
}

void verify_address(uint32_t begin, uint32_t end)
{
    if(begin > end)
        throw AppException("begin is greater than end");
    else if((begin % 4) != 0)
        throw AppException("begin is not 4 byte aligned");
    else if((end % 4) != 3)
        throw AppException("end is not 3 byte aligned");
}

void write(const void* buffer, size_t size)
{
#ifdef _WIN32
    fflush(stdout);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    fwrite(buffer, 1, size, stdout);
#ifdef _WIN32
    fflush(stdout);
    _setmode(_fileno(stdout), _O_TEXT);
#endif
}

int main(int argc, char* argv[])
{
    int error = CMALCOR_ERROR_NOERROR;
    auto args = docopt::docopt(USAGE, { argv+1, argv+argc }, true, VERSION);

    try
    {
        if(args["is-present"].asBool())
        {
            return CmAlcor_IsMousePresent()? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(args["firmware-version"].asBool())
        {
            int version = CmAlcor_GetFirmwareVersion(&error);
            verify(error);
            printf("%d.%d.%d.%d\n", (version >> 12) & 0xF, (version >> 8) & 0xF, (version >> 4) & 0xF, (version >> 0) & 0xF);
        }
        else if(args["memory-read"].asBool())
        {
            auto begin = (uint32_t) std::stoul(args["<begin>"].asString(), 0, 0);
            auto end   = (uint32_t) std::stoul(args["<end>"].asString(), 0, 0);
            verify_address(begin, end);

            static const size_t buffer_size = (end - begin) + 1;
            std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);

            if(!CmAlcor_MemoryRead(&error, begin, end, buffer.get()))
                verify(error);

            if(args["--raw"].asBool())
            {
                write(buffer.get(), buffer_size);
            }
            else
            {
                const uint8_t* p = buffer.get();
                const uint8_t* pend = p + buffer_size;

                while(p < pend)
                {
                    for(size_t i = 0; i < 16 && p < pend; ++i, ++p)
                        printf("%.2X ", *p);
                    printf("\n");
                }
            }
        }
        else if(args["flash-erase"].asBool())
        {
            if(args["--force"].asBool())
            {
                auto begin = (uint32_t)std::stoul(args["<begin>"].asString(), 0, 0);
                auto end = (uint32_t)std::stoul(args["<end>"].asString(), 0, 0);
                verify_address(begin, end);

                if(!CmAlcor_FlashErasePages(&error, begin, end))
                    verify(error);
            }
            else
            {
                throw AppException("Before doing a flash-erase, specially on the settings region, make sure you've disabled the custom LED!\n"
                                   "Due to the highly unsafety of this operation, the --force parameter is required.");
            }
        }
        else if(args["enable-custom-led"].asBool())
        {
            if(!CmAlcor_EnableCustomLED(&error))
                verify(error);
        }
        else if(args["disable-custom-led"].asBool())
        {
            if(!CmAlcor_DisableCustomLED(&error))
                verify(error);
        }
        else if(args["set-led"].asBool())
        {
            auto mode = std::stoi(args["<mode>"].asString(), 0, 0);
            auto brght = std::stoi(args["<brightness>"].asString(), 0, 0);
            auto red = std::stoi(args["<red>"].asString(), 0, 0);
            auto green = std::stoi(args["<green>"].asString(), 0, 0);
            auto blue = std::stoi(args["<blue>"].asString(), 0, 0);
            // parameter checking is made on the function.

            if(!CmAlcor_SetLED(&error, mode, brght, red, green, blue))
                verify(error);
        }
        else if(args["get-led"].asBool())
        {
            int mode, brght, red, green, blue;
            if(CmAlcor_GetLED(&error, &mode, &brght, &red, &green, &blue))
            {
                printf("%d %d %d %d %d\n", mode, brght, red, green, blue);
            }
            else
            {
                if(error != CMALCOR_ERROR_NOERROR)
                    verify(error);
                else
                    throw AppException("no programmed setting");
            }
        }
        else if(args["is-led-configured"].asBool())
        {
            auto has_config = CmAlcor_HasLEDConfig(&error);
            verify(error);
            return has_config? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    catch(const AppException& ex)
    {
        fprintf(stderr, "error: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    catch(const std::exception& ex)
    {
        fprintf(stderr, "error: %s\n", ex.what());
        return EXIT_FAILURE;
    }
}

