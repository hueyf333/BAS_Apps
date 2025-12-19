cmake_minimum_required(VERSION 3.16)
project(BAS_Apps LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bas_studio
    src/main.cpp
    src/AudioEngine.cpp
    src/AudioBuffer.cpp
    src/Clip.cpp
    src/Track.cpp
    src/Mixer.cpp
    src/Transport.cpp
    src/Project.cpp
    src/Automation.cpp
    src/StudioUI.cpp
)

target_include_directories(bas_studio PRIVATE include)
