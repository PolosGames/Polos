project "Runtime"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. output_dir .. "/%{prj.name}")

	pchheader "plpch.h"
	pchsource "engine/plpch.cpp"

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		graphics_api,
		"GLFW_INCLUDE_NONE"
	}

	files
	{
		"engine/**.h",
		"engine/**.cpp"
	}

	includedirs
	{ 	
		"engine/core",
		"engine/platform",
		"engine/renderer",
		"engine/window",
		"engine",
		includes["spdlog"],
		includes["Optick"]
	}

	links
	{
		"Optick",
		"spdlog"
	}

	buildoptions { "/Zc:__cplusplus" }

	for k, v in pairs(gfxapi_includes) do includedirs {v} end
	for k, v in pairs(gfxapi_libs) do links {v} end

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..output_dir.. "/Sandbox") }

	filter "configurations:Debug"
		defines "PL_DEBUG"
		runtime "Debug"
		symbols "On"

		defines { "PL_PROFILING=%{ENABLE_PROFILING}" }
	
	filter "configurations:Release"
		defines "PL_RELEASE"
		runtime "Release"
		optimize "On"

		defines { "PL_PROFILING=0" }