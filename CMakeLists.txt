cmake_minimum_required(VERSION 3.10)

project(Donut VERSION 0.2 DESCRIPTION "Donut firmware for EL-ONE hardware synthesizer")

file(GLOB sources_1 "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB sources_2 "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/*.cpp")

find_library (jack_lib jack)
message("jack: ${jack_lib}")

add_executable(donut ${sources_1} ${sources_2})
target_link_libraries(donut PRIVATE
        ${jack_lib}
        -lncurses
        $<$<PLATFORM_ID:Linux>:-pthread>)

if (APPLE)
    target_link_libraries(donut PRIVATE
            "-framework CoreMIDI"
            "-framework CoreAudio"
            "-framework CoreFoundation")
endif()

target_compile_options(donut PRIVATE
        -Wall
        -fexceptions)

target_compile_features(donut PRIVATE cxx_std_20)
target_compile_definitions(donut PRIVATE
        $<$<PLATFORM_ID:Windows>:__WINDOWS_MM__>
        $<$<PLATFORM_ID:Linux>:__UNIX_JACK__>
        $<$<PLATFORM_ID:Darwin>:__MACOSX_CORE__>)

target_include_directories(donut PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(donut PRIVATE "/usr/local/include")


