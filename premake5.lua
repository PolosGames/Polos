workspace "Polos"
	architecture "x86_64"
	startproject "Senaz"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

output_dir  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Polos"
include "Senaz"