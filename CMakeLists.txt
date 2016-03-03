cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

set(PROJECT_NAME OpenGLEngine)
project(${PROJECT_NAME})

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateEngineExecutable.cmake")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


# --- Platform specific complier options ---
IF(APPLE)
    SET(GCC_CHAR_IS_UNSIGNED_CHAR "-funsigned-char")
    SET(STD_C11 "-std=c++11 -stdlib=libc++")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
ELSE()
    set( MSVC_ARCH x86 )
    if( CMAKE_CL_64 )
        set( MSVC_ARCH x64 )
    endif()
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CHAR_IS_UNSIGNED_CHAR} ${STD_C11}")


# --- Assets ---

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE STRING "Path to the assets.")
set(SHADERS_DIR "${ASSETS_DIR}/shaders" CACHE STRING "Path to the shaders.")

file(GLOB SHADER_FILES ${SHADERS_DIR}/*.*)

# --- Deps ---
include_directories(dep/glm)
add_definitions(-DGLM_SWIZZLE)

include_directories(dep/stb)

include_directories(dep/glfw/include)


# -----
add_subdirectory(Engine)
add_subdirectory(TestBed)
