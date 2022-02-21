#include "catch2/catch_all.hpp"
#include "AUI/Screen.h"
#include "AUI/Button.h"
#include "AUI/WidgetPath.h"
#include "AUI/Internal/Log.h"
#include <memory>

using namespace AUI;

TEST_CASE("TestWidgetPath")
{
    Screen screen{"TestScreen"};

    SECTION("Ref count updates properly.")
    {
        Button widget1{screen, {}};
        std::unique_ptr<Widget> widget2{std::make_unique<Button>(screen, SDL_Rect{})};
        Button widget3{screen, {}};
        {
            WidgetPath path;
            path.add(&widget1);
            path.add(widget2.get());
            path.insert(&widget3, 1);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 1);
            REQUIRE(widget3.getRefCount() == 1);

            path.remove(&widget3);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 1);
            REQUIRE(widget3.getRefCount() == 0);

            path.remove(widget2.get());
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 0);
            REQUIRE(widget3.getRefCount() == 0);
        }
        REQUIRE(widget1.getRefCount() == 0);
        REQUIRE(widget2->getRefCount() == 0);
        REQUIRE(widget3.getRefCount() == 0);
    }
}
