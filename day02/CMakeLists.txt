add_executable (day02 "main.cpp")
find_package(fmt CONFIG REQUIRED)
target_link_libraries(day02 PRIVATE fmt::fmt)
set_property(TARGET day02 PROPERTY CXX_STANDARD 20)
file(GLOB INPUTS
  "input*.txt"
)
file(COPY ${INPUTS} DESTINATION ".")
