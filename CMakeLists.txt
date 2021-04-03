cmake_minimum_required(VERSION 3.14)
project(AmalgamUI)

message(STATUS "Configuring AmalgamUI")

# Add our library target
add_library(AmalgamUI STATIC "")

# Specify our dependencies.
target_link_libraries(AmalgamUI
    PRIVATE
        ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES} ${SDL2_GFX_LIBRARIES}
)

# Compile with C++14.
target_compile_features(AmalgamUI PRIVATE cxx_std_14)
set_target_properties(AmalgamUI PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
target_compile_options(AmalgamUI PUBLIC -Wall -Wextra)

# Add sources to our library target.
add_subdirectory(Source)
