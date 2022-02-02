includes = {}
includes["spdlog"] = "%{wks.location}/Polos/external/spdlog/include"
includes["GLFW"] = "%{wks.location}/Polos/external/GLFW/include"
includes["glad"] = "%{wks.location}/Polos/external/glad/include"

libraries_d = {}
libraries_d["spdlog"] = "%{wks.location}/Polos/external/spdlog/lib/debug/spdlogd"
libraries_d["GLFW"] = "%{wks.location}/Polos/external/glfw/lib/debug/glfw3"
libraries_d["opengl"] = "opengl32"

libraries_r = {}
libraries_r["spdlog"] = "%{wks.location}/Polos/external/spdlog/lib/release/spdlog"
libraries_r["GLFW"] = "%{wks.location}/Polos/external/glfw/lib/release/glfw3"
libraries_r["opengl"] = "opengl32"

-- Graphics API Selection

ignored_files = {}

gfxapi_libs_d = {}
gfxapi_libs_r = {}
gfxapi_includes = {}

graphics_api = "USE_OPENGL"

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