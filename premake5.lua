include "external_depends.lua"

workspace "Polos"
	architecture "x64"
	startproject "Senaz"

	configurations
	{
		"Debug",
		"Release"
	}

output_dir  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Polos"
include "Senaz"