function(build_libraries)

    set(LIBRARIES
            glad
            glfw
    )

    foreach (LIBRARY ${LIBRARIES})
        set(LIBRARY_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/${LIBRARY})
        set(LIBRARY_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/libs/${LIBRARY})

        if (NOT EXISTS ${LIBRARY_DIRECTORY}/CMakeLists.txt)
            message("> Couldn't find CMakeLists.txt for '${LIBRARY}'.")
            continue()
        endif ()

        message("> Building '${LIBRARY}'...")
        add_subdirectory(${LIBRARY_DIRECTORY} ${LIBRARY_BINARY_DIRECTORY})
    endforeach ()

endfunction()

function(link_libraries TARGET LIBRARIES)

    foreach (LIBRARY ${LIBRARIES})
        message("> Linking ${LIBRARY} to ${TARGET}...")

        target_link_libraries(${TARGET} PRIVATE ${LIBRARY})
        target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/libs/${LIBRARY}/include)
    endforeach ()

endfunction()
