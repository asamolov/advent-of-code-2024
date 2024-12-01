add_executable (day01 "main.cpp" "../utils.h")
find_package(fmt CONFIG REQUIRED)
target_link_libraries(day01 PRIVATE fmt::fmt)
set_property(TARGET day01 PROPERTY CXX_STANDARD 20)
file(GLOB INPUTS
  "input*.txt"
)
file(COPY ${INPUTS} DESTINATION ".")
