set(LIBRARY_NAME "${PROJECT_NAME}Root")

find_package(ROOT REQUIRED COMPONENTS
    RIO
)

make_CalvisionDaq_library(${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME} PUBLIC
    CalvisionDaqCommon
    CalvisionDaqDigitizer
    ROOT::RIO
)

install_CalvisionDaq_library(${LIBRARY_NAME} root)
