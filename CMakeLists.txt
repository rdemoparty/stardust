cmake_minimum_required(VERSION 2.6)
project(stardust)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${stardust_SOURCE_DIR}/cmake")

# Bump up warning levels appropriately for clang, gcc & msvc and build in debug mode
# if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable -std=c++11")
# 	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
# 	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
# elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
# 	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
# 		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
# 	else()
# 		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
# 	endif()
# endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_directories(${GLEW_LIBRARY_DIRS})

include_directories("src/glm")
include_directories("src/framework")
include_directories("src/game")

add_subdirectory(src)

add_executable(stardust src/main.cpp ${SOURCE})
target_link_libraries(stardust ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
	