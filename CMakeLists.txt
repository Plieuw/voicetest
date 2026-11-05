cmake_minimum_required(VERSION 3.16)
project(voicetest C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(voicetest_dsp
    firmware/src/voice_engine.c
)
target_include_directories(voicetest_dsp PUBLIC firmware/include)

if (MSVC)
    target_compile_options(voicetest_dsp PRIVATE /W4)
else()
    target_compile_options(voicetest_dsp PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(test_voice_engine tests/test_voice_engine.c)
target_link_libraries(test_voice_engine PRIVATE voicetest_dsp)
add_test(NAME voice_engine COMMAND test_voice_engine)
