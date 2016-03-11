
# Generate an executable in the provide project that links in the Engine library
function (GenerateEngineExecutable PROJECT_NAME)
    find_package(VLD)
    set(USE_VLD ${VLD_FOUND})

    configure_file(
        "${PROJECT_SOURCE_DIR}/src/config.h.in"
        "${PROJECT_BINARY_DIR}/src/config.h"
    )
    include_directories("${PROJECT_BINARY_DIR}/src")

    include_directories(src/)

    file(GLOB_RECURSE SOURCES "src/*.cpp")
    file(GLOB_RECURSE HEADERS "src/*.h")

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADER_FILES})

    # Group the files in Visual Studio
    source_group(Headers FILES ${HEADERS})
    source_group(Source FILES ${SOURCES})
    source_group(Assets ${ASSETS_DIR})
    source_group(Shaders FILES ${SHADER_FILES})

    # Add the engine ilbrary as a dependency
    target_link_libraries (${PROJECT_NAME} RefEngineGLFW)

    # --- Asset copying ---
    IF(APPLE)
        set(TARGET_ASSETS_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)
    ELSE(APPLE)
        set(TARGET_ASSETS_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/../data)
    ENDIF (APPLE)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${TARGET_ASSETS_DIR} COMMENT "Copying assets to output directory."
    )

endfunction(GenerateEngineExecutable)
