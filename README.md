# Advent of Code 2024

This year - learning modern C++.

# Template

[`day00`](./day00) folder contains trivial template for the project. Will collect useful stuff.

To create a new project from the template, please copy it to `dayXX` and replace `day00->dayXX` in the copied CMakeLists.txt.

Also, add following two lines to the root CMakeLists.txt:

```
add_subdirectory(dayXX)
add_custom_target(runXX COMMAND dayXX DEPENDS dayXX WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
```
