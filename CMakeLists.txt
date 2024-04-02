cmake_minimum_required(VERSION 3.28)
project(QtApp.Common VERSION 1.0.0 LANGUAGES CXX)

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
  include(CTest)
endif()

add_subdirectory(qtapp/common)

