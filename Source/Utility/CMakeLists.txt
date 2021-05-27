target_sources(AmalgamUI
    PRIVATE
        Private/Log.cpp
        Private/ResourceManager.cpp
        Private/ScalingHelpers.cpp
        Private/Timer.cpp
    PUBLIC
        # Note: We add the extra "AUI/Internal" directories so that files don't
        #       clash with consumer files.
        Public/AUI/Internal/Ignore.h
        Public/AUI/Internal/Log.h
        Public/AUI/Internal/ResourceManager.h
        Public/AUI/Internal/ScalingHelpers.h
        Public/AUI/Internal/Timer.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
