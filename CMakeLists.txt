cmake_minimum_required(VERSION 3.22)

project(Distortion)


set(CMAKE_CXX_STANDARD 23)

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

include(cmake/cpm.cmake)

CPMAddPackage(
        NAME JUCE
        GITHUB_REPOSITORY juce-framework/JUCE
        GIT_TAG 8.0.13
        VERSION 8.0.13
        SOURCE_DIR "${LIB_DIR}/juce"
)

CPMAddPackage(
        NAME GOOGLETEST
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.17.0
        VERSION 1.17.0
        SOURCE_DIR ${LIB_DIR}/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"


)

enable_testing()
add_subdirectory(plugin)
add_subdirectory(test)