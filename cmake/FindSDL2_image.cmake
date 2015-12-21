# - Locate SDL2_image library
# This module defines:
#  SDL2_IMAGE_FOUND - System has SDL2_image
#  SDL2_IMAGE_INCLUDE_DIRS - SDL2_image include directories
#  SDL2_IMAGE_LIBRARIES - Libraries needed to link against SDL2_image

find_path(SDL2_IMAGE_INCLUDE_DIR SDL_mixer.h
		PATH_SUFFIXES include/SDL2 include SDL2
		i686-w64-mingw32/include/SDL2
		i686-w64-mingw32/include
		x86_64-w64-mingw32/include/SDL2
		x86_64-w64-mingw32/include
		PATHS
		/opt/cross-tools
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local/include/SDL2
		/usr/include/SDL2
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
		)

find_library(SDL2_IMAGE_LIBRARY
		NAMES SDL2_image
		PATH_SUFFIXES lib
		i686-w64-mingw32/lib
		)

set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
		SDL2_image
		DEFAULT_MSG SDL2_IMAGE_LIBRARIES SDL2_IMAGE_INCLUDE_DIRS)

mark_as_advanced(SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)