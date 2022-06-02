cmake_minimum_required(VERSION 3.18)

project(Donut VERSION 0.2 DESCRIPTION "Donut firmware for EL-ONE hardware synthesizer")

file(GLOB sources_1 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB sources_2 "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp")

## Find dependencies
add_subdirectory("imgui")

find_library (jack_lib jack)
message("jack: ${jack_lib}")

add_definitions( -DBUILD_PLATFORM=${PLATFORM_ID} )
add_definitions( -DBUILD_ENVIRONMENT=debug )
#add_definitions( -DBUILD_GUI_COUT )
#add_definitions( -DBUILD_GUI_NCURSES )
add_definitions( -DBUILD_GUI_IMGUI )

# Configure build options (features, sound engines, etc)
add_definitions( -DFEATURES_PADS )
add_definitions( -DFEATURES_MODMATRIX )
#add_definitions( -DENGINES_PARTICLES )

if(UNIX AND NOT APPLE)
    # finds OpenGL, GLU and X11
    find_package(OpenGL REQUIRED)
    if(NOT OPENGL_FOUND)
        message("ERROR: OpenGL not found")
    endif(NOT OPENGL_FOUND)
    set(GL_LIBRARY GL GLU X11)
endif()

add_executable(donut ${sources_1} ${sources_2})

if(UNIX AND NOT APPLE)
    target_link_libraries(donut PRIVATE glfw3 ${GL_LIBRARY} dl m)
    add_definitions( -DSYS_LINUX )
endif()

target_link_libraries(donut PRIVATE
        ${jack_lib}
        -lncurses
        $<$<PLATFORM_ID:Linux>:-pthread>)

if (APPLE)
    target_link_libraries(donut PRIVATE
#            MIDI stuff
            "-framework CoreMIDI"
#            Audio stuff
            "-framework CoreAudio"
            "-framework CoreFoundation"
#            Graphics stuff
            "-framework Cocoa"
            "-framework OpenGL"
            "-framework IOKit"
            "-framework CoreVideo"
            "-L/usr/local/lib"
            "-L/opt/homebrew/lib"
            "-lglfw")
endif()

target_link_libraries(donut PRIVATE imgui)
target_compile_options(donut PRIVATE
        -Wall
        -Wformat
        -fexceptions)

target_compile_features(donut PRIVATE cxx_std_20)
target_compile_definitions(donut PRIVATE
        $<$<PLATFORM_ID:Windows>:__WINDOWS_MM__>
        $<$<PLATFORM_ID:Linux>:__UNIX_JACK__>
        $<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>)

target_include_directories(donut PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(donut PRIVATE "/usr/local/include")
target_include_directories(donut PRIVATE "/opt/homebrew/include")
