cmake_minimum_required(VERSION 3.0.0)
#这个link_libraries(ws2_32)要先与project，要不不会生效，踩过坑
link_libraries(ws2_32)
project(nlink_unpack VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(
  nlink_unpack
        main.c
  nlink_utils.c
  nlink_linktrack_nodeframe1.c
  cJSON.c
        s2j.c

        uwb_to_json.c uwb_to_json.h)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
