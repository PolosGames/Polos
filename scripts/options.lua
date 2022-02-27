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