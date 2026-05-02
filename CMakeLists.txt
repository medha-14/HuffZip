cmake_minimum_required(VERSION 3.14)
project(huffman_compressor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Shared sources (no main)
set(SHARED_SRCS
    src/huffman.cpp
    src/bitstream.cpp
    src/file_io.cpp
)

# -----------------------------------------------------------------------
# compress  —  ./compress <input_file>
# -----------------------------------------------------------------------
add_executable(compress
    src/compress.cpp
    ${SHARED_SRCS}
)
target_include_directories(compress PRIVATE include)
target_compile_options(compress PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------------------------------------------------------
# decompress  —  ./decompress <file.huff>
# -----------------------------------------------------------------------
add_executable(decompress
    src/decompress.cpp
    ${SHARED_SRCS}
)
target_include_directories(decompress PRIVATE include)
target_compile_options(decompress PRIVATE -Wall -Wextra -Wpedantic)
