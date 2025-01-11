add_executable(fclock
    fclock.cc
    text_writer.cc
    clock_config.cc
)

target_compile_options(fclock PRIVATE
    -Wall
    -O3
    -g
    -Wextra
    -Wno-unused-parameter
)

target_link_libraries(fclock PRIVATE
    rgbmatrix
    ${CONFUSE_LIBRARY}
    ${PTHREAD_LIBRARY}
    ${RT_LIBRARY}
    ${MATH_LIBRARY}
)

# Install targets
install(TARGETS fclock
    RUNTIME DESTINATION bin
)

# Install fonts
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../external/rpi-rgb-led-matrix/fonts/
    DESTINATION share/fclock/fonts
    FILES_MATCHING PATTERN "*.bdf"
)

# Install default config
install(FILES fclock.conf
    DESTINATION share/fclock
)
