include("${CMAKE_SOURCE_DIR}/cmake/FindCaenDigitizer.cmake")

set(LIBRARY_NAME "${PROJECT_NAME}Digitizer")

make_CalvisionDaq_library(${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME} PUBLIC
    CalvisionDaqCommon
    CAENComm
    CAENDigitizer
    CAENVME
    usb-1.0
)

install_CalvisionDaq_library(${LIBRARY_NAME} digitizer)
