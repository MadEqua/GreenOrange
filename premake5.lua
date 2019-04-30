workspace "GreenOrange"
    startproject "GreenOrange"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "GreenOrange"
include "GreenOrange/dependencies/glfw_premake5.lua"
include "GreenOrange/dependencies/glad"
include "GreenOrange/dependencies/imgui_premake5.lua"