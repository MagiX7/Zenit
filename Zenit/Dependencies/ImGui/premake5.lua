project "ImGui"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	targetdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/Int")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
		"misc/cpp/imgui_stdlib.h",
		"misc/cpp/imgui_stdlib.cpp"
	}

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
