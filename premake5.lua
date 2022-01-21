workspace "Polos"
	architecture "x64"
	startproject "Senaz"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

output_dir  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Polos"
	location "Polos"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/" .. output_dir .. "/%{prj.name}")
	objdir("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/engine/**.h",
		"%{prj.name}/engine/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/external/spdlog/include",
		"%{prj.name}/engine"
	}

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PL_PLATFORM_WINDOWS",
			"PL_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..output_dir.. "/Senaz")
		}

	filter "configurations:Debug"
		defines "PL_DEBUG"
		symbols "On"
		buildoptions "/MDd"
	
	filter "configurations:Release"
		defines "PL_RELEASE"
		optimize "On"
		buildoptions "/MD"
	
	filter "configurations:Dist"
		defines "PL_DIST"
		optimize "On"

project "Senaz"
	location "Senaz"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/" .. output_dir .. "/%{prj.name}")
	objdir("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs
	{
		"Polos/external/spdlog/include",
		"Polos/engine"
	}

	links
	{
		"Polos"
	}

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"PL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "PL_DEBUG"
		symbols "On"
		buildoptions "/MDd"
	
	filter "configurations:Release"
		defines "PL_RELEASE"
		optimize "On"
		buildoptions "/MD"
	
	filter "configurations:Dist"
		defines "PL_DIST"
		optimize "On"