cmake_minimum_required(VERSION 3.5)

message(STATUS "AUI: Configuring Unit Tests")

# Configure Catch2.
if (NOT TARGET Catch2::Catch2)
    message(STATUS "AUI: Catch2 target not provided. Configuring Catch2.")
    SET(CATCH_BUILD_TESTING OFF CACHE BOOL "Build SelfTest project")
    SET(CATCH_INSTALL_DOCS OFF CACHE BOOL "Install documentation alongside library")
    add_subdirectory("${PROJECT_SOURCE_DIR}/Libraries/Catch2/"
                     "${PROJECT_BINARY_DIR}/Libraries/Catch2/")
 endif()

# Add the executable.
add_executable(AUIUnitTests
    Private/TestMain.cpp
    Private/TestWidgetWeakRef.cpp
    Private/TestWidgetPath.cpp
)

# Include our source dir.
target_include_directories(AUIUnitTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

# Link our dependencies.
target_link_libraries(AUIUnitTests
    PRIVATE
        AmalgamUI
        Catch2::Catch2
)

# Compile with C++17
target_compile_features(AUIUnitTests PRIVATE cxx_std_17)
set_target_properties(AUIUnitTests PROPERTIES CXX_EXTENSIONS OFF)
