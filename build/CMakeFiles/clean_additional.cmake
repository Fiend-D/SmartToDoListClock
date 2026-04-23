# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles/SmartDeskClock_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/SmartDeskClock_autogen.dir/ParseCache.txt"
  "SmartDeskClock_autogen"
  )
endif()
