
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
    
    if(${USE_QT})
		# AUTO_MOC/AUTO_UIC fails in subdirs! Do it manually
		file(GLOB_RECURSE UIS "src/*.ui")
		qt5_wrap_cpp(Window_hdr_moc ${HEADERS})
		qt5_wrap_ui(Window_ui_moc  ${UIS})
		include_directories("${PROJECT_BINARY_DIR}")
    endif(${USE_QT})

    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADER_FILES} ${Window_hdr_moc} ${Window_ui_moc})

    # Group the files in Visual Studio
    source_group(Headers FILES ${HEADERS} ${Window_hdr_moc} ${Window_ui_moc})
    source_group(Source FILES ${SOURCES})
    source_group(Assets ${ASSETS_DIR})
    source_group(Shaders FILES ${SHADER_FILES})

    # Add the engine library as a dependency
    if(${USE_GLFW})
		target_link_libraries (${PROJECT_NAME} RefEngineGLFW)
    endif()
    
    if(${USE_QT})
        target_link_libraries (${PROJECT_NAME} RefEngineQT)

		# Hmm, can't figure out how to do per-config commands, copying all the dlls.
		add_custom_command(
			TARGET ${PROJECT_NAME}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:Qt5::Core,LOCATION> $<TARGET_FILE_DIR:RefEngineQT>
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:Qt5::Widgets,LOCATION> $<TARGET_FILE_DIR:RefEngineQT>
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:Qt5::Core,LOCATION_Debug> $<TARGET_FILE_DIR:RefEngineQT>
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:Qt5::Gui,LOCATION_Debug> $<TARGET_FILE_DIR:RefEngineQT>
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:Qt5::Widgets,LOCATION_Debug> $<TARGET_FILE_DIR:RefEngineQT>
			COMMENT "Copying Qt dlls" 
		)
    endif()

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
