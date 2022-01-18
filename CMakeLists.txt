cmake_minimum_required(VERSION 3.0.0)
project(ceplug VERSION 0.1.0)

set(TARGET_DIR "~/.local/share/Steam/steamapps/common/Portal\ 2\ Community\ Edition/")

FILE(GLOB cppSrc *.cpp)
add_library(ceplug SHARED ${cppSrc})
target_include_directories(ceplug PRIVATE .)
add_custom_command(TARGET ceplug POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ceplug> ${TARGET_DIR}/ceplug.so
	COMMENT "Created ${TARGET_DIR}/ceplug.so"
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -Wall -Werror -O0")
SET(CMAKE_SHARED_LINKER_FLAGS "")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
