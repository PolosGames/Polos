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
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"engine/**.h",
		"engine/**.cpp"
	}

	includedirs
	{
		"external/spdlog/include",
		"engine"
	}

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..output_dir.. "/Senaz")
		}

	filter "configurations:Debug"
		defines "PL_DEBUG"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		defines "PL_RELEASE"
		runtime "Release"
		optimize "On"
	
	filter "configurations:Dist"
		defines "PL_DIST"
		runtime "Release"
		optimize "On"