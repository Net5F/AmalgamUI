target_sources(AmalgamUI
	PRIVATE
		Private/Component.cpp
		Private/Image.cpp
	PUBLIC
        # Note: We add the extra "AUI" directory so that consumers can include
        # files as "AUI/Xyz.h" for some extra clarity.
		Public/AUI/Component.h
		Public/AUI/Image.h
)

target_include_directories(AmalgamUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Public
)
