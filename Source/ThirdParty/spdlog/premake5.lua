project "spdlog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"include/**.h",
		"src/**.cpp"
	}

	includedirs { "include" }

	defines { "SPDLOG_COMPILED_LIB" }

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		disablewarnings { "4251", "4275" }

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"
	
	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"