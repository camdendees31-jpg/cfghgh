# include some defines automatically made by qpm
include(qpm_defines.cmake)

cmake_minimum_required(VERSION 3.22)
project(${COMPILE_ID})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    ${COMPILE_ID}
    SHARED
)

target_compile_options(${COMPILE_ID} PRIVATE -Wno-invalid-offsetof)
target_compile_options(${COMPILE_ID} PRIVATE -fdeclspec)
target_compile_definitions(${COMPILE_ID} PRIVATE -DMOD_ID="${MOD_ID}")
target_compile_definitions(${COMPILE_ID} PRIVATE -DVERSION="${MOD_VERSION}")

RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list ${SOURCE_DIR}/*.c)
target_sources(${COMPILE_ID} PRIVATE ${cpp_file_list} ${c_file_list})

target_include_directories(${COMPILE_ID} PRIVATE .)
target_include_directories(${COMPILE_ID} PRIVATE ${SHARED_DIR})
target_include_directories(${COMPILE_ID} PRIVATE ${EXTERN_DIR}/includes)
target_include_directories(${COMPILE_ID} PRIVATE ${INCLUDE_DIR})
target_include_directories(${COMPILE_ID} SYSTEM PRIVATE ${EXTERN_DIR}/includes/libil2cpp/il2cpp/external/baselib/Include)
target_include_directories(${COMPILE_ID} SYSTEM PRIVATE ${EXTERN_DIR}/includes/libil2cpp/il2cpp/external/baselib/Platforms/Android/Include)

target_link_libraries(${COMPILE_ID} PRIVATE -llog)

include(extern.cmake)
