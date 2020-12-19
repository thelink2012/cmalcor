--[[
    Use 'premake5 --help' for help
--]]

--[[
    Options and Actions
--]]

newoption {
    trigger     = "outdir",
    value       = "path",
    description = "Output directory for the build files"
}
if not _OPTIONS["outdir"] then
    _OPTIONS["outdir"] = "."
end

newoption {
    trigger     = "mizar",
    description = "Makes a project that builds the Mizar based library"
}

--[[
    The Solution
--]]
solution "CmAlcor"

    configurations { "Release", "Debug" }

    location( _OPTIONS["outdir"] )
    targetprefix "" -- no 'lib' prefix on gcc
    targetdir (_OPTIONS["outdir"] .. '/' .. "bin")
    implibdir (_OPTIONS["outdir"] .. '/' .. "bin")

    startproject "cmalcor-cli"

    flags {
        "StaticRuntime",
        "NoBufferSecurityCheck"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "_SCL_SECURE_NO_WARNINGS"
    }

    if _OPTIONS["mizar"] then
        defines { "MIZAR_PATCH=1" }
    end

    includedirs { "include" }

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

        excludes { "src/main.cpp" }

        links {
            "hidapi-hidraw",
        }

    project "cmalcor-cli"
        language "C++"
        kind "ConsoleApp"
        targetname "cmalcor"
        
        defines { "CMALCOR_COMPILING_CLI" }

        flags { "NoPCH" }

        files {
            "src/main.cpp",
            "deps/docopt/docopt.cpp",
        }
 
        includedirs { "deps/docopt/" }

        links { "cmalcor" }
        
