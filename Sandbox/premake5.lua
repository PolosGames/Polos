project "Sandbox"
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
		"%{wks.location}/Runtime/engine",
		includes["spdlog"],
		includes["Optick"]
	}

	links
	{
		"Runtime"
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