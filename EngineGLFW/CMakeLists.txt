set(PROJECT_NAME RefEngineGLFW)
project(${PROJECT_NAME})

include_directories(dep/glfw/include)


# --- Project setup ---
include_directories(src/)

file(GLOB_RECURSE SOURCES "src/*.c*")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
source_group(Headers FILES ${HEADERS})
source_group(Source FILES ${SOURCES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries (${PROJECT_NAME} RefEngine)


# --- GFLW ---
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" ON)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/dep/glfw ${CMAKE_BINARY_DIR}/dep/glfw)
set_property(TARGET glfw APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
