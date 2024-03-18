function(make_CalvisionDaq_library LIBRARY_NAME)
    file(GLOB HEADER_LIST CONFIGURE_DEPENDS "./*.h")
    file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "./*.cpp")

    add_library(${LIBRARY_NAME} SHARED ${HEADER_LIST})
    target_sources(${LIBRARY_NAME} PRIVATE ${SOURCE_LIST})

    target_include_directories(${LIBRARY_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CalvisionDaq_BUILD_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

function(install_CalvisionDaq_library LIBRARY_NAME FOLDER_NAME)
    set(TARGETS_NAME "${LIBRARY_NAME}Targets")

    file(GLOB HEADER_LIST CONFIGURE_DEPENDS "./*.h")

    install(
        TARGETS ${LIBRARY_NAME}
        EXPORT ${TARGETS_NAME}
        LIBRARY DESTINATION ${CalvisionDaq_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CalvisionDaq_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CalvisionDaq_INSTALL_BIN_DIR}
    )

    install(
        FILES ${HEADER_LIST}
        DESTINATION "${CalvisionDaq_INSTALL_INCLUDE_DIR}/${FOLDER_NAME}"
    )

    install(
        EXPORT ${TARGETS_NAME}
        FILE ${TARGETS_NAME}.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
endfunction()
