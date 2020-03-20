# Install script for directory: /mnt/win_d/projects/ACEStream/src/modules/dev

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xmod_includex" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/ACEStream/stream_dev_common.h;/usr/local/include/ACEStream/stream_dev_defines.h;/usr/local/include/ACEStream/stream_dev_tools.h;/usr/local/include/ACEStream/stream_dev_tools.inl;/usr/local/include/ACEStream/stream_dev_cam_source_v4l.h;/usr/local/include/ACEStream/stream_dev_cam_source_v4l.inl;/usr/local/include/ACEStream/stream_dev_mic_source_alsa.h;/usr/local/include/ACEStream/stream_dev_mic_source_alsa.inl;/usr/local/include/ACEStream/stream_dev_target_alsa.h;/usr/local/include/ACEStream/stream_dev_target_alsa.inl")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include/ACEStream" TYPE FILE FILES
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_common.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_defines.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_tools.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_tools.inl"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_cam_source_v4l.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_cam_source_v4l.inl"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_mic_source_alsa.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_mic_source_alsa.inl"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_target_alsa.h"
    "/mnt/win_d/projects/ACEStream/src/modules/dev/stream_dev_target_alsa.inl"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xmod_libx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libACEStream_Device.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/mnt/win_d/projects/ACEStream/clang/src/modules/dev/libACEStream_Device.a")
endif()

