// #include <glslang/Include/glslang_c_interface.h>

// // Required for use of glslang_default_resource
// #include <glslang/Public/resource_limits_c.h>

// #include <cstddef>
// #include <cstdint>
// #include <cstring>
// #include <filesystem>
// #include <fstream>
// #include <print>
// #include <vector>

// struct SpirVBinary
// {
//     std::uint32_t* words;// SPIR-V words
//     std::size_t    size; // number of words in SPIR-V binary
// };

// namespace
// {

// struct alignas(128) resource// NOLINT
// {
//     std::size_t            uncompressed_size;
//     std::size_t            size;
//     std::string            stem_name;
//     std::filesystem::path  path;
//     std::vector<std::byte> data;
// };

// auto ReadFile(std::filesystem::path const& t_file_path) -> resource
// {
//     std::string const file_name = t_file_path.filename().string();
//     std::println("Reading file: {}", file_name);

//     std::ifstream file{t_file_path, std::ios::ate | std::ios::binary};

//     if (!file.is_open())
//     {
//         std::println("Error: Could not open file: {}", file_name);
//         return {};
//     }

//     if (file.fail())
//     {
//         std::println("Error: {}, File: {}", std::strerror(errno), file_name);// NOLINT
//         return {};
//     }
//     std::println("File has been opened successfully.");

//     std::size_t const size = static_cast<std::size_t>(file.tellg());
//     std::println("File size: {}", size);

//     std::vector<std::byte> data{size};
//     file.seekg(0);
//     file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));// NOLINT
//     std::println("Expected file read was {} bytes, got {} bytes", size, file.gcount());

//     return resource{
//         .uncompressed_size = size,
//         .size              = size,
//         .stem_name         = t_file_path.stem().string(),
//         .path              = t_file_path,
//         .data              = data,
//     };
// }

// SpirVBinary compileShaderToSPIRV_Vulkan(glslang_stage_t stage, const char* shaderSource, const char* fileName)
// {
//     const glslang_input_t input = {
//         .language                          = GLSLANG_SOURCE_GLSL,
//         .stage                             = stage,
//         .client                            = GLSLANG_CLIENT_VULKAN,
//         .client_version                    = GLSLANG_TARGET_VULKAN_1_2,
//         .target_language                   = GLSLANG_TARGET_SPV,
//         .target_language_version           = GLSLANG_TARGET_SPV_1_5,
//         .code                              = shaderSource,
//         .default_version                   = 100,
//         .default_profile                   = GLSLANG_NO_PROFILE,
//         .force_default_version_and_profile = 0,
//         .forward_compatible                = 0,
//         .messages                          = GLSLANG_MSG_DEFAULT_BIT,
//         .resource                          = glslang_default_resource(),
//     };

//     glslang_shader_t* shader = glslang_shader_create(&input);

//     SpirVBinary bin = {
//         .words = NULL,
//         .size  = 0,
//     };
//     if (0 == glslang_shader_preprocess(shader, &input))
//     {
//         std::println("GLSL preprocessing failed %s", fileName);
//         std::println("%s", glslang_shader_get_info_log(shader));
//         std::println("%s", glslang_shader_get_info_debug_log(shader));
//         std::println("%s", input.code);
//         glslang_shader_delete(shader);
//         return bin;
//     }

//     if (0 == glslang_shader_parse(shader, &input))
//     {
//         std::println("GLSL parsing failed %s", fileName);
//         std::println("%s", glslang_shader_get_info_log(shader));
//         std::println("%s", glslang_shader_get_info_debug_log(shader));
//         std::println("%s", glslang_shader_get_preprocessed_code(shader));
//         glslang_shader_delete(shader);
//         return bin;
//     }

//     glslang_program_t* program = glslang_program_create();
//     glslang_program_add_shader(program, shader);

//     if (0 == glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
//     {
//         std::println("GLSL linking failed %s", fileName);
//         std::println("%s", glslang_program_get_info_log(program));
//         std::println("%s", glslang_program_get_info_debug_log(program));
//         glslang_program_delete(program);
//         glslang_shader_delete(shader);
//         return bin;
//     }

//     glslang_program_SPIRV_generate(program, stage);

//     bin.size  = glslang_program_SPIRV_get_size(program);
//     bin.words = reinterpret_cast<std::uint32_t*>(malloc(bin.size * sizeof(uint32_t)));// NOLINT
//     glslang_program_SPIRV_get(program, bin.words);

//     const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
//     if (nullptr != spirv_messages)
//     {
//         std::println("GLSL SPIR-V generation messages %s", fileName);
//         std::println("%s", spirv_messages);
//     }

//     glslang_program_delete(program);
//     glslang_shader_delete(shader);

//     return bin;
// }

// }// namespace

// #cmakedefine BASIC_COLOR_FRAG_LOCATION "@BASIC_COLOR_FRAG_LOCATION@"
// #cmakedefine BASIC_COLOR_VERT_LOCATION "@BASIC_COLOR_VERT_LOCATION@"

// int main()
// {
//     glslang_initialize_process();

//     const char* basic_color_frag_location = BASIC_COLOR_FRAG_LOCATION;
//     const char* basic_color_vert_location = BASIC_COLOR_VERT_LOCATION;

//     // Read shader sources
//     std::string const basic_color_frag_source = reinterpret_cast<const char*>(
//         ReadFile(std::filesystem::path{basic_color_frag_location}).data.data());

//     std::string const basic_color_vert_source(reinterpret_cast<const char*>(
//         ReadFile(std::filesystem::path{basic_color_vert_location}).data.data()), );

//     // Compile shaders to SPIR-V
//     SpirVBinary frag_spirv =
//         compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_FRAGMENT, basic_color_frag_source.c_str(), basic_color_frag_location);
//     SpirVBinary vert_spirv =
//         compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_VERTEX, basic_color_vert_source.c_str(), basic_color_vert_location);

//     // Clean up
//     glslang_finalize_process();

//     return 0;
// }
