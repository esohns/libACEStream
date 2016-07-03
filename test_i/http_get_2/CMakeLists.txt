cmake_minimum_required (VERSION 2.8)

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 find_package (Threads REQUIRED)
endif ()

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace build/linux/ace
                DOC "searching for ${ACE_LIB_FILE}")
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()

set (ACE_SSL_LIB_FILE libACE_SSL.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_SSL_LIBRARY ${ACE_SSL_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace/lib build/linux/ace/lib
                DOC "searching for ${ACE_SSL_LIB_FILE}")
 else ()
  find_library (ACE_SSL_LIBRARY ${ACE_SSL_LIB_FILE}
                DOC "searching for ${ACE_SSL_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_SSL_LIB_FILE)
 set (ACE_SSL_LIB_FILE ACE_SSL${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_SSL_LIBRARY ${ACE_SSL_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_SSL_LIB_FILE}"
               NO_DEFAULT_PATH)
endif ()
#if (NOT EXISTS ACE_SSL_LIBRARY)
if (NOT ACE_SSL_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_SSL_LIB_FILE} (was: \"${ACE_SSL_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE SSL library \"${ACE_SSL_LIBRARY}\"")
endif ()

include (FindPkgConfig)
if (UNIX)
 pkg_check_modules (PKG_Z REQUIRED zlib)
# *NOTE*: libCommon requires libcap
 pkg_check_modules (PKG_CAP REQUIRED libcap)
 pkg_check_modules (PKG_SSL REQUIRED libssl)
#pkg_check_modules (PKG_ACE REQUIRED ACE)
endif ()
pkg_check_modules (PKG_XML2 REQUIRED libxml-2.0)

#if (WIN32)
# *TODO*: repair pkgconfig support
# find_library (LIBXML2_LIBRARY libxml2.dll.a
#               PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../../libxml2-2.9.1
#               PATH_SUFFIXES .libs
#               DOC "searching for libxml2.dll.a"
#               NO_DEFAULT_PATH)
# if (NOT LIBXML2_LIBRARY)
#  message (FATAL_ERROR "could not find libxml2 (was: \"libxml2.dll.a\"), aborting")
# endif ()
#endif ()

if (UNIX)
 include_directories (/usr/lib/libreoffice/sdk/include)

 add_definitions (-DSAL_UNX)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib-1.2.8)

# *NOTE*: workaround
 set(_PF86 "ProgramFiles(x86)")
 set (LIBREOFFICE_SDK_DIRECTORY
#      "$ENV{PROGRAMFILES}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
#      "$ENV{${_PF86}}/LibreOffice_5.1_SDK/sdk"
       "C:/LibreOffice_5.1_SDK_installer/sdk")
# message (STATUS "LibreOffice SDK directory: \"${LIBREOFFICE_SDK_DIRECTORY}\"")
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES libCommon/cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()

find_package (ACEStream ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
endif ()
add_definitions (-DSTREAM_DB_HAS_DLL)

#string (TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
find_package (ACENetwork ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES libACENetwork/cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)

# include_directories (${PKG_CAP_INCLUDE_DIRS})
 include_directories (${PKG_SSL_INCLUDE_DIRS})
 include_directories (${PKG_XML2_INCLUDE_DIRS})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/modules/libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/modules/libACENetwork/src/protocol/http)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/modules/libACENetwork/src/client_server)
else ()
 include_directories ($ENV{ACE_ROOT})

# include_directories (${PKG_LIBXML2_INCLUDE_DIRS})
# *TODO*: repair pkgconfig support
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libiconv-1.9.2/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libxml2-2.9.4/win32/include/libxml2)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../openssl/include)

 include_directories (${LIBREOFFICE_SDK_DIRECTORY}/include)
# include_directories (${LIBREOFFICE_SDK_DIRECTORY}/cli)

# *TODO*: repair pkgconfig support
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/include/glib-2.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/include/gtk-2.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade/include/libglade-2.0)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/src/protocol/http)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/src/client_server)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/bison)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/dec)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/doc)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/file)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/html)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/misc)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/net)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src/modules/net/protocols/http)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)

############## next target ################
set (http_get_2_EXE_SRCS
     test_i_common.h
     test_i_common_modules.h
     test_i_connection_common.h
     test_i_connection_manager_common.h
     test_i_defines.h
     test_i_message.cpp
     test_i_message.h
     test_i_module_spreadsheetwriter.cpp
     test_i_module_spreadsheetwriter.h
     test_i_module_htmlparser.cpp
     test_i_module_htmlparser.h
     test_i_module_httpget.cpp
     test_i_module_httpget.h
     test_i_session_message.cpp
     test_i_session_message.h
     test_i_http_get.cpp
     test_i_http_get_common.h
     test_i_http_get_signalhandler.cpp
     test_i_http_get_signalhandler.h
     test_i_http_get_stream.cpp
     test_i_http_get_stream.h
     test_i_http_get_stream.inl
#  test_i_template_instantiations.cpp
     stdafx.cpp
     stdafx.h)

set (EXECUTABLE http_get_2)
add_executable (${EXECUTABLE} ${http_get_2_EXE_SRCS})

target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE"
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif ()
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})
target_link_libraries (${EXECUTABLE} ${ACE_SSL_LIBRARY})
if (UNIX)
 target_link_libraries (${EXECUTABLE} ${PKG_Z_LIBRARIES})
# *NOTE*: libCommon requires libcap
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 target_link_libraries (${EXECUTABLE} ${PKG_SSL_LIBRARIES})
 target_link_libraries (${EXECUTABLE} ${PKG_XML2_LIBRARIES})

 target_link_libraries (${EXECUTABLE} /usr/lib/libreoffice/sdk/lib/libuno_sal.so)

 target_link_libraries (${EXECUTABLE} /usr/lib/ure/lib/libgcc3_uno.so)
 target_link_libraries (${EXECUTABLE} /usr/lib/libreoffice/sdk/lib/libuno_cppu.so)

 target_link_libraries (${EXECUTABLE} /usr/lib/ure/lib/libstorelo.so)
 target_link_libraries (${EXECUTABLE} /usr/lib/ure/lib/libreglo.so)

 target_link_libraries (${EXECUTABLE} /usr/lib/ure/lib/libxmlreaderlo.so)
 target_link_libraries (${EXECUTABLE} /usr/lib/ure/lib/libunoidllo.so)
 target_link_libraries (${EXECUTABLE} /usr/lib/libreoffice/sdk/lib/libuno_cppuhelpergcc3.so)

 #target_link_libraries (${EXECUTABLE} /usr/lib/libreoffice/sdk/lib/libuno_salhelpergcc3.so)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib-1.2.8/libz.dll.a)

# *TODO*: repair pkgconfig support
 #target_link_libraries (http_get ${LIBXML2_LIBRARY})
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../libxml2-2.9.4/win32/lib/libxml2.lib)

 target_link_libraries (${EXECUTABLE} ${LIBREOFFICE_SDK_DIRECTORY}/lib/isal.lib)

 target_link_libraries (${EXECUTABLE} ${LIBREOFFICE_SDK_DIRECTORY}/lib/icppu.lib)
 target_link_libraries (${EXECUTABLE} ${LIBREOFFICE_SDK_DIRECTORY}/lib/icppuhelper.lib)
endif ()
#target_link_libraries (${EXECUTABLE} ${MYSQL_LIBRARY})
target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} ACEStream)
target_link_libraries (${EXECUTABLE} ACEStream_Decoder)
target_link_libraries (${EXECUTABLE} ACEStream_Document)
target_link_libraries (${EXECUTABLE} ACEStream_HTML)
target_link_libraries (${EXECUTABLE} ACENetwork)
target_link_libraries (${EXECUTABLE} ACENetwork_Protocol_HTTP)
#target_link_libraries (${EXECUTABLE} ACENetwork_Client)

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_i)