set(LIBRARIES
        glad
        glfw
)

function(build_libraries)
    foreach (LIBRARY ${LIBRARIES})
        message("Current library: '${LIBRARY}'.")

        set(LIBRARY_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/${LIBRARY})
        set(LIBRARY_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/libs/${LIBRARY})

        if (NOT EXISTS ${LIBRARY_DIRECTORY}/CMakeLists.txt)
            message("Couldn't find CMakeLists.txt for '${LIBRARY}'.")
            continue()
        endif ()

        message("Found CMakeLists.txt for '${LIBRARY}'. Building...")
        add_subdirectory(${LIBRARY_DIRECTORY} ${LIBRARY_BINARY_DIRECTORY})

        message("Linking...")

        target_link_libraries(Physarum PRIVATE ${LIBRARY})
        target_include_directories(Physarum PRIVATE ${LIBRARY_DIRECTORY}/include)
    endforeach ()
endfunction()
