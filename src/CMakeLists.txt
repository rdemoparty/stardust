add_subdirectory(framework)
add_subdirectory(game)
add_subdirectory(3rdparty)

set(SOURCE
   ${SOURCE}
   ${GLEW_SOURCES}
   PARENT_SCOPE
)
