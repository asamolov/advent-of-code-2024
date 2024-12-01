cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(advent-of-code-2024)

add_subdirectory(day00)
add_custom_target(run00 COMMAND day00 DEPENDS day00 WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
add_subdirectory(day01)
add_custom_target(run01 COMMAND day01 DEPENDS day01 WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
