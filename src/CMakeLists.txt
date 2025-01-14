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
    -march=armv8-a
    -fPIC
)

target_link_options(fclock PRIVATE
    -static
)

# Add dependencies to ensure libraries are built before fclock
add_dependencies(fclock confuse rgbmatrix)

target_link_libraries(fclock PRIVATE
    rgbmatrix
    confuse
    -static
    -pthread
    -lrt
    -lm
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
