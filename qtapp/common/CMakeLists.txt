# Project settings
## Language standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX d)
## Output directories and other MSVC settings
if(PROJECT_IS_TOP_LEVEL)
  if(MSVC)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Configs" FORCE)
    foreach(conf_name ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER ${conf_name} upper_conf_name)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${upper_conf_name} ${CMAKE_BINARY_DIR}/${conf_name}/bin)
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${upper_conf_name} ${CMAKE_BINARY_DIR}/${conf_name}/lib)
    endforeach()

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  endif()
endif()

## Default visibility of the symbols in this module
include(GenerateExportHeader)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

# Dependencies
## Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)
## spdlog
find_package(spdlog REQUIRED)

# Create the library
set(LIB_NAME QtApp.Common)
add_library(${LIB_NAME} SHARED)

target_include_directories(${LIB_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

set(${LIB_NAME}_PUBLIC_HEADER
  logging.h
)

target_sources(${LIB_NAME}
  PUBLIC
    FILE_SET lib_hdrs 
      TYPE HEADERS 
      FILES "${${LIB_NAME}_PUBLIC_HEADER}"
  PRIVATE
    logging.cc
)

set_target_properties(${LIB_NAME} PROPERTIES
  EXPORT_NAME Common
)

target_link_libraries(${LIB_NAME}
  PUBLIC
    Qt6::Widgets
  PRIVATE
    spdlog::spdlog
)

## Export header
generate_export_header(${LIB_NAME}
  BASE_NAME qtapp_common)

# Copy 7z.exe to binary directory which will be used to
# archive log files every month.
if(WIN32)
  add_custom_command(TARGET ${LIB_NAME}
    POST_BUILD
    COMMAND
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/7z/7z.exe
    $<TARGET_FILE_DIR:${LIB_NAME}>/7z.exe)
endif(WIN32)

# Unit tests
if(BUILD_TESTING)
  find_package(GTest REQUIRED)

  # logging
  add_executable(logging_test logging_test.cc)
  target_link_libraries(logging_test 
    PRIVATE 
    ${LIB_NAME}
    Qt6::Widgets
    GTest::gtest)
  if(MSVC)
    set_property(TARGET logging_test PROPERTY FOLDER "Test")
  endif(MSVC)
  add_test(NAME Logging COMMAND logging_test)
endif(BUILD_TESTING)

# Install
add_library(QtApp::Common ALIAS ${LIB_NAME})
include(GNUInstallDirs)
target_include_directories(${LIB_NAME}
  INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${LIB_NAME} EXPORT Common
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET lib_hdrs DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qtapp/common
)

install(EXPORT Common
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/qtapp
  NAMESPACE QtApp::
  FILE CommonConfig.cmake
)
configure_file(CommonTargets.cmake.in CommonTargets.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/CommonTargets.cmake"
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/qtapp
)

install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/7z/7z.exe"
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qtapp_common_export.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qtapp/common
)

install(FILES "${PROJECT_SOURCE_DIR}/qtapp/common.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qtapp
)

