project "Zenit"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/")
    objdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/Int")

    --pchheader "znpch.h"
    --pchsource "Zenit/Source/znpch.cpp"

    files
    {
        "Source/**.h",
        "Source/**.cpp",
        "%{prj.location}/Dependencies/stb_image/**.h",
        "%{prj.location}/Dependencies/stb_image/**.cpp",
        "%{prj.location}/Dependencies/spdlog/src/**.h",
        "%{prj.location}/Dependencies/spdlog/src/**.cpp",
        "%{prj.location}/Dependencies/spdlog/src/**.cpp",
        "%{prj.location}/Dependencies/parson/parson.h",
        "%{prj.location}/Dependencies/parson/parson.c",

        "%{prj.location}/Dependencies/imgui-node-editor/crude_json.h",
        "%{prj.location}/Dependencies/imgui-node-editor/crude_json.cpp",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_bezier_math.h",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_canvas.h",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_canvas.cpp",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_extra_math.h",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_node_editor.h",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_node_editor.cpp",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_node_editor_api.h",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_node_editor_api.cpp",
        "%{prj.location}/Dependencies/imgui-node-editor/imgui_node_editor_internal.h",
    }

    characterset ("MBCS")

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
        "%{IncludeDir.assimp}",
        "%{IncludeDir.node_editor}",
        "%{IncludeDir.parson}",
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "assimp",
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
        buildoptions "/MTd"
        symbols "On"

    filter "configurations:Release"
        defines {"SPDLOG_COMPILED_LIB"}
        runtime "Release"
        buildoptions "/MT"
        optimize "On"
