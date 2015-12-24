cmake_minimum_required(VERSION 2.6)
project(stardust)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${stardust_SOURCE_DIR}/cmake")
set(GAME_EXECUTABLE_NAME "stardust")

# Bump up warning levels appropriately for clang, gcc & msvc and build in debug mode
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")

	# get rid of lua tmpnam deprecated warning
#	add_definitions(-DLUA_USE_POSIX)

	if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
	endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

set(BIN_PATH ../bin)

# if (WIN32)
# 	set(SDL2_INCLUDE_DIR "src/3rdparty/SDL2/include")
# 	if (CMAKE_CL_64)
# 		set(SDL2_LIBRARY "../src/3rdparty/SDL2/lib/x64/SDL2")
# 		set(SDL2_MAIN_LIBRARY "../src/3rdparty/SDL2/lib/x64/SDL2main")
# 		set(GAME_EXECUTABLE_NAME "stardust64.exe")
# 	else (CMAKE_CL_64)
# 		set(SDL2_LIBRARY "../src/3rdparty/SDL2/lib/x86/SDL2")
# 		set(SDL2_MAIN_LIBRARY "../src/3rdparty/SDL2/lib/x86/SDL2main")
# 		# GLM: on x86 there is an error related to align(16), so we force non SIMD here
# 		add_definitions(-DGLM_FORCE_PURE)
# 		set(GAME_EXECUTABLE_NAME "stardust32.exe")
# 	endif (CMAKE_CL_64)
# else(WIN32)
#	set(GAME_EXECUTABLE_NAME "stardust")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -static-libgcc -static-libstdc++ -Wunused-but-set-variable")
# endif(WIN32)

# SDL setup
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# SDL image setup
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

# SDL mixer setup
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

# GL setup
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})

# FREETYPE setup
find_package(Freetype REQUIRED)
include_directories(${Freetype_INCLUDE_DIRS})
link_directories(${Freetype_LIBRARY_DIRS})
# freetype header location for mac
include_directories("/usr/local/opt/freetype/include/")
include_directories("/usr/local/opt/freetype/include/freetype2")
# freetype header location for linux
include_directories("/usr/include/freetype2")

# GLEW setup
include_directories(./src/3rdparty/glew/include)
file(GLOB GLEW_SOURCES ./src/3rdparty/glew/src/glew.c)
add_definitions(-DGLEW_STATIC)


if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	# mitigates https://trac.macports.org/ticket/49004. glew build takes a lot of time on clang
	add_definitions(-Os)
endif()

include_directories("src/")
include_directories("src/3rdparty/glm")
include_directories("src/3rdparty/lua-5.2.3/src")
include_directories("src/3rdparty/easyloggingpp")
include_directories("src/3rdparty/mongoose")
include_directories("src/framework")
include_directories("src/game")
include_directories("src/game/scripting")
add_subdirectory(src)

add_executable(stardust src/main.cpp ${SOURCE})
target_link_libraries(stardust ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})
if (WIN32)
	target_link_libraries(stardust -lws2_32)
else (WIN32)
endif (WIN32)

# set_target_properties(stardust PROPERTIES OUTPUT_NAME ${GAME_EXECUTABLE_NAME})
set_target_properties(stardust PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})
set_target_properties(stardust PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_PATH})
set_target_properties(stardust PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_PATH})
