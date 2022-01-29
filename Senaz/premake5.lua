project "Senaz"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir("%{wks.location}/bin/" .. output_dir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. output_dir .. "/%{prj.name}")

	files
	{
		"**.h",
		"**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Polos/external/spdlog/include",
		"%{wks.location}/Polos/engine"
	}

	links
	{
		"Polos"
	}

	filter "system:Windows"
		staticruntime "On"
		systemversion "latest"

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
		optimize "On"