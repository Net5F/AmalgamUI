target_sources(AmalgamUI
    PRIVATE
        Private/Log.cpp
        Private/AssetCache.cpp
        Private/ScalingHelpers.cpp
        Private/SDLHelpers.cpp
        Private/WidgetLocator.cpp
        Private/WidgetPath.cpp
        Private/WidgetWeakRef.cpp
    PUBLIC
        Public/AUI/AssetCache.h
        Public/AUI/ScalingHelpers.h
        Public/AUI/SDLHelpers.h
        Public/AUI/WidgetLocator.h
        Public/AUI/WidgetPath.h
        Public/AUI/WidgetWeakRef.h

        # Note: We add the extra "AUI/Internal" directory so that we don't
        #       pollute an IDE's suggestions with files that the consumer
        #       isn't likely to need.
        Public/AUI/Internal/Assert.h
        Public/AUI/Internal/Ignore.h
        Public/AUI/Internal/Log.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)
