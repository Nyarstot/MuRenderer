workspace "MuRenderer"
    architecture "x64"
    startproject "MuRenderer"
    configurations {"Debug", "Release"}

    buildpattern = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"

    project "MuRenderer"
        location "MuRenderer"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        characterset "MBCS"
        staticruntime "on"

        targetdir("build/bin/" .. buildpattern .. "/%{prj.name}")
        objdir("build/int/" .. buildpattern .. "/%{prj.name}")

        pchheader "pch.hpp"
        pchsource "%{wks.location}/%{prj.name}/source/pch.cpp"

        files
        {
            "%{wks.location}/%{prj.name}/include/**.hpp",
            "%{wks.location}/%{prj.name}/source/**.cpp"
        }

        includedirs
        {
            "%{wks.location}/%{prj.name}/include"
        }

        links
        {
            "d3d12.lib",
            "dxgi.lib",
            "d3dcompiler.lib",
            "dxguid.lib",
            "dxgidebug.dll",
            "Advapi32.lib",
            "user32.lib",
            "Gdi32.lib"
        }

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "MU_WINDOWS"
            }

        filter "configurations:Release"
            optimize "on"

            defines
            {
                "MU_RELEASE"
            }