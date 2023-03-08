project "Zenit Editor"
    --kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/")
    objdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/Int")

    files
    {
        "Source/**.h",
        "Source/**.cpp",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    characterset("MBCS")

    includedirs
    {
        "Source",
        "%{wks.location}/Zenit/Source",
        "%{wks.location}/Zenit/Dependencies",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.node_editor}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.parson}",
    }

    links
    {
        "Zenit",
    }

    

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "_CRT_SECURE_NO_WARNINGS",
        }

    filter "configurations:Debug"
        -- defines here
        kind "ConsoleApp"
        defines {"SPDLOG_COMPILED_LIB", "ZN_DEBUG"}
        runtime "Debug"
        buildoptions "/MTd"
        symbols "On"

    filter "configurations:Release"
        -- defines here
        defines {"SPDLOG_COMPILED_LIB", "ZN_RELEASE"}
        kind "WindowedApp"
        runtime "Release"
        buildoptions "/MT"
        optimize "On"
