target_sources(AmalgamUI
    PRIVATE
        Private/Button.cpp
        Private/Component.cpp
        Private/Container.cpp
        Private/Image.cpp
        Private/Text.cpp
        Private/TextInput.cpp
        Private/Thumbnail.cpp
        Private/VerticalGridContainer.cpp
    PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
        Public/AUI/Button.h
        Public/AUI/Component.h
        Public/AUI/Container.h
        Public/AUI/Image.h
        Public/AUI/Text.h
        Public/AUI/TextInput.h
        Public/AUI/Thumbnail.h
        Public/AUI/VerticalGridContainer.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
