newoption {
    trigger     = "mizar",
    description = "Makes a project that builds the Mizar based library"
}

workspace "cmalcor"
    cppdialect "C++17"
    startproject "cmalcor-cli"

    if _OPTIONS["mizar"] then
        defines "MIZAR_PATCH=1"
    end

    includedirs "include"

project "cmalcor"
    language "C++"
    kind "SharedLib"
    targetname "cmalcor"
    
    defines "CMALCOR_COMPILING"

    files {
        "include/**.h",
        "src/**.cpp",
        "src/**.cc",
        "src/**.hpp",
        "src/**.h",
        "src/**.def",
    }

    excludes "src/main.cpp"

    filter "system:windows"
        links { "hid", "setupapi" }
        
    filter "system:linux"
        links "hidapi-hidraw"

project "cmalcor-cli"
    language "C++"
    kind "ConsoleApp"
    targetname "cmalcor"
    
    defines "CMALCOR_COMPILING_CLI"

    files {
        "src/main.cpp",
        "deps/docopt/docopt.cpp",
    }

    includedirs "deps/docopt/"

    links "cmalcor"

workspace "*"
    configurations { "Release", "Debug" }
    location "build"
    targetdir "bin"
    implibdir "bin"

    flags {
        "NoPCH",
        "NoBufferSecurityCheck"
    }

filter "configurations:Debug"
    symbols "On"
        
filter "configurations:Release"
    defines "NDEBUG"
    symbols "Off"

    optimize "Speed"
    functionlevellinking "On"
    flags "LinkTimeOptimization"

filter { "toolset:*_xp" }
    defines { "WINVER=0x0501", "_WIN32_WINNT=0x0501" }
    buildoptions { "/Zc:threadSafeInit-" }

filter { "action:vs*" }
    defines { "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS" }

filter { "action:vs*", "platforms:Win32" }
    buildoptions { "/arch:IA32" } -- disable SSE/SSE2

filter "system:windows"
    staticruntime "On"
    targetprefix ""
