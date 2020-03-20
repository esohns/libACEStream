# cotire.cmake 1.8.0 generated file
# /mnt/win_d/projects/ACEStream/clang/src/modules/vis/ACEStream_Visualization_CXX_cotire.cmake
set (COTIRE_ADDITIONAL_PREFIX_HEADER_IGNORE_EXTENSIONS "inc;inl;ipp")
set (COTIRE_ADDITIONAL_PREFIX_HEADER_IGNORE_PATH "")
set (COTIRE_CLEAN_ALL_TARGET_NAME "clean_cotire")
set (COTIRE_CLEAN_TARGET_SUFFIX "_clean_cotire")
set (COTIRE_CMAKE_MODULE_FILE "/media/erik/USB_BLACK/lib/cotire/CMake/cotire.cmake")
set (COTIRE_CMAKE_MODULE_VERSION "1.8.0")
set (COTIRE_DEBUG "OFF")
set (COTIRE_INTDIR "cotire")
set (COTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES "-j")
set (COTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES "2")
set (COTIRE_PCH_ALL_TARGET_NAME "all_pch")
set (COTIRE_PCH_TARGET_SUFFIX "_pch")
set (COTIRE_PREFIX_HEADER_FILENAME_SUFFIX "_prefix")
set (COTIRE_TARGETS_FOLDER "cotire")
set (COTIRE_TARGET_COMPILE_DEFINITIONS_DEBUG "_DEBUG;DEBUG_DEBUGGER;ACESTREAM_NTRACE=1;ACE_HAS_DLL;COMMON_HAS_DLL;HAVE_CONFIG_H;_GLIBCXX_USE_CXX11_ABI=0;FFMPEG_SUPPORT;OPENGL_SUPPORT;GTK2_SUPPORT;GTK_SUPPORT;GTKGLAREA_SUPPORT;GTKGL_SUPPORT;wxUSE_LOG_DEBUG;CURSES_SUPPORT;GTK_USE;GTK2_USE")
set (COTIRE_TARGET_COMPILE_FLAGS_DEBUG "-g;-fPIC;-std=c++11")
set (COTIRE_TARGET_CONFIGURATION_TYPES "Debug")
set (COTIRE_TARGET_CXX_COMPILER_LAUNCHER "COTIRE_TARGET_CXX_COMPILER_LAUNCHER-NOTFOUND")
set (COTIRE_TARGET_IGNORE_PATH "/mnt/win_d/projects/ACEStream")
set (COTIRE_TARGET_INCLUDE_DIRECTORIES_DEBUG "$<BUILD_INTERFACE:/usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/cairo;/usr/include/pixman-1;/usr/include/gdk-pixbuf-2.0;/usr/include/libmount;/usr/include/blkid;/usr/include/harfbuzz;/usr/include/fribidi;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/usr/include/uuid;/usr/include/freetype2;/usr/include/libpng16>;$<BUILD_INTERFACE:/media/erik/USB_BLACK/lib/gtkglarea>;$<BUILD_INTERFACE:>;$<BUILD_INTERFACE:/usr/include/x86_64-linux-gnu>;$<BUILD_INTERFACE:/media/erik/USB_BLACK/lib/ACE_TAO/ACE>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/gl>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/math>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../../../../Common/src/ui/gtk>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/modules/vis/../../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../dec>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/vis/../stat>;/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/Common/src;/mnt/win_d/projects/Common/src/error;/mnt/win_d/projects/Common/src/signal;/mnt/win_d/projects/Common/src/timer;/mnt/win_d/projects/Common/cmake/src/..;/usr/include/x86_64-linux-gnu//ImageMagick-6;/usr/include/ImageMagick-6;/mnt/win_d/projects/Common/src/gl/..;/mnt/win_d/projects/Common/src/gl/../image;/mnt/win_d/projects/Common/cmake/src/gl/../..;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../3rd_party/tree>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../3rd_party/bison>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/modules/misc/../../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../dec>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../file>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../lib>")
set (COTIRE_TARGET_INCLUDE_PRIORITY_PATH "")
set (COTIRE_TARGET_LANGUAGE "CXX")
set (COTIRE_TARGET_MAXIMUM_NUMBER_OF_INCLUDES "-j")
set (COTIRE_TARGET_POST_UNDEFS "")
set (COTIRE_TARGET_PRE_UNDEFS "")
set (COTIRE_TARGET_SOURCES "stdafx.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_tools.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_gtk_cairo.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_gtk_cairo_spectrum_analyzer.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_gtk_pixbuf.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_gtk_window.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_libav_resize.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_opengl_glut.cpp;/mnt/win_d/projects/ACEStream/src/modules/vis/stream_vis_x11_window.cpp")
set (COTIRE_UNITY_BUILD_ALL_TARGET_NAME "all_unity")
set (COTIRE_UNITY_BUILD_TARGET_SUFFIX "_unity")
set (COTIRE_UNITY_OUTPUT_DIRECTORY "unity")
set (COTIRE_UNITY_SOURCE_EXCLUDE_EXTENSIONS "m;mm")
set (COTIRE_UNITY_SOURCE_FILENAME_SUFFIX "_unity")
set (CMAKE_GENERATOR "Ninja")
set (CMAKE_BUILD_TYPE "Debug")
set (CMAKE_CXX_COMPILER_ID "Clang")
set (CMAKE_CXX_COMPILER_VERSION "9.0.0")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")
set (CMAKE_CXX_COMPILER_ARG1 "")
set (CMAKE_INCLUDE_FLAG_CXX "-I")
set (CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
set (CMAKE_CXX_SOURCE_FILE_EXTENSIONS "C;M;c++;cc;cpp;cxx;mm;CPP")
