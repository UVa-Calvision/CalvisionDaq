set(LIBRARY_NAME "${PROJECT_NAME}Common")

make_CalvisionDaq_library(${LIBRARY_NAME})

target_link_libraries(${LIBRARY_NAME} m pthread CppUtils::CppUtilsCUtils)

install_CalvisionDaq_library(${LIBRARY_NAME} common)
