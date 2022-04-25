include "Dependencies.lua"

workspace "Little Engine"
	architecture "x64"
	startproject "Little Editor"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "Build/%{cfg.buildcfg}"

group "Dependencies"
	include "Little Engine/Dependencies/GLFW"
	include "Little Engine/Dependencies/GLAD"
	include "Little Engine/Dependencies/ImGui"
group ""

include "Little Engine"
include "Sandbox"
include "Little Editor"