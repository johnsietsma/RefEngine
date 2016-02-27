set(PROJECT_NAME TestBed)
project(${PROJECT_NAME})

find_package(VLD)

set(USE_VLD ${VLD_FOUND} CACHE BOOL "Use Visual Leak Detector")

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/src/config.h"
)
include_directories("${PROJECT_BINARY_DIR}/src")

include_directories(src/)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${ASSET_FILES})

# Group the files in Visual Studio
source_group(TestBed FILES ${HEADERS} ${SOURCES})
source_group(Assets FILES ${ASSET_FILES})

# Add the engine ilbrary as a dependency
target_link_libraries (${PROJECT_NAME} Engine)

# --- Asset copying ---
IF(APPLE)
    set(TARGET_ASSETS_DIR $<TARGET_FILE_DIR:TestBed>/data)
ELSE(APPLE)
    set(TARGET_ASSETS_DIR $<TARGET_FILE_DIR:TestBed>/../data)
ENDIF (APPLE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_ASSETS_DIR} COMMENT "Copying assets to output directory."
)
