option(CAEN_LIBRARY_DIR "Location of CAEN proprietary libraries (unpacked from tarball, not installed)" "${CMAKE_SOURCE_DIR}/dt5742-libraries")



function(import_Caen_library LIBRARY_NAME LIB_VERISON ARCH)
    add_library(${LIBRARY_NAME} SHARED IMPORTED)

    set(LIBRARY_PATH "${CAEN_LIBRARY_DIR}/${LIBRARY_NAME}-${LIB_VERSION}")
    set(LIBRARY_SONAME "lib${LIBRARY_NAME}.so.${LIB_VERSION}")

    set_target_properties(${LIBRARY_NAME} PROPERTIES
        INTERFACE_COMPILE_FEATURES "cxx_std_14"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBRARY_PATH}/include"
    )

    set_property(TARGET ${LIBRARY_NAME} APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
    set_target_properties(${LIBRARY_NAME} PROPERTIES
        IMPORTED_LOCATION_NOCONFIG "${LIBRARY_PATH}/lib/${ARCH}/${LIBRARY_SONAME}"
        IMPORTED_SONAME_NOCONFIG "${LIBRARY_SONAME}"
    )
endfunction()




import_Caen_library(CAENComm "1.5.0" "x86")

import_Caen_library(CAENDigitizer "2.17.0" "x86_64")
set_target_properties(CAENDigitizer PROPERTIES INTERFACE_LINK_LIBRARIES "CAENComm")

import_Caen_library(CAENVMELib "3.3.0" "x86")
