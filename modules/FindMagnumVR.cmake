#.rst:
# Find Magnum VR library
# -------------------------------
#
# Finds the Magnum VR library. Basic usage::
#
#  find_package(MagnumVR REQUIRED)
#
# This command tries to find Magnum integration library and then defines the
# following:
#
#  MagnumVR_FOUND      - Whether the library was found
#
# This command alone is useless without specifying the components:
#
#  Openvr                       - OpenVR SDK integration library
#
# Example usage with specifying additional components is:
#
#  find_package(MagnumVR REQUIRED Openvr)
#
# For each component is then defined:
#
#  MagnumVR_*_FOUND    - Whether the component was found
#  MagnumVR::*         - Component imported target
#
# The package is found if either debug or release version of each requested
# library is found. If both debug and release libraries are found, proper
# version is chosen based on actual build configuration of the project (i.e.
# Debug build is linked to debug libraries, Release build to release
# libraries).
#
# Additionally these variables are defined for internal usage:
#
#  MAGNUMVR_INCLUDE_DIR - Magnum integration include dir (w/o
#   dependencies)
#  MAGNUMVR_*_LIBRARY_DEBUG - Debug version of given library, if found
#  MAGNUMVR_*_LIBRARY_RELEASE - Release version of given library, if
#   found
#

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018 Konstantinos Chatzilygeroudis <costashatz@gmail.com>
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

# Magnum library dependencies
set(_MAGNUMVR_DEPENDENCIES )
foreach(_component ${MagnumVR_FIND_COMPONENTS})
    if(_component STREQUAL Bullet)
        set(_MAGNUMVR_${_component}_MAGNUM_DEPENDENCIES SceneGraph Shaders GL)
    elseif(_component STREQUAL Dart)
        set(_MAGNUMVR_${_component}_MAGNUM_DEPENDENCIES SceneGraph Primitives MeshTools GL)
    elseif(_component STREQUAL ImGui)
        set(_MAGNUMVR_${_component}_MAGNUM_DEPENDENCIES GL)
    endif()

    list(APPEND _MAGNUMVR_DEPENDENCIES ${_MAGNUMVR_${_component}_MAGNUM_DEPENDENCIES})
    list(APPEND _MAGNUMVR_OPTIONAL_DEPENDENCIES ${_MAGNUMVR_${_component}_MAGNUM_OPTIONAL_DEPENDENCIES})
endforeach()
find_package(Magnum REQUIRED ${_MAGNUMVR_DEPENDENCIES})
if(_MAGNUMVR_OPTIONAL_DEPENDENCIES)
    find_package(Magnum OPTIONAL_COMPONENTS ${_MAGNUMVR_OPTIONAL_DEPENDENCIES})
endif()

# Global integration include dir
find_path(MAGNUMVR_INCLUDE_DIR Magnum
    HINTS ${MAGNUM_INCLUDE_DIR})
mark_as_advanced(MAGNUMVR_INCLUDE_DIR)

# Component distinction (listing them explicitly to avoid mistakes with finding
# components from other repositories)
set(_MAGNUMVR_LIBRARY_COMPONENT_LIST Openvr)
set(_MAGNUMVR_HEADER_ONLY_COMPONENT_LIST )

# Inter-component dependencies (none yet)
# set(_MAGNUMVR_Component_DEPENDENCIES Dependency)

# Ensure that all inter-component dependencies are specified as well
set(_MAGNUMVR_ADDITIONAL_COMPONENTS )
foreach(_component ${MagnumVR_FIND_COMPONENTS})
    # Mark the dependencies as required if the component is also required
    if(MagnumVR_FIND_REQUIRED_${_component})
        foreach(_dependency ${_MAGNUMVR_${_component}_DEPENDENCIES})
            set(MagnumVR_FIND_REQUIRED_${_dependency} TRUE)
        endforeach()
    endif()

    list(APPEND _MAGNUMVR_ADDITIONAL_COMPONENTS ${_MAGNUMVR_${_component}_DEPENDENCIES})
endforeach()

# Join the lists, remove duplicate components
if(_MAGNUMVR_ADDITIONAL_COMPONENTS)
    list(INSERT MagnumVR_FIND_COMPONENTS 0 ${_MAGNUMVR_ADDITIONAL_COMPONENTS})
endif()
if(MagnumVR_FIND_COMPONENTS)
    list(REMOVE_DUPLICATES MagnumVR_FIND_COMPONENTS)
endif()

# Convert components lists to regular expressions so I can use if(MATCHES).
# TODO: Drop this once CMake 3.3 and if(IN_LIST) can be used
foreach(_WHAT LIBRARY HEADER_ONLY)
    string(REPLACE ";" "|" _MAGNUMVR_${_WHAT}_COMPONENTS "${_MAGNUMVR_${_WHAT}_COMPONENT_LIST}")
    set(_MAGNUMVR_${_WHAT}_COMPONENTS "^(${_MAGNUMVR_${_WHAT}_COMPONENTS})$")
endforeach()

# Find all components
foreach(_component ${MagnumVR_FIND_COMPONENTS})
    string(TOUPPER ${_component} _COMPONENT)

    # Create imported target in case the library is found. If the project is
    # added as subproject to CMake, the target already exists and all the
    # required setup is already done from the build tree.
    if(TARGET MagnumVR::${_component})
        set(MagnumVR_${_component}_FOUND TRUE)
    else()
        # Library components
        if(_component MATCHES ${_MAGNUMVR_LIBRARY_COMPONENTS} AND NOT _component MATCHES ${_MAGNUMVR_HEADER_ONLY_COMPONENTS})
            add_library(MagnumVR::${_component} UNKNOWN IMPORTED)

            # Try to find both debug and release version
            find_library(MAGNUMVR_${_COMPONENT}_LIBRARY_DEBUG Magnum${_component}VR-d)
            find_library(MAGNUMVR_${_COMPONENT}_LIBRARY_RELEASE Magnum${_component}VR)
            mark_as_advanced(MAGNUMVR_${_COMPONENT}_LIBRARY_DEBUG
                MAGNUMVR_${_COMPONENT}_LIBRARY_RELEASE)

            if(MAGNUMVR_${_COMPONENT}_LIBRARY_RELEASE)
                set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS RELEASE)
                set_property(TARGET MagnumVR::${_component} PROPERTY
                    IMPORTED_LOCATION_RELEASE ${MAGNUMVR_${_COMPONENT}_LIBRARY_RELEASE})
            endif()

            if(MAGNUMVR_${_COMPONENT}_LIBRARY_DEBUG)
                set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                    IMPORTED_CONFIGURATIONS DEBUG)
                set_property(TARGET MagnumVR::${_component} PROPERTY
                    IMPORTED_LOCATION_DEBUG ${MAGNUMVR_${_COMPONENT}_LIBRARY_DEBUG})
            endif()
        endif()

        # Header-only library components
        if(_component MATCHES ${_MAGNUMVR_HEADER_ONLY_COMPONENTS})
            add_library(MagnumVR::${_component} INTERFACE IMPORTED)
        endif()
        if(_component STREQUAL Ovr)
            find_package(OVR)
            set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Openvr::OVR)

            set(_MAGNUMVR_${_COMPONENT}_INCLUDE_PATH_NAMES OvrVR.h)
        endif()

        # Find library includes
        if(_component MATCHES ${_MAGNUMVR_LIBRARY_COMPONENTS})
            find_path(_MAGNUMVR_${_COMPONENT}_INCLUDE_DIR
                NAMES ${_MAGNUMVR_${_COMPONENT}_INCLUDE_PATH_NAMES}
                HINTS ${MAGNUMVR_INCLUDE_DIR}/Magnum/${_component}VR)
            mark_as_advanced(_MAGNUMVR_${_COMPONENT}_INCLUDE_DIR)
        endif()

        if(_component MATCHES ${_MAGNUMVR_LIBRARY_COMPONENTS})
            # Link to core Magnum library, add other Magnum required and
            # optional dependencies
            set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES Magnum::Magnum)
            foreach(_dependency ${_MAGNUMVR_${_component}_MAGNUM_DEPENDENCIES})
                set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES Magnum::${_dependency})
            endforeach()
            foreach(_dependency ${_MAGNUMVR_${_component}_MAGNUM_OPTIONAL_DEPENDENCIES})
                if(Magnum_${_dependency}_FOUND)
                    set_property(TARGET MagnumVR::${_component} APPEND     PROPERTY
                        INTERFACE_LINK_LIBRARIES Magnum::${_dependency})
                endif()
            endforeach()

            # Add inter-project dependencies
            foreach(_dependency ${_MAGNUMVR_${_component}_DEPENDENCIES})
                set_property(TARGET MagnumVR::${_component} APPEND PROPERTY
                    INTERFACE_LINK_LIBRARIES MagnumVR::${_dependency})
            endforeach()
        endif()

        # Decide if the library was found
        if(_component MATCHES ${_MAGNUMVR_LIBRARY_COMPONENTS} AND _MAGNUMVR_${_COMPONENT}_INCLUDE_DIR AND (_component MATCHES ${_MAGNUMVR_HEADER_ONLY_COMPONENTS} OR MAGNUMVR_${_COMPONENT}_LIBRARY_DEBUG OR MAGNUMVR_${_COMPONENT}_LIBRARY_RELEASE))
            set(MagnumVR_${_component}_FOUND TRUE)
        else()
            set(MagnumVR_${_component}_FOUND FALSE)
        endif()
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MagnumVR
    REQUIRED_VARS MAGNUMVR_INCLUDE_DIR
    HANDLE_COMPONENTS)
