function(make_CalvisionDaq_exec EXEC_NAME LINK_LIBS)
    add_executable(${EXEC_NAME} "${EXEC_NAME}.cpp")

    foreach(LINK_LIB ${LINK_LIBS})
        target_link_libraries(${EXEC_NAME} PRIVATE ${LINK_LIB})
    endforeach()
endfunction()


make_CalvisionDaq_exec(decode "CalvisionDaqCommon;CalvisionDaqDigitizer;CalvisionDaqRoot")
make_CalvisionDaq_exec(readout "CalvisionDaqCommon;CalvisionDaqDigitizer")
make_CalvisionDaq_exec(calibrate "CalvisionDaqCommon;CalvisionDaqDigitizer")
