cmake_minimum_required(VERSION 3.14)

project(CalvisionDaq VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CalvisionDaq_CXX_STD "cxx_std_14")


include(GNUInstallDirs)
set(CalvisionDaq_BUILD_INCLUDE_DIR "${CalvisionDaq_SOURCE_DIR}/src")
set(CalvisionDaq_INSTALL_BIN_DIR "${CMAKE_INSTALL_BINDIR}")
set(CalvisionDaq_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/CalvisionDaq")
set(CalvisionDaq_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/CalvisionDaq")

include(cmake/MakeCalvisionDaqLibrary.cmake)

add_subdirectory(src/CalvisionDaq)
