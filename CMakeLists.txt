cmake_minimum_required(VERSION 3.14)
project(AmalgamUI)

message(STATUS "Configuring AmalgamUI")

# Provide header-only EnTT, if it hasn't already been supplied.
# Used for the resource manager and hashed string.
if (NOT TARGET EnTT)
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libraries/entt/"
                     "${PROJECT_BINARY_DIR}/Libraries/entt/")
endif()

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
        ${SDL2_TTF_LIBRARIES} ${SDL2_GFX_LIBRARIES}
        EnTT::EnTT
)

# Compile with C++17.
target_compile_features(AmalgamUI PRIVATE cxx_std_17)
set_target_properties(AmalgamUI PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
target_compile_options(AmalgamUI PUBLIC -Wall -Wextra)

# Add sources to our library target.
add_subdirectory(Source)
