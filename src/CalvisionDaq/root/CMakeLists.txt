set(LIBRARY_NAME "${PROJECT_NAME}Root")

make_CalvisionDaq_library(${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME} PUBLIC
    CalvisionDaqCommon
    CalvisionDaqDigitizer
    CppUtils::CppUtilsIO
    ROOT::Core
    ROOT::Tree
    ROOT::RIO
    ROOT::Matrix
)

install_CalvisionDaq_library(${LIBRARY_NAME} root)
