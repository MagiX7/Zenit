project "Zenit"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/")
    objdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/Int")

    files
    {
        "Source/**.h",
        "Source/**.cpp",
        "%{prj.location}/Dependencies/stb_image/**.h",
        "%{prj.location}/Dependencies/stb_image/**.cpp",
        "%{prj.location}/Dependencies/spdlog/src/**.h",
        "%{prj.location}/Dependencies/spdlog/src/**.cpp",
        "%{prj.location}/Dependencies/spdlog/include/**.h",
        "%{prj.location}/Dependencies/spdlog/include/**.cpp",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "Source",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.spdlog}",
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
        }

    filter "configurations:Debug"
        defines {"SPDLOG_COMPILED_LIB"}
        runtime "Debug"
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines {"SPDLOG_COMPILED_LIB"}
        runtime "Release"
        buildoptions "/MD"
        optimize "On"
