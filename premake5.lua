newoption {
   trigger = "gfxapi",
   value = "API",
   description = "Choose a particular 3D API for rendering",
   allowed = {
      { "opengl",    "OpenGL (Default)" },
      { "vulkan",    "Vulkan" },
      { "direct11",  "DirectX11 (Windows only)" },
      { "direct12",  "DirectX12 (Windows only)" },     
   },
   default = "opengl"
}

-- Dependency Locations

includes = {}
includes["spdlog"] = "%{wks.location}/ThirdParty/spdlog/include"
includes["glfw"] = "%{wks.location}/ThirdParty/glfw/include"
includes["glad"] = "%{wks.location}/ThirdParty/glad/include"
includes["Optick"] = "%{wks.location}/ThirdParty/Optick/api"

graphics_api = "USE_OPENGL"

OS_NAME = os.get()

gfxapi_includes = {}
gfxapi_libs = {}

if _OPTIONS["gfxapi"] == "opengl" then
	gfxapi_includes[0] = includes["glfw"]
	gfxapi_includes[1] = includes["glad"]

	gfxapi_libs[0] = "glfw"
	gfxapi_libs[1] = "glad"
	gfxapi_libs[2] = "opengl32.lib"
elseif _OPTIONS["gfxapi"] == "vulkan" then
	graphics_api = "USE_VULKAN"

elseif _OPTIONS["gfxapi"] == "d3d11" then
   if not OS_NAME == "windows" then
      print("Not on Windows")
      exit()
   end
   graphics_api = "USE_D3D11"

elseif _OPTIONS["gfxapi"] == "d3d12" then
   if not OS_NAME == "windows" then
      print("Not on Windows")
      exit()
   end
   graphics_api = "USE_D3D12"
end

-- Solution Creation

workspace "Polos"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}

output_dir  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "ThirdParty"
	include "ThirdParty/Optick"
	include "ThirdParty/spdlog"
	include "ThirdParty/glfw"
	include "ThirdParty/glad"
group ""

include "Runtime"
include "Sandbox"