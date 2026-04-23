# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_tic_tac_toe_dlc_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED tic_tac_toe_dlc_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(tic_tac_toe_dlc_FOUND FALSE)
  elseif(NOT tic_tac_toe_dlc_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(tic_tac_toe_dlc_FOUND FALSE)
  endif()
  return()
endif()
set(_tic_tac_toe_dlc_CONFIG_INCLUDED TRUE)

# output package information
if(NOT tic_tac_toe_dlc_FIND_QUIETLY)
  message(STATUS "Found tic_tac_toe_dlc: 0.0.0 (${tic_tac_toe_dlc_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'tic_tac_toe_dlc' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT tic_tac_toe_dlc_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(tic_tac_toe_dlc_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${tic_tac_toe_dlc_DIR}/${_extra}")
endforeach()
