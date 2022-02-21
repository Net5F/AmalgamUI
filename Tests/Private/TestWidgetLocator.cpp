#include "catch2/catch_all.hpp"
#include "AUI/Screen.h"
#include "AUI/Window.h"
#include "AUI/VerticalGridContainer.h"
#include "AUI/Image.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <memory>

using namespace AUI;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

class TestWidget : public Widget
{
public:
    TestWidget(Screen& inScreen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "")
    : Widget(inScreen, inLogicalExtent, inDebugName)
    , image{inScreen, {0, 0, (inLogicalExtent.w / 2)
            , (inLogicalExtent.h / 2)}, "Image"}
    {
        children.push_back(image);
    }

private:
    Image image;
};

/**
 * A widget containing a widget tree to use for testing.
 *
 * The widget contains a grid container holding three widgets, laid out
 * horizontally.
 *
 * Things are laid out such that a hit test in the top left corner should hit
 * (top to bottom) Image -> TestWidget -> VerticalGridContainer
 *                 -> TestParentWidget,
 * while a hit test in the bottom right corner should only hit
 * TestWidgetParent.
 */
class TestWidgetParent : public Widget
{
public:
    TestWidgetParent(Screen& screen)
    : Widget(screen, {200, 200, 400, 400}, "TestWidgetParent")
    , widgetContainer(screen, {0, 0, 200, 200}, "GridContainer")
    {
        children.push_back(widgetContainer);

        widgetContainer.setCellWidth(100);
        widgetContainer.setCellHeight(100);
        widgetContainer.setNumColumns(3);

        widgetContainer.push_back(
            std::make_unique<TestWidget>(screen, SDL_Rect{0, 0, 100, 100}, "Widget1"));
        widgetContainer.push_back(
            std::make_unique<TestWidget>(screen, SDL_Rect{0, 0, 100, 100}, "Widget2"));
        widgetContainer.push_back(
            std::make_unique<TestWidget>(screen, SDL_Rect{0, 0, 100, 100}, "Widget3"));
    }

    virtual ~TestWidgetParent()
    {
    }

    VerticalGridContainer widgetContainer;
};

TEST_CASE("TestWidgetLocator")
{
    Screen screen{"TestScreen"};

    WidgetLocator widgetLocator({200, 200, 400, 400});

    SECTION("Grid cell extent")
    {
        SDL_Rect gridCellExtent{widgetLocator.getGridCellExtent()};
        REQUIRE(gridCellExtent.x == 1);
        REQUIRE(gridCellExtent.y == 1);
        REQUIRE(gridCellExtent.w == 4);
        REQUIRE(gridCellExtent.h == 4);
    }

    SECTION("Add and remove widgets")
    {
        Image image1{screen, {200, 200, 400, 400}, "Image1"};
        Image image2{screen, {200, 200, 200, 200}, "Image2"};
        Image image3{screen, {200, 200, 100, 100}, "Image3"};

        // Lay out the widgets to set their renderExtent and have them add
        // themselves to the locator.
        image1.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image2.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image3.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);

        widgetLocator.removeWidget(&image1);
        widgetLocator.removeWidget(&image2);
        widgetLocator.removeWidget(&image3);
    }

    SECTION("Basic widget path")
    {
        Image image1{screen, {200, 200, 400, 400}, "Image1"};
        Image image2{screen, {200, 200, 200, 200}, "Image2"};
        Image image3{screen, {200, 200, 100, 100}, "Image3"};

        // Lay out the widgets to set their renderExtent and have them add
        // themselves to the locator.
        image1.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image2.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image3.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);

        WidgetPath widgetPath{widgetLocator.getPathUnderPoint({210, 210})};
        REQUIRE(widgetPath.size() == 3);
    }

    SECTION("More complicated widget path")
    {
        TestWidgetParent widget{screen};

        // Lay out the widgets to set their renderExtent and have them add
        // themselves to the locator.
        widget.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);

        {
            WidgetPath widgetPath{widgetLocator.getPathUnderPoint({210, 210})};
            REQUIRE(widgetPath.size() == 4);
        }
        {
            WidgetPath widgetPath{widgetLocator.getPathUnderPoint({580, 580})};
            REQUIRE(widgetPath.size() == 1);
        }
    }

    SECTION("Add and remove widgets with paths")
    {
        Image image1{screen, {200, 200, 400, 400}, "Image1"};
        Image image2{screen, {200, 200, 200, 200}, "Image2"};
        Image image3{screen, {200, 200, 100, 100}, "Image3"};

        // Lay out the widgets to set their renderExtent and have them add
        // themselves to the locator.
        image1.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image2.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);
        image3.updateLayout({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, &widgetLocator);

        WidgetPath widgetPath{widgetLocator.getPathUnderPoint({210, 210})};
        REQUIRE(widgetPath.size() == 3);

        widgetLocator.removeWidget(&image1);
        widgetLocator.removeWidget(&image2);
        widgetLocator.removeWidget(&image3);

        WidgetPath widgetPath2{widgetLocator.getPathUnderPoint({210, 210})};
        REQUIRE(widgetPath2.size() == 0);
    }
}
