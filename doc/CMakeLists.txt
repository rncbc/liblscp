# project(liblscp)

find_package (Doxygen)

if (DOXYGEN_FOUND)
  set (top_srcdir "${CMAKE_CURRENT_SOURCE_DIR}/..")
  configure_file (liblscp.doxygen.in liblscp.doxygen IMMEDIATE @ONLY)
  add_custom_target (doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} liblscp.doxygen
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif ()
