target_sources(AmalgamUI
    PRIVATE
        Private/ImageType/ImageType.cpp
        Private/ImageType/MultiResImage.cpp
        Private/ImageType/NineSliceImage.cpp
        Private/ImageType/SimpleImage.cpp
        Private/ImageType/TiledImage.cpp
        Private/Button.cpp
        Private/Checkbox.cpp
        Private/CollapsibleContainer.cpp
        Private/ConfirmationDialog.cpp
        Private/Container.cpp
        Private/HorizontalGridContainer.cpp
        Private/Image.cpp
        Private/ScrollArea.cpp
        Private/Text.cpp
        Private/TextButton.cpp
        Private/TextInput.cpp
        Private/Thumbnail.cpp
        Private/VerticalGridContainer.cpp
        Private/VerticalListContainer.cpp
    PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
        Public/AUI/ImageType/ImageType.h
        Public/AUI/ImageType/MultiResImage.h
        Public/AUI/ImageType/NineSliceImage.h
        Public/AUI/ImageType/SimpleImage.h
        Public/AUI/ImageType/TiledImage.h
        Public/AUI/Button.h
        Public/AUI/Checkbox.h
        Public/AUI/CollapsibleContainer.h
        Public/AUI/ConfirmationDialog.h
        Public/AUI/Container.h
        Public/AUI/HorizontalGridContainer.h
        Public/AUI/Image.h
        Public/AUI/ScrollArea.h
        Public/AUI/Text.h
        Public/AUI/TextButton.h
        Public/AUI/TextInput.h
        Public/AUI/Thumbnail.h
        Public/AUI/VerticalGridContainer.h
        Public/AUI/VerticalListContainer.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
