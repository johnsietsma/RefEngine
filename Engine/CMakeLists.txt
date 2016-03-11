set(PROJECT_NAME RefEngine)
project(${PROJECT_NAME})


# --- Project setup ---
include_directories(src/)

file(GLOB_RECURSE SOURCES "src/*.c*")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
source_group(Headers FILES ${HEADERS})
source_group(Source FILES ${SOURCES})


# --- OpenGL ---
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})


# --- FBX SDK ---
find_package(FBX REQUIRED)

include_directories(${FBX_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} optimized ${FBX_LIBRARY})
target_link_libraries(${PROJECT_NAME} debug ${FBX_LIBRARY_DEBUG})



target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


if(APPLE)
    INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
    FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation)

    target_link_libraries(${PROJECT_NAME} ${CORE_FOUNDATION_LIBRARY})
endif()

