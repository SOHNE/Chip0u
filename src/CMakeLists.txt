set(Chip0u_HEADER_FILES
        Application.h
        chip8/Chip8.h
)

set (CHIP_SOURCE_FILES
        main.cpp
        Application.cpp
        chip8/Chip8.cpp
)

add_executable(Chip0u)
target_sources(Chip0u PRIVATE ${CHIP_SOURCE_FILES} ${Chip0u_HEADER_FILES})
target_link_libraries(Chip0u PRIVATE vendor)
target_include_directories(Chip0u PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(Chip0u PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

# copy roms folder, if it exists. roms is on root
set(ROMS_DIR ${CMAKE_SOURCE_DIR}/roms)
if( EMSCRIPTEN )
    if (EXISTS ${ROMS_DIR})
        set_target_properties(Chip0u PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 --preload-file ${ROMS_DIR}@/roms --bind --use-preload-plugins")
    else()
        message(WARNING "No roms folder found. Please add a roms folder to the root of the project.")
    endif ()


    # Tell Emscripten to build an example.html file.
    # TODO: Add user drag and drop support (-s FORCE_FILESYSTEM=1)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --emrun -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 -s ALLOW_MEMORY_GROWTH")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set_target_properties(Chip0u PROPERTIES SUFFIX ".html")
    set_target_properties(Chip0u PROPERTIES OUTPUT_NAME "index")
else()
    if(EXISTS ${ROMS_DIR})
        add_custom_command(TARGET Chip0u POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${ROMS_DIR} $<TARGET_FILE_DIR:Chip0u>/roms)
    endif()
endif ()

if (APPLE)
    list(APPEND APPLE_FRAMEWORKS
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreFoundation"
            "-framework CoreVideo")
    target_link_libraries(Chip0u PRIVATE ${APPLE_FRAMEWORKS})
endif ()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CHIP_SOURCE_FILES} ${Chip0u_HEADER_FILES})
