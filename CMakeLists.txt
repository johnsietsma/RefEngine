cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

set(PROJECT_NAME RefEngine)
project(${PROJECT_NAME})

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateEngineExecutable.cmake")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


# --- Platform setup ---
IF(APPLE)
    SET(GCC_CHAR_IS_UNSIGNED_CHAR "-funsigned-char")
    SET(STD_C11 "-std=c++11 -stdlib=libc++")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CHAR_IS_UNSIGNED_CHAR} ${STD_C11}")
ELSE()
    set( MSVC_ARCH x86 )
    if( CMAKE_CL_64 )
        set( MSVC_ARCH x64 )
    endif()
ENDIF()



# --- Deps ---
include_directories(dep/glm)
add_definitions(-DGLM_SWIZZLE)

include_directories(dep/stb)

#set(CMAKE_PREFIX_PATH "d:/libs/glew-1.10.0")
#set(CMAKE_LIBRARY_PATH "d:/libs/glew-1.10.0/lib/Release/Win32/")
#find_package(GLEW)
#if (GLEW_FOUND)
#    include_directories(${GLEW_INCLUDE_DIRS})
#    link_libraries(${GLEW_LIBRARIES})
#endif()

find_package(Qt5Widgets)



# --- Assets ---

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE STRING "Path to the assets.")
set(SHADERS_DIR "${ASSETS_DIR}/shaders" CACHE STRING "Path to the shaders.")

file(GLOB SHADER_FILES ${SHADERS_DIR}/*.*)



# -----
add_subdirectory(Engine)
add_subdirectory(EngineGLFW)


add_subdirectory(TestBedGLFW)

if( ${QT_FOUND} ) 
    add_subdirectory(EngineQT)
    add_subdirectory(TestBedQT)
endif()
