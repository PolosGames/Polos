project "Optick"
 	kind "StaticLib"
	language "C++"
	cppdialect "C++11"

	targetdir ("bin/" .. output_dir .. "/%{prj.name}")
	objdir ("bin-int/" .. output_dir .. "/%{prj.name}")

	flags { "NoManifest", "FatalWarnings" }
	warnings "Extra"

	includedirs
	{
		"include",
		"api"
	}

	defines { "_CRT_SECURE_NO_WARNINGS", "OPTICK_LIB=1" }
	defines { "USE_OPTICK=1"}
	defines { "OPTICK_FIBERS=1"}

--	if isDX12 then
--		links { 
--			"d3d12", 
--			"dxgi",
--		}
--	else
		defines { "OPTICK_ENABLE_GPU_D3D12=0" }
--	end
	
--	if isVulkan then
--		includedirs
--		{
--			"$(VULKAN_SDK)/Include",
--		}
--		libdirs {
--			"$(VULKAN_SDK)/Lib",
--		}
--		links { 
--			"vulkan-1",
--		}
--	else
		defines { "OPTICK_ENABLE_GPU_VULKAN=0" }
--	end
	
	files {
		"src/**.cpp",
        "include/**.h", 
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		staticruntime "On"
		
		links { "pthread" }
	
	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		defines { "NDEBUG"}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		defines { "_DEBUG", "_CRTDBG_MAP_ALLOC" }