include "Dependencies.lua"

workspace "Zenit"
	architecture "x86"
	startproject "Zenit"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "Build/%{cfg.buildcfg}"

group "Dependencies"
	include "Zenit/Dependencies/GLFW"
	include "Zenit/Dependencies/GLAD"
	include "Zenit/Dependencies/ImGui"
group ""

include "Zenit"
