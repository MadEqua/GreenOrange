project "GlslToHeader"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir("bin/" .. outputDir)
    debugdir("bin/" .. outputDir)
    objdir("bin-int/" .. outputDir)

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        optimize "off"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        symbols "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"