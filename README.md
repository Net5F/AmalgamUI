# AmalgamUI
A simple image-based UI library, built on the SDL2 native renderer.

# Library build as CMake target
Assuming a library directory at `PROJECT_SOURCE_DIR/Libraries`:
```
# Configure AmalgamUI.
add_subdirectory("${PROJECT_SOURCE_DIR}/../Libraries/AmalgamUI/"
                 "${PROJECT_BINARY_DIR}/Libraries/AmalgamUI/")
```

# Standalone unit test build
```
mkdir Build
cd Build
cmake -DCMAKE_BUILD_TYPE=Debug -DAUI_BUILD_TESTS=On -G Ninja ..
ninja all
./Tests/AUIUnitTests.exe
```
