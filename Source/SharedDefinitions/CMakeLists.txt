target_sources(AmalgamUI
    PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
        Public/AUI/Orientation.h
        Public/AUI/Padding.h
)

target_include_directories(AmalgamUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
