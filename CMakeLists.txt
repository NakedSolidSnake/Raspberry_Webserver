cmake_minimum_required(VERSION 3.7.0)

project(WebServer)

set (CMAKE_C_FLAGS "-D_GNU_SOURCE -Wall -std=c99")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)


#build webserver
add_executable(webserver "")

target_link_libraries(webserver PUBLIC hardware pthread)

target_sources(webserver
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/http.c  
  ${CMAKE_CURRENT_LIST_DIR}/src/server.c
  ${CMAKE_CURRENT_LIST_DIR}/src/webserver.c
  )

target_include_directories(webserver 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src  
  )

install(TARGETS webserver DESTINATION bin)

file(COPY css DESTINATION bin)
file(COPY js DESTINATION bin)
file(COPY page DESTINATION bin)
