# project(liblscp)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories (${CMAKE_SOURCE_DIR})

add_executable (example_server
  example_server.c
  server.h
  parser.h
  server.c
  parser.c  
)

add_executable (example_client
  example_client.c
)

target_link_libraries (example_server PRIVATE ${PROJECT_NAME})
target_link_libraries (example_client PRIVATE ${PROJECT_NAME})
