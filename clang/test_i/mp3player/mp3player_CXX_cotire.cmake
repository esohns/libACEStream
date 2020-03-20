# cotire.cmake 1.8.0 generated file
# /mnt/win_d/projects/ACEStream/clang/test_i/mp3player/mp3player_CXX_cotire.cmake
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
set (COTIRE_TARGET_COMPILE_DEFINITIONS_DEBUG "_DEBUG;DEBUG_DEBUGGER;ACESTREAM_NTRACE=1;HAVE_CONFIG_H;ACE_HAS_DLL;GUI_SUPPORT;OPENGL_SUPPORT;GTK2_SUPPORT;GTK_SUPPORT;GTKGLAREA_SUPPORT;GTKGL_SUPPORT;wxUSE_LOG_DEBUG;CURSES_SUPPORT;GTK_USE;GTK2_USE;COMMON_HAS_DLL;COMMON_UI_HAS_DLL;COMMON_UI_GTK_HAS_DLL;COMMON_GL_HAS_DLL;STREAM_DB_HAS_DLL;_GLIBCXX_INCLUDE_NEXT_C_HEADERS;_GLIBCXX_USE_CXX11_ABI=0")
set (COTIRE_TARGET_COMPILE_FLAGS_DEBUG "-g;-std=c++11")
set (COTIRE_TARGET_CONFIGURATION_TYPES "Debug")
set (COTIRE_TARGET_CXX_COMPILER_LAUNCHER "COTIRE_TARGET_CXX_COMPILER_LAUNCHER-NOTFOUND")
set (COTIRE_TARGET_IGNORE_PATH "/mnt/win_d/projects/ACEStream")
set (COTIRE_TARGET_INCLUDE_DIRECTORIES_DEBUG "/usr/include/libxml2;/usr/include/mysql;/usr/include/x86_64-linux-gnu;/usr/local/src/ACE_wrappers;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../Common/src;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../Common/src/log;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../Common/src/signal;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../Common/src/timer;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../ACENetwork/src;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../ACENetwork/src/protocol/http;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../ACENetwork/src/client_server;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../ACENetwork/include;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../../ACENetwork/cmake;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../3rd_party/bison;/mnt/win_d/projects/ACEStream/clang/test_i/mp3player/../..;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src/modules/dec;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src/modules/dev;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src/modules/file;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src/modules/misc;/mnt/win_d/projects/ACEStream/test_i/mp3player/../../src/modules/stat;/mnt/win_d/projects/ACEStream/test_i/mp3player/..;/media/erik/USB_BLACK/lib/ACE_TAO/ACE;/mnt/win_d/projects/Common/src;/mnt/win_d/projects/Common/src/error;/mnt/win_d/projects/Common/src/signal;/mnt/win_d/projects/Common/src/timer;/mnt/win_d/projects/Common/cmake/src/..;/mnt/win_d/projects/Common/src/log/..;/mnt/win_d/projects/Common/cmake/src/log/../..;/mnt/win_d/projects/Common/cmake/src/signal/../..;/mnt/win_d/projects/Common/src/signal/..;/mnt/win_d/projects/Common/src/timer/..;/mnt/win_d/projects/Common/cmake/src/timer/../..;$<BUILD_INTERFACE:/usr/include/x86_64-linux-gnu>;$<BUILD_INTERFACE:/media/erik/USB_BLACK/lib/ACE_TAO/ACE>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/../3rd_party/tree>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat>;$<BUILD_INTERFACE:>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../../../3rd_party/bison>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dec/../vis>;$<BUILD_INTERFACE:/usr/include/gtk-2.0;/usr/lib/x86_64-linux-gnu/gtk-2.0/include;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/cairo;/usr/include/pixman-1;/usr/include/gdk-pixbuf-2.0;/usr/include/libmount;/usr/include/blkid;/usr/include/harfbuzz;/usr/include/fribidi;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include;/usr/include/uuid;/usr/include/freetype2;/usr/include/libpng16>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../vis>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/dev/../dec>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/cmake>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/file/../lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../dec>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/lib/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/math>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/modules/stat/../../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/stat/../lib>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/error>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/image>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/log>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/timer>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../../Common/src/ui>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../../../3rd_party/bison>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/clang/src/modules/misc/../../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../..>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../dec>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../dev>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../file>;$<BUILD_INTERFACE:/mnt/win_d/projects/ACEStream/src/modules/misc/../lib>")
set (COTIRE_TARGET_INCLUDE_PRIORITY_PATH "")
set (COTIRE_TARGET_LANGUAGE "CXX")
set (COTIRE_TARGET_MAXIMUM_NUMBER_OF_INCLUDES "-j")
set (COTIRE_TARGET_POST_UNDEFS "")
set (COTIRE_TARGET_PRE_UNDEFS "")
set (COTIRE_TARGET_SOURCES "stdafx.cpp;test_i_message.cpp;test_i_session_message.cpp;test_i.cpp;test_i_stream.cpp")
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
