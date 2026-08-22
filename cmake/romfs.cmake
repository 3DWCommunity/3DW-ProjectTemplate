function(generate_romfs)
    string(REPLACE "0x" "" TITLE_ID_NOPREFIX ${TITLE_ID})
    make_directory(${CMAKE_CURRENT_BINARY_DIR}/sd/atmosphere/contents/${TITLE_ID_NOPREFIX}/romfs)
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Copying sd"
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/res/sd/* ${CMAKE_CURRENT_BINARY_DIR}/sd
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "-- Copying romfs"
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/res/romfs/* ${CMAKE_CURRENT_BINARY_DIR}/sd/atmosphere/contents/${TITLE_ID_NOPREFIX}/romfs
    )
endfunction()
