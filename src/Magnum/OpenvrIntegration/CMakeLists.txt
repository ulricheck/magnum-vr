#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2015, 2016 Jonathan Hale <squareys@googlemail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

find_package(Magnum REQUIRED GL)
find_package(OpenVR REQUIRED)

if(BUILD_STATIC)
    set(MAGNUM_OPENVRINTEGRATION_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

set(MagnumOpenvrIntegration_SRCS
        VRSystem.cpp
        OpenvrHMD.cpp
#        Context.cpp
#        Enums.cpp
#        Session.cpp
        )

set(MagnumOpenvrIntegration_HEADERS
        VRSystem.h
        HmdBase.h
        OpenvrHMD.h
#        Context.h
#        Enums.h
        OpenvrIntegration.h
        Integration.h
#        Session.h

        visibility.h
        )

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumOpenvrIntegration_HEADERS Conversion.h)
endif()

# OvrIntegration library
add_library(MagnumOpenvrIntegration ${SHARED_OR_STATIC}
        ${MagnumOpenvrIntegration_SRCS}
        ${MagnumOpenvrIntegration_HEADERS})
target_include_directories(MagnumOpenvrIntegration PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src)
set_target_properties(MagnumOpenvrIntegration PROPERTIES
        DEBUG_POSTFIX "-d"
        FOLDER "Magnum/OpenvrIntegration")
if(NOT BUILD_STATIC)
    set_target_properties(MagnumOpenvrIntegration PROPERTIES VERSION ${MAGNUM_LIBRARY_VERSION} SOVERSION ${MAGNUM_LIBRARY_SOVERSION})
elseif(BUILD_STATIC_PIC)
    set_target_properties(MagnumOpenvrIntegration PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_link_libraries(MagnumOpenvrIntegration
        Magnum::GL
        Magnum::Magnum
        OpenVR::OpenVR)

install(TARGETS MagnumOpenvrIntegration
        RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumOpenvrIntegration_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/OpenvrIntegration)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/configure.h DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/OpenvrIntegration)

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# Magnum OVR integration target alias for superprojects
add_library(MagnumIntegration::Openvr ALIAS MagnumOpenvrIntegration)
