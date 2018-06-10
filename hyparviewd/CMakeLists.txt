smfc_gen(
    CPP
    TARGET_NAME hyparview_fbs
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/hyparview_message.fbs
        ${CMAKE_CURRENT_SOURCE_DIR}/hyparview_protocol.fbs)

add_executable(hyparviewd server.cc ${hyparview_fbs})
target_link_libraries(hyparviewd smf)
target_include_directories(hyparviewd
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC $<INSTALL_INTERFACE:include/hyparview>)
