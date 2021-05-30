#include "AUI/TextBox.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include <cstring>

namespace AUI {

TextBox::TextBox(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, normalImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, hoveredImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, selectedImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, disabledImage(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, text(screen, "", {0, 0, screenExtent.w, screenExtent.h})
, currentState{State::Normal}
, margin{0}
, cursorColor{0, 0, 0, 255}
, cursorWidth{2}
, cursorIndex{0}
, cursorIsVisible{false}
{
    // Default to left-justifying the text within the button. The user can set
    // it otherwise if they care to.
    text.setVerticalAlignment(AUI::Text::VerticalAlignment::Middle);
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Left);

    // Set the underlying text to empty string since it defaults to something
    // else.
    text.setText("");

    // Register for the events that we want to listen for.
    screen.registerListener(EventType::MouseButtonDown, this);
    screen.registerListener(EventType::MouseMove, this);
    screen.registerListener(EventType::KeyDown, this);
    screen.registerListener(EventType::TextInput, this);
    screen.registerListener(EventType::Tick, this);
}

void TextBox::setMargin(int inMargin)
{
    margin = inMargin;
}

void TextBox::setCursorColor(const SDL_Color& inCursorColor)
{
    cursorColor = inCursorColor;
}

void TextBox::setCursorWidth(unsigned int inCursorWidth)
{
    cursorWidth = inCursorWidth;
}

void TextBox::setText(std::string_view inText)
{
    // Set the text member's text.
    text.setText(inText);

    // Update the cursor.
    cursorIndex = inText.length();
}

void TextBox::setTextFont(const std::string& relPath, int size)
{
    text.setFont(relPath, size);
}

void TextBox::setTextColor(const SDL_Color& inColor)
{
    text.setColor(inColor);
}

void TextBox::setTextHorizontalAlignment(Text::HorizontalAlignment inHorizontalAlignment)
{
    text.setHorizontalAlignment(inHorizontalAlignment);
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

        // Reset the text cursor's state.
        cursorIsVisible = false;
        accumulatedBlinkTime = 0;

        return true;
    }
    else {
        // Else the mouse press was outside, become unselected.
        currentState = State::Normal;

        // Stop generating text input events.
        SDL_StopTextInput();

        // Reset the text cursor's state.
        cursorIsVisible = false;

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
            handleBackspaceEvent();
            return true;
        }
        case SDLK_DELETE: {
            handleDeleteEvent();
            return true;
        }
        case SDLK_c: {
            // If this was a CTRL+c copy command.
            if (SDL_GetModState() & KMOD_CTRL) {
                handleCopyEvent();
            }
            return true;
        }
        case SDLK_x: {
            // If this was a CTRL+x cut command
            if (SDL_GetModState() & KMOD_CTRL) {
                handleCutEvent();
            }
            return true;
        }
        case SDLK_v: {
            // If this was a CTRL+v paste command.
            if (SDL_GetModState() & KMOD_CTRL) {
                handlePasteEvent();
            }
            return true;
        }
        case SDLK_LEFT: {
            handleLeftEvent();
            return true;
        }
        case SDLK_RIGHT: {
            handleRightEvent();
            return true;
        }
        case SDLK_HOME: {
            handleHomeEvent();
            return true;
        }
        case SDLK_END: {
            handleEndEvent();
            return true;
        }
    }

    return false;
}

bool TextBox::onTextInput(SDL_TextInputEvent& event)
{
    // Append the user's new character to the end of the text.
    text.insertText(event.text, cursorIndex);

    // Move the cursor forwards.
    cursorIndex++;

    // Make the cursor visible and reset the blink time so it stays solid
    // while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return true;
}

void TextBox::onTick()
{
    // If we're selected, blink the cursor.
    if (currentState == State::Selected) {
        // Accumulate the time passed since last tick().
        accumulatedBlinkTime += Screen::TICK_TIMESTEP_S;

        // If enough time has passed.
        if (accumulatedBlinkTime > CURSOR_BLINK_RATE_S) {
            // Toggle the cursor draw state.
            if (!cursorIsVisible) {
                cursorIsVisible = true;
            }
            else {
                cursorIsVisible = false;
            }

            accumulatedBlinkTime -= CURSOR_BLINK_RATE_S;
        }
    }
}

void TextBox::render(const SDL_Point& parentOffset)
{
    // Keep our extent up to date.
    refreshScaling();

    // Children should render at the parent's offset + this component's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += scaledExtent.x;
    childOffset.y += scaledExtent.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = scaledExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render the appropriate background image for our current state.
    renderAppropriateImage(childOffset);

    // Determine which side the margin should apply to, based on alignment.
    int alignedMargin = getAlignedMargin();

    // Render the text, adding the margin.
    text.render({childOffset.x + alignedMargin, childOffset.y});

    // Render the text cursor, if necessary.
    if (cursorIsVisible) {
        renderTextCursor(childOffset, alignedMargin);
    }
}

void TextBox::handleBackspaceEvent()
{
    // If there's any text, delete the last character.
    if (text.eraseCharacter(cursorIndex - 1)) {
        // If we deleted a character, move the cursor backwards.
        cursorIndex--;

        // Make the cursor visible and reset the blink time so it stays
        // solid while interacting.
        cursorIsVisible = true;
        accumulatedBlinkTime = 0;
    }
}

void TextBox::handleDeleteEvent()
{
    // If there's a character after the cursor, delete it.
    if (text.eraseCharacter(cursorIndex)) {
        // Make the cursor visible and reset the blink time so it stays
        // solid while interacting.
        cursorIsVisible = true;
        accumulatedBlinkTime = 0;
    }
}

void TextBox::handleCopyEvent()
{
    // If there's text to copy, copy it to the clipboard.
    const std::string& textString = text.asString();
    if (textString.length() > 0) {
        SDL_SetClipboardText(textString.c_str());
    }
}

void TextBox::handleCutEvent()
{
    // If there's text to copy, copy it to the clipboard.
    const std::string& textString = text.asString();
    if (textString.length() > 0) {
        SDL_SetClipboardText(textString.c_str());
    }

    // Empty the text.
    text.setText("");

    // Move the cursor to the start.
    cursorIndex = 0;
}

void TextBox::handlePasteEvent()
{
    // Paste the text from the clipboard to the cursor index.
    char* clipboardText = SDL_GetClipboardText();
    text.insertText(clipboardText, cursorIndex);

    // Move the cursor to the end of the inserted text.
    cursorIndex += std::strlen(clipboardText);

    SDL_free(clipboardText);
}

void TextBox::handleLeftEvent()
{
    // If we can, move the cursor left.
    if (cursorIndex > 0) {
        cursorIndex--;
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;
}

void TextBox::handleRightEvent()
{
    // If we can, move the cursor right.
    if (cursorIndex < text.asString().length()) {
        cursorIndex++;
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;
}

void TextBox::handleHomeEvent()
{
    // Move the cursor to the front.
    cursorIndex = 0;
}

void TextBox::handleEndEvent()
{
    // Move the cursor to the end.
    cursorIndex = text.asString().length();
}

void TextBox::renderAppropriateImage(const SDL_Point& childOffset)
{
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
}

int TextBox::getAlignedMargin()
{
    // Determine which direction the margin should move the text.
    switch (text.getHorizontalAlignment()) {
        case Text::HorizontalAlignment::Left:
        case Text::HorizontalAlignment::Middle: {
            // Apply to the left side.
            return margin;
        }
        case Text::HorizontalAlignment::Right: {
            // Apply to the right side.
            return -margin;
        }
        default: {
            // Shouldn't reach here.
            return 0;
        }
    }
}

void TextBox::renderTextCursor(const SDL_Point& childOffset, int alignedMargin)
{
    // Save the current draw color to re-apply later.
    SDL_Color originalColor;
    SDL_GetRenderDrawColor(Core::GetRenderer(), &originalColor.r
                           , &originalColor.g, &originalColor.b, &originalColor.a);

    // Calc where the cursor should be.
    SDL_Rect offsetCursorExtent{text.calcCharacterOffset(cursorIndex)};
    offsetCursorExtent.x += childOffset.x + alignedMargin;
    offsetCursorExtent.y += childOffset.y;
    offsetCursorExtent.w = cursorWidth;

    // Draw the cursor.
    SDL_SetRenderDrawColor(Core::GetRenderer()
    , cursorColor.r, cursorColor.g, cursorColor.b, cursorColor.a);

    SDL_RenderFillRect(Core::GetRenderer(), &offsetCursorExtent);

    // Re-apply the original draw color.
    SDL_SetRenderDrawColor(Core::GetRenderer(), originalColor.r
                           , originalColor.g, originalColor.b, originalColor.a);
}

} // namespace AUI
