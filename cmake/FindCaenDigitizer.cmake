set(CAEN_LIBRARY_DIR 
    "${CMAKE_SOURCE_DIR}/dt5742-libraries"
    CACHE PATH
    "Location of CAEN proprietary libraries (unpacked from tarball, not installed)"
)


add_library(CAENComm SHARED IMPORTED)
set_target_properties(CAENComm PROPERTIES
    INTERFACE_COMPILE_FEATURES "cxx_std_14"
    INTERFACE_INCLUDE_DIRECTORIES "${CAEN_LIBRARY_DIR}/CAENComm-1.5.0/include"
    IMPORTED_LOCATION "${CAEN_LIBRARY_DIR}/CAENComm-1.5.0/lib/x64/libCAENComm.so.1.5.0"
)
install(
    FILES "${CAEN_LIBRARY_DIR}/CAENComm-1.5.0/lib/x64/libCAENComm.so.1.5.0"
    DESTINATION "${CalvisionDaq_INSTALL_LIB_DIR}"
)

add_library(CAENDigitizer SHARED IMPORTED)
set_target_properties(CAENDigitizer PROPERTIES
    INTERFACE_COMPILE_FEATURES "cxx_std_14"
    INTERFACE_INCLUDE_DIRECTORIES "${CAEN_LIBRARY_DIR}/CAENDigitizer-2.17.0/include"
    IMPORTED_LOCATION "${CAEN_LIBRARY_DIR}/CAENDigitizer-2.17.0/lib/x86_64/libCAENDigitizer.so.2.17.0"
    INTERFACE_LINK_LIBRARIES CAENComm
)
install(
    FILES "${CAEN_LIBRARY_DIR}/CAENDigitizer-2.17.0/lib/x86_64/libCAENDigitizer.so.2.17.0"
    DESTINATION "${CalvisionDaq_INSTALL_LIB_DIR}"
)

add_library(CAENVME SHARED IMPORTED)
set_target_properties(CAENVME PROPERTIES
    INTERFACE_COMPILE_FEATURES "cxx_std_14"
    INTERFACE_INCLUDE_DIRECTORIES "${CAEN_LIBRARY_DIR}/CAENVMELib-3.3.0/include"
    IMPORTED_LOCATION "${CAEN_LIBRARY_DIR}/CAENVMELib-3.3.0/lib/x64/libCAENVME.so.3.3.0"
)
install(
    FILES "${CAEN_LIBRARY_DIR}/CAENVMELib-3.3.0/lib/x64/libCAENVME.so.3.3.0"
    DESTINATION "${CalvisionDaq_INSTALL_LIB_DIR}"
)


