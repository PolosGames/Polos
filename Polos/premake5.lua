project "Polos"
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

	includedirs
	{ 	
		"engine",
		includes["spdlog"]
	}

	for k, v in pairs(gfxapi_includes) do includedirs {v} end

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..output_dir.. "/Senaz") }

	filter "configurations:Debug"
		defines "PL_DEBUG"
		runtime "Debug"
		symbols "On"

		for k, v in pairs(gfxapi_libs_d) do
			links {v}
		end


	
	filter "configurations:Release"
		defines "PL_RELEASE"
		runtime "Release"
		optimize "On"

		for k, v in pairs(gfxapi_libs_r) do
			links {v}
		end