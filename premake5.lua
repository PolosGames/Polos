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
includes["spdlog"] = "%{wks.location}/Runtime/external/spdlog/include"
includes["GLFW"] = "%{wks.location}/Runtime/external/GLFW/include"
includes["glad"] = "%{wks.location}/Runtime/external/glad/include"
includes["Optick"] = "%{wks.location}/Runtime/external/Optick/api"

libraries_d = {}
libraries_d["spdlog"] = "%{wks.location}/Runtime/external/spdlog/lib/debug/spdlogd"
libraries_d["GLFW"] = "%{wks.location}/Runtime/external/glfw/lib/debug/glfw3"
libraries_d["opengl"] = "opengl32"

libraries_r = {}
libraries_r["spdlog"] = "%{wks.location}/Runtime/external/spdlog/lib/release/spdlog"
libraries_r["GLFW"] = "%{wks.location}/Runtime/external/glfw/lib/release/glfw3"
libraries_r["opengl"] = "opengl32"

-- Graphics API Selection

ignored_files = {}

gfxapi_libs_d = {}
gfxapi_libs_r = {}
gfxapi_includes = {}

graphics_api = "USE_OPENGL"

if _OPTIONS["gfxapi"] == "opengl" then
   gfxapi_libs_d[0] = libraries_d["opengl"]
   gfxapi_libs_r[0] = libraries_r["opengl"]
   gfxapi_libs_d[1] = libraries_d["GLFW"]
   gfxapi_libs_r[2] = libraries_r["GLFW"]

   gfxapi_includes[0] = includes["GLFW"]
   gfxapi_includes[1] = includes["glad"]
elseif _OPTIONS["gfxapi"] == "vulkan" then
   graphics_api = "USE_VULKAN"
--   gfxapi_includes[0] = includes["vulkan"]
--   gfxapi_libs_d[0] = libraries_d["vulkan"]
--   gfxapi_libs_d[1] = libraries_r["vulkan"]

elseif _OPTIONS["gfxapi"] == "d3d11" then
   if not os.get() == "windows" then
      print("Not on Windows")
      exit()
   end
   graphics_api = "USE_D3D11"

elseif _OPTIONS["gfxapi"] == "d3d12" then
   if not os.get() == "windows" then
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

include "Runtime/external/Optick"

include "Runtime"
include "Sandbox"