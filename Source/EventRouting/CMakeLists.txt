target_sources(AmalgamUI
    PRIVATE
        Private/EventRouter.cpp
    PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
        Public/AUI/DragDropData.h
        Public/AUI/EventResult.h
        Public/AUI/EventRouter.h
        Public/AUI/FocusLostType.h
        Public/AUI/MouseButtonType.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
