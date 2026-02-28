cmake_minimum_required(VERSION 3.14)

project(AmalgamUI)

###############################################################################
# Build Options
###############################################################################
option(AUI_BUILD_TESTS "Build AUI unit tests." OFF)
###############################################################################

message(STATUS "Configuring AmalgamUI")

# Find SDL3 and associated libs.
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)

# Add our library target
add_library(AmalgamUI STATIC "")

# Specify our dependencies.
target_link_libraries(AmalgamUI
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
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
