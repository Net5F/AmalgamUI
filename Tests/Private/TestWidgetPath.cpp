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

    // Note: We skip testing any functions that just forward to std::vector,
    //       unless there's some interesting interaction to test.
    SECTION("Ref count updates properly.")
    {
        Button widget1{screen, {}};
        std::unique_ptr<Widget> widget2{std::make_unique<Button>(screen, SDL_Rect{})};
        Button widget3{screen, {}};

        {
            WidgetPath path;
            path.push_back(widget1);
            path.push_back(*widget2.get());
            path.insert(path.begin() + 1, widget3);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 1);
            REQUIRE(widget3.getRefCount() == 1);

            path.erase(path.begin() + 1);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 1);
            REQUIRE(widget3.getRefCount() == 0);

            path.erase(path.end() - 1);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 0);
            REQUIRE(widget3.getRefCount() == 0);
        }

        REQUIRE(widget1.getRefCount() == 0);
        REQUIRE(widget2->getRefCount() == 0);
        REQUIRE(widget3.getRefCount() == 0);
    }

    SECTION("Ref validity updates properly")
    {
        WidgetPath path;

        {
            Button widget1{screen, {}};
            Button widget2{screen, {}};
            Button widget3{screen, {}};

            path.push_back(widget1);
            path.push_back(widget2);
            path.push_back(widget3);

            for (const WidgetWeakRef& ref : path) {
                REQUIRE(ref.isValid());
            }
        }

        for (const WidgetWeakRef& ref : path) {
            REQUIRE(!(ref.isValid()));
        }
    }

    SECTION("Widget order is correct")
    {
        WidgetPath path;

        Button widget1{screen, {}};
        Button widget2{screen, {}};
        Button widget3{screen, {}};

        path.push_back(widget1);
        path.push_back(widget2);
        path.push_back(widget3);

        REQUIRE(&(path[0].get()) == &widget1);
        REQUIRE(&(path[1].get()) == &widget2);
        REQUIRE(&(path[2].get()) == &widget3);
    }
}
