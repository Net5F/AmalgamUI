#include "AUI/TextBox.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI {

TextBox::TextBox(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, normalImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, hoveredImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, selectedImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, disabledImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, cursorImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, text(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, currentState{State::Normal}
, leftMargin{0}
{
    // Default to left-justifying the text within the button. The user can set
    // it otherwise if they care to.
    text.setVerticalAlignment(AUI::Text::VerticalAlignment::Middle);
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Left);

    screen.registerListener(EventType::MouseButtonDown, this);
    screen.registerListener(EventType::MouseMove, this);
    screen.registerListener(EventType::KeyDown, this);
    screen.registerListener(EventType::TextInput, this);
}

void TextBox::setLeftMargin(int inLeftMargin)
{
    leftMargin = inLeftMargin;
}

TextBox::State TextBox::getCurrentState()
{
    return currentState;
}

void TextBox::setOnTextChanged(std::function<void(void)> inOnTextChanged)
{
    onTextChanged = std::move(inOnTextChanged);
}

bool TextBox::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return false;
    }

    // If the mouse press was inside our extent, become selected.
    if (containsPoint({event.x, event.y})) {
        // Set our state to pressed.
        currentState = State::Selected;

        // Begin generating text input events.
        SDL_StartTextInput();

        return true;
    }
    else {
        // Else the mouse press was outside, become unselected.
        currentState = State::Normal;

        // Stop generating text input events.
        SDL_StopTextInput();

        return false;
    }
}

void TextBox::onMouseMove(SDL_MouseMotionEvent& event)
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If the mouse is inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If we're normal, change to hovered.
        if (currentState == State::Normal) {
            currentState = State::Hovered;
        }
    }
    else {
        // Else, the mouse isn't in our extent. If we're hovered, unhover.
        if (currentState == State::Hovered) {
            currentState = State::Normal;
        }
    }
}

bool TextBox::onKeyDown(SDL_KeyboardEvent& event)
{
    // If we aren't selected, ignore the event.
    if (currentState != State::Selected) {
        return false;
    }

    switch (event.keysym.sym) {
        case SDLK_BACKSPACE: {
            // If there's any text, delete the last character.
            text.removeLastChar();
            return true;
        }
        case SDLK_c: {
            // If this was a CTRL+c copy command.
            if (SDL_GetModState() & KMOD_CTRL) {
                // If there's text to copy, copy it to the clipboard.
                const std::string& textString = text.asString();
                if (textString.length() > 0) {
                    SDL_SetClipboardText(textString.c_str());
                }
            }
            return true;
        }
        case SDLK_x: {
            // If this was a CTRL+x cut command
            if (SDL_GetModState() & KMOD_CTRL) {
                // If there's text to copy, copy it to the clipboard.
                const std::string& textString = text.asString();
                if (textString.length() > 0) {
                    SDL_SetClipboardText(textString.c_str());
                }

                // Empty the text.
                text.setText("");
            }
            return true;
        }
        case SDLK_v: {
            // If this was a CTRL+v paste command.
            if (SDL_GetModState() & KMOD_CTRL) {
                // Paste the text from the clipboard to the end of the text.
                text.appendText(SDL_GetClipboardText());
            }
            return true;
        }
    }

    return false;
}

bool TextBox::onTextInput(SDL_TextInputEvent& event)
{
    // Append the user's new character to the end of the text.
    text.appendText(event.text);

    return true;
}

void TextBox::render(const SDL_Point& parentOffset)
{
    // Keep our extent up to date.
    refreshScaling();

    // Children should render at the parent's offset + this component's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += actualScreenExtent.x;
    childOffset.y += actualScreenExtent.y;

    // Save the extent that we should render at.
    lastRenderedExtent = actualScreenExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render the appropriate background image for our current state.
    switch (currentState) {
        case State::Normal: {
            normalImage.render(childOffset);
            break;
        }
        case State::Hovered: {
            hoveredImage.render(childOffset);
            break;
        }
        case State::Selected: {
            selectedImage.render(childOffset);
            break;
        }
        case State::Disabled: {
            disabledImage.render(childOffset);
            break;
        }
    }

    // Render the text, adding the left margin..
    text.render({childOffset.x + leftMargin, childOffset.y});

    // TODO: Render the cursor and hook up a tick.
}

} // namespace AUI
