# project(liblscp)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories (
  ${CMAKE_SOURCE_DIR}/lscp
)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
    file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/config.h)
endif()
configure_file (cmake_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (EXISTS ${CMAKE_SOURCE_DIR}/lscp/version.h)
    file(REMOVE ${CMAKE_SOURCE_DIR}/lscp/version.h)
endif()
configure_file (${CMAKE_SOURCE_DIR}/lscp/version.h.in ${CMAKE_SOURCE_DIR}/lscp/version.h)


set (PUBLIC_HEADERS
  ${CMAKE_SOURCE_DIR}/lscp/client.h
  ${CMAKE_SOURCE_DIR}/lscp/device.h
  ${CMAKE_SOURCE_DIR}/lscp/event.h
  ${CMAKE_SOURCE_DIR}/lscp/socket.h
  ${CMAKE_SOURCE_DIR}/lscp/thread.h
  ${CMAKE_SOURCE_DIR}/lscp/version.h
)

set (HEADERS
  common.h
)

set (SOURCES
  client.c
  common.c
  device.c
  event.c
  socket.c
  thread.c
)

add_library (${PROJECT_NAME} SHARED
  ${HEADERS}
  ${SOURCES}
)

target_link_libraries (${PROJECT_NAME} PUBLIC pthread)

set_target_properties (${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
  PREFIX "lib"
  OUTPUT_NAME "lscp"
  VERSION ${SHARED_VERSION_INFO}
  SOVERSION ${SHARED_VERSION_CURRENT})

install (TARGETS ${PROJECT_NAME} LIBRARY
  DESTINATION ${CONFIG_LIBDIR}
  PUBLIC_HEADER
  DESTINATION ${CONFIG_INCLUDEDIR}/lscp)
