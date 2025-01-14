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

# Link order matters: dynamic libs first, then static libs
target_link_options(fclock PRIVATE
    -static-libgcc
    -static-libstdc++
    -Wl,--as-needed
    -Wl,-Bstatic
    -Wl,--start-group
)

target_link_libraries(fclock PRIVATE
    rgbmatrix
    confuse
    pthread
    rt
    m
)

target_link_options(fclock PRIVATE
    -Wl,--end-group
    -Wl,-Bdynamic
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
