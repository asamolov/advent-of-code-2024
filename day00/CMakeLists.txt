add_executable (day00 "main.cpp")
find_package(fmt CONFIG REQUIRED)
target_link_libraries(day00 PRIVATE fmt::fmt)
set_property(TARGET day00 PROPERTY CXX_STANDARD 20)
