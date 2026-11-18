cmake_minimum_required(VERSION 3.25)

file(READ "${INPUT}" shader_hex HEX)
file(SIZE "${INPUT}" shader_size)
string(REGEX REPLACE "(..)" "std::byte{0x\\1}, " shader_bytes "${shader_hex}")

# Wrap every four bytes to keep the initializer readable in build output.
set(byte "std::byte[{]0x[0-9a-f][0-9a-f][}], ")
string(REGEX REPLACE "(${byte}${byte}${byte}${byte})" "    \\1\n" shader_bytes "${shader_bytes}")
string(REPLACE " \n" "\n" shader_bytes "${shader_bytes}")

# Separate build directories can generate the same source file concurrently.
string(RANDOM LENGTH 16 nonce)
set(temporary_output "${OUTPUT}.${nonce}.tmp")
file(WRITE "${temporary_output}"
    "// Generated from SPIR-V by EmbedShader.cmake.\n"
    "#include <array>\n"
    "#include <cstddef>\n\n"
    "namespace polos::rendering::shader::${SHADER_NAMESPACE}\n"
    "{\n"
    "static constexpr std::array<std::byte, ${shader_size}> kData{\n"
    "${shader_bytes}"
    "};\n"
    "}\n"
)
file(RENAME "${temporary_output}" "${OUTPUT}")
