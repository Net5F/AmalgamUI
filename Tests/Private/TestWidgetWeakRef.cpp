#include "catch2/catch_all.hpp"
#include "AUI/Screen.h"
#include "AUI/Button.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/Internal/Log.h"
#include <memory>

using namespace AUI;

TEST_CASE("TestWidgetWeakRef")
{
    Screen screen{"TestScreen"};

    SECTION("Construction")
    {
        Button widget1{screen, {}};
        std::unique_ptr<Widget> widget2{
            std::make_unique<Button>(screen, SDL_Rect{})};
        Button widget3{screen, {}};
        {
            WidgetWeakRef ref1(widget1);
            WidgetWeakRef ref2(*widget2);
            WidgetWeakRef ref3(widget3);
            REQUIRE(widget1.getRefCount() == 1);
            REQUIRE(widget2->getRefCount() == 1);
            REQUIRE(widget3.getRefCount() == 1);
        }
        REQUIRE(widget1.getRefCount() == 0);
        REQUIRE(widget2->getRefCount() == 0);
        REQUIRE(widget3.getRefCount() == 0);
    }

    SECTION("Copy construction")
    {
        Button widget1{screen, {}};
        {
            WidgetWeakRef ref1(widget1);
            REQUIRE(widget1.getRefCount() == 1);

            WidgetWeakRef ref11(ref1);
            REQUIRE(widget1.getRefCount() == 2);
        }
        REQUIRE(widget1.getRefCount() == 0);
    }

    SECTION("Move construction")
    {
        Button widget1{screen, {}};
        {
            WidgetWeakRef ref1(widget1);
            REQUIRE(widget1.getRefCount() == 1);

            WidgetWeakRef ref2(std::move(ref1));
            REQUIRE(widget1.getRefCount() == 1);
        }
        REQUIRE(widget1.getRefCount() == 0);
    }

    SECTION("Copy assignment")
    {
        Button widget1{screen, {}};
        WidgetWeakRef ref1{widget1};
        REQUIRE(widget1.getRefCount() == 1);

        {
            WidgetWeakRef ref2 = ref1;
            WidgetWeakRef ref3 = ref2;
            REQUIRE(widget1.getRefCount() == 3);
        }
        REQUIRE(widget1.getRefCount() == 1);
    }

    SECTION("Move assignment")
    {
        Button widget1{screen, {}};
        WidgetWeakRef ref1{widget1};
        REQUIRE(widget1.getRefCount() == 1);

        {
            WidgetWeakRef ref2 = std::move(ref1);
            REQUIRE(widget1.getRefCount() == 1);
        }
        REQUIRE(widget1.getRefCount() == 0);
    }

    SECTION("Invalidate single ref")
    {
        std::unique_ptr<Widget> widget1{
            std::make_unique<Button>(screen, SDL_Rect{})};
        std::unique_ptr<Widget> widget2{
            std::make_unique<Button>(screen, SDL_Rect{})};

        WidgetWeakRef ref1{*widget1};
        WidgetWeakRef ref2{*widget2};
        REQUIRE(ref1.isValid());
        REQUIRE(ref2.isValid());

        widget1 = nullptr;
        REQUIRE(!ref1.isValid());
        REQUIRE(ref2.isValid());

        widget2 = nullptr;
        REQUIRE(!ref1.isValid());
        REQUIRE(!ref2.isValid());
    }

    SECTION("Invalidate multiple refs")
    {
        std::unique_ptr<Widget> widget1{
            std::make_unique<Button>(screen, SDL_Rect{})};

        WidgetWeakRef ref1{*widget1};
        WidgetWeakRef ref2{*widget1};
        WidgetWeakRef ref3{*widget1};
        REQUIRE(ref1.isValid());
        REQUIRE(ref2.isValid());
        REQUIRE(ref3.isValid());

        widget1 = nullptr;
        REQUIRE(!ref1.isValid());
        REQUIRE(!ref2.isValid());
        REQUIRE(!ref3.isValid());
    }
}
