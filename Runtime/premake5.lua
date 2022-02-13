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
		graphics_api
	}

	files
	{
		"engine/**.h",
		"engine/**.cpp"
	}

	filter "options:gfxapi=opengl or options:gfxapi=vulkan"
		files { "external/glad/src/glad.c" }
		defines { "GLFW_INCLUDE_NONE" }

	includedirs
	{ 	
		"engine/core",
		"engine/platform",
		"engine/renderer",
		"engine/window",
		"engine",
		includes["spdlog"]
	}

	for k, v in pairs(gfxapi_includes) do includedirs {v} end

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..output_dir.. "/Sandbox") }

	filter "configurations:Debug"
		defines "PL_DEBUG"
		runtime "Debug"
		symbols "On"

		for k, v in pairs(gfxapi_libs_d) do
			links {v}
		end

		filter "system:Windows"
			postbuildcommands { ("{COPY} " ..gfxapi_libs_d["GLFW"].. "/" "../bin/" ..output_dir.. "/Sandbox") }
	
	filter "configurations:Release"
		defines "PL_RELEASE"
		runtime "Release"
		optimize "On"

		for k, v in pairs(gfxapi_libs_r) do
			links {v}
		end

	filter "files:**.c"
		flags {"NoPCH"}