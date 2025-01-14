
if(NOT "/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitinfo.txt" IS_NEWER_THAN "/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/njr/code/fclock/external/rpi-rgb-led-matrix"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/njr/code/fclock/external/rpi-rgb-led-matrix'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"  clone --no-checkout --config "advice.detachedHead=false" "https://github.com/hzeller/rpi-rgb-led-matrix.git" "rpi-rgb-led-matrix"
    WORKING_DIRECTORY "/home/njr/code/fclock/external"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/hzeller/rpi-rgb-led-matrix.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"  checkout master --
  WORKING_DIRECTORY "/home/njr/code/fclock/external/rpi-rgb-led-matrix"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git"  submodule update --recursive --init 
    WORKING_DIRECTORY "/home/njr/code/fclock/external/rpi-rgb-led-matrix"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/njr/code/fclock/external/rpi-rgb-led-matrix'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitinfo.txt"
    "/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/njr/code/fclock/obj-arm-linux-gnueabihf/rpi-rgb-led-matrix-prefix/src/rpi-rgb-led-matrix-stamp/rpi-rgb-led-matrix-gitclone-lastrun.txt'")
endif()

