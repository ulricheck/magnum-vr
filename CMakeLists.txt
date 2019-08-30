
cmake_minimum_required(VERSION 3.1)

# Configuration fails on < 3.4 if only C++ is enabled (CheckFunctionExists
# macro called from FindX11)
if(NOT CMAKE_VERSION VERSION_LESS 3.4.0)
    set(LANG CXX)
endif()
project(MagnumVR ${LANG})

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()


# Use folders for nice tree in Visual Studio and XCode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/" ${CMAKE_MODULE_PATH})
find_package(Magnum REQUIRED)

# Parts of the library
option(WITH_OPENVR "Build OpenvrIntegration library" ON)

include(CMakeDependentOption)
option(BUILD_STATIC "Build static libraries (default are shared)" OFF)
cmake_dependent_option(BUILD_STATIC_PIC "Build static libraries with position-independent code" ON "BUILD_STATIC" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
cmake_dependent_option(BUILD_GL_TESTS "Build unit tests for OpenGL code" OFF "BUILD_TESTS" OFF)

if(BUILD_TESTS)
    find_package(Corrade REQUIRED TestSuite)
    if(CORRADE_TARGET_IOS)
        set(CORRADE_TESTSUITE_BUNDLE_IDENTIFIER_PREFIX "cz.mosra.magnum-vr")
    endif()
    enable_testing()
endif()

if(BUILD_GL_TESTS)
    find_package(Magnum REQUIRED OpenGLTester)
endif()

# Dynamic linking is meaningless on Emscripten and too inconvenient on Android
if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(BUILD_STATIC ON)
endif()

if(NOT BUILD_STATIC)
    set(SHARED_OR_STATIC SHARED)
else()
    set(SHARED_OR_STATIC STATIC)
endif()

set(MAGNUMINTEGRATION_CMAKE_MODULE_INSTALL_DIR share/cmake/MagnumVR)

# Library version
set(MAGNUM_LIBRARY_VERSION 1.0)
set(MAGNUM_LIBRARY_SOVERSION 1)

add_subdirectory(modules)
add_subdirectory(src)

# Build snippets as part of testing
if(BUILD_TESTS)
    add_subdirectory(doc/snippets)
endif()
