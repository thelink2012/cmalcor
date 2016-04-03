--[[
    Use 'premake5 --help' for help
--]]

--[[
    Options and Actions
--]]

--[[
    The Solution
--]]
solution "CmAlcor"

    configurations { "Release", "Debug" }

    targetprefix "" -- no 'lib' prefix on gcc
    targetdir "bin"
    implibdir "bin"

    startproject "cmalcor"

    flags {
        "StaticRuntime",
        "NoImportLib",
        "NoRTTI",
        "NoBufferSecurityCheck"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS"
    }

    configuration "Debug*"
        flags { "Symbols" }
        
    configuration "Release*"
        defines { "NDEBUG" }
        optimize "Speed"

    project "cmalcor"
        language "C++"
        kind "SharedLib"
        targetname "cmalcor"
        
        defines { "CMALCOR_COMPILING" }

        flags { "NoPCH" }

        files {
            "include/**.h",
            "src/**.cpp",
            "src/**.cc",
            "src/**.hpp",
            "src/**.h",
            "src/**.def",
        }

        includedirs { "include" }

        links {
            "hid",
            "setupapi",
        }
        


