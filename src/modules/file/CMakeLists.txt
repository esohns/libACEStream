cmake_minimum_required (VERSION 2.8)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../)

############## next target ################
set (${PROJECT_NAME}_File_LIB_SRCS
     stream_file_defines.h
     stream_file_exports.h
     stream_file_source.cpp
     stream_file_source.h
     stream_file_source.inl
     stream_file_sink.cpp
     stream_file_sink.h
     stream_file_sink.inl
     stdafx.cpp
     stdafx.h)

add_library (${PROJECT_NAME}_File SHARED ${ACEStream_File_LIB_SRCS})

add_dependencies (${PROJECT_NAME}_File Common)
add_dependencies (${PROJECT_NAME}_File ${PROJECT_NAME})

if (WIN32)
 target_link_libraries (${PROJECT_NAME}_File ${ACE_LIBRARY})
endif ()

set_target_properties (${PROJECT_NAME}_File PROPERTIES DEFINE_SYMBOL STREAM_FILE_BUILD_DLL)
set_target_properties (${PROJECT_NAME}_File PROPERTIES VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} SOVERSION 1)

# add all targets to the build-tree export set
export (TARGETS ${PROJECT_NAME}_File
        APPEND FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

############### install files ################
install (FILES
         stream_file_defines.h
         stream_file_exports.h
         stream_file_source.h
         stream_file_source.inl
         stream_file_sink.h
         stream_file_sink.inl
         DESTINATION include)
install (TARGETS ${PROJECT_NAME}_File
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)