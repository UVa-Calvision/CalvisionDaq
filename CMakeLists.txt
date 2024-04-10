cmake_minimum_required(VERSION 3.18)

project(CalvisionDaq VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -Wextra -Werror -Wno-reorder")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CalvisionDaq_CXX_STD "cxx_std_14")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


include("${CMAKE_PREFIX_PATH}/lib/cmake/CppUtils/CppUtilsPreProcTargets.cmake")
include("${CMAKE_PREFIX_PATH}/lib/cmake/CppUtils/CppUtilsCUtilsTargets.cmake")
include("${CMAKE_PREFIX_PATH}/lib/cmake/CppUtils/CppUtilsIOTargets.cmake")

find_package(BLAS REQUIRED)


set(CalvisionDaq_BUILD_INCLUDE_DIR "${CalvisionDaq_SOURCE_DIR}/src")
set(CalvisionDaq_INSTALL_BIN_DIR "bin")
set(CalvisionDaq_INSTALL_LIB_DIR "lib")
set(CalvisionDaq_INSTALL_INCLUDE_DIR "include/CalvisionDaq")

include(cmake/MakeCalvisionDaqLibrary.cmake)

find_package(ROOT REQUIRED COMPONENTS
    Core
    Tree
    RIO
)
include(${ROOT_USE_FILE})


add_subdirectory(src/CalvisionDaq)
