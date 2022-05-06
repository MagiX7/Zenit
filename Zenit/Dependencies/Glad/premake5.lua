project "GLAD"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/" .. outputdir .. "/%{prj.name}/Int")

	files
	{
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	includedirs
	{
		"include"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
