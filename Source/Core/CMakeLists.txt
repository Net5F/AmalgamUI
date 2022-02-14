target_sources(AmalgamUI
	PRIVATE
		Private/Core.cpp
		Private/Initializer.cpp
		Private/Screen.cpp
		Private/Widget.cpp
		Private/Window.cpp
	PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
		Public/AUI/Core.h
		Public/AUI/Initializer.h
		Public/AUI/Screen.h
		Public/AUI/ScreenResolution.h
		Public/AUI/Widget.h
		Public/AUI/Window.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Public
)
