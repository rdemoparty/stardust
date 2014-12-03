add_subdirectory(framework)
add_subdirectory(game)

set(SOURCE
   ${SOURCE}
   ${GLEW_SOURCES}
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   PARENT_SCOPE
)
