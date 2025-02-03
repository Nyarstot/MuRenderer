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
            "dxgidebug.dll"
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