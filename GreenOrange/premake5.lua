project "GreenOrange"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir("bin/" .. outputDir)
    debugdir("bin/" .. outputDir)
    objdir("bin-int/" .. outputDir)

    dependson("GlslToHeader")
    -- Not pretty. Find a better way.
    prebuildcommands { "%{wks.location}GlslToHeader/bin/" .. outputDir .. "/GlslToHeader.exe %{prj.location}src/glsl/template.frag" }
    prebuildcommands { "%{wks.location}GlslToHeader/bin/" .. outputDir .. "/GlslToHeader.exe %{prj.location}src/glsl/fullScreen.vert" }
    prebuildcommands { "%{wks.location}GlslToHeader/bin/" .. outputDir .. "/GlslToHeader.exe %{prj.location}src/glsl/fallback.frag" }

    -- Include directories relative to root folder
    includeDir = {}
    includeDir["GLFW"] = "dependencies/GLFW/include"
    includeDir["glad"] = "dependencies/glad/include"
    includeDir["ImGui"] = "dependencies/ImGui"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{includeDir.GLFW}",
        "%{includeDir.glad}",
        "%{includeDir.ImGui}",
        "%{includeDir.ImGui}/examples"
    }

    links
    {
        "GLFW",
        "glad",
        "ImGui",
        "opengl32.lib"
    }

    defines
    {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "GLFW_INCLUDE_NONE"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "GO_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "GO_DEBUG"
        runtime "Debug"
        optimize "off"
        symbols "on"

    filter "configurations:Release"
        defines "GO_RELEASE"
        runtime "Release"
        optimize "on"
        symbols "on"

    filter "configurations:Dist"
        defines "GO_DIST"
        runtime "Release"
        optimize "on"
        symbols "off"