PROJECT(homework1)

set(CXX_STANDARD 14)

add_executable(homework1 main.cpp)
add_custom_command(TARGET homework1 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/in.txt
  ${CMAKE_CURRENT_BINARY_DIR}/in.txt)