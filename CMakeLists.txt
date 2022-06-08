cmake_minimum_required(VERSION 3.14)

project(AmalgamUI)

###############################################################################
# Build Options
###############################################################################
option(AUI_BUILD_TESTS "Build AUI unit tests." OFF)
###############################################################################

message(STATUS "Configuring AmalgamUI")

# Tell CMake where the CMake folder is, since our source dir isn't the base dir.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake/")

# Find SDL2 and associated libs.
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add our library target
add_library(AmalgamUI STATIC "")

# Include the headers from our dependencies.
target_include_directories(AmalgamUI
    PRIVATE
        ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} 
        ${SDL2_TTF_INCLUDE_DIRS}
)

# Specify our dependencies.
target_link_libraries(AmalgamUI
    PRIVATE
        ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
)

# Compile with C++20.
target_compile_features(AmalgamUI PRIVATE cxx_std_20)
set_target_properties(AmalgamUI PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(AmalgamUI PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(AmalgamUI PUBLIC /W3 /permissive-)
endif()

# Add sources to our library target.
add_subdirectory(Source)

# Optionally build our unit tests.
if (AUI_BUILD_TESTS)
    add_subdirectory(Tests)
endif()
