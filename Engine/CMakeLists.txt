set(PROJECT_NAME Engine)
project(${PROJECT_NAME})

# --- Platform setup ---
IF(APPLE)
ELSE(APPLE)
    set( MSVC_ARCH x86 )
    if( CMAKE_CL_64 )
        set( MSVC_ARCH x64 )
    endif()
ENDIF()

include_directories(src/)

file(GLOB_RECURSE SOURCES "src/*.c*")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
source_group(Engine FILES ${HEADERS} ${SOURCES})


# --- OpenGL ---
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})


# --- FBX SDK ---
find_package(FBX REQUIRED)

include_directories(${FBX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} optimized ${FBX_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${FBX_LIBRARY_DEBUG})


# --- GFLW ---
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" ON)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/dep/glfw ${CMAKE_BINARY_DIR}/dep/glfw)
set_property(TARGET glfw APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


if(APPLE)
    INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
    FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)

    target_link_libraries(${PROJECT_NAME} ${CORE_FOUNDATION_LIBRARY})
endif()

