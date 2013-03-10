# - Main CMake script for IsingMC
#
# On windows run 
# 'cmake -G "MSYS Makefiles" '

# - Require lastest generatation of CMake
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# - Declare the project
project(IsingMC CXX RC)

# - Tell compiler where to find headers
# include_directories(include)

# - Add the IsingMC libaray
#add_library(IsingMClib myClass.h)

# REQUIRED does not work in CMake <=2.4.6 for SDL
Find_Package ( SDL REQUIRED )
Find_Package ( SDL_image REQUIRED ) # if using SDL_image

# Workaround for the non-working REQUIRED flag
if ( NOT SDL_FOUND )
   message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY} # if using SDL_image, obviously
   SDLmain # Sadly not included in SDL_LIBRARY variable
)

# - Add the IsingMC executable
add_executable(IsingMC IsingMC.cpp myClass.h)

# - Link pp6calculator to the IsingMC library
# target_link_libraries(IsingMC IsingMClib)

# - Install IsingMC to the bin subdirectory of CMAKE_INSTALL_PREFIX
install(TARGETS IsingMC DESTINATION bin)

# - Package IsingMC sources and executable
include(CPack)