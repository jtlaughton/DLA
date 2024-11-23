cmake_minimum_required(VERSION 3.30)
project(DLA)

set(CMAKE_CXX_STANDARD 20)

add_executable(DLA
        src/cpp/main.cpp
        glad.c
        src/headers/renderer.h
        src/cpp/renderer.cpp
        src/headers/Shader.h
        src/cpp/Shader.cpp
        src/headers/FastRandomHashMap.h
        src/cpp/FastRandomHashMap.cpp
        src/cpp/dlatexture.cpp
        src/headers/dlatexture.h)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
    message(FATAL_ERROR "The glfw submodules was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(external/glfw)

target_include_directories(${PROJECT_NAME}
        PUBLIC external/glfw/include
)

target_link_directories(${PROJECT_NAME}
        PRIVATE external/glfw/src
)

target_link_libraries(${PROJECT_NAME}
        glfw
)