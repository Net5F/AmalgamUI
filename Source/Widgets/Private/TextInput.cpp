#include "AUI/TextInput.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Ignore.h"
#include <cstring>

namespace AUI
{
int TextInput::focusedInputCount = 0;

TextInput::TextInput(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, normalImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, focusedImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, disabledImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Normal}
, cursorColor{0, 0, 0, 255}
, logicalCursorWidth{2}
, scaledCursorWidth{ScalingHelpers::logicalToActual(logicalCursorWidth)}
, cursorIndex{0}
, cursorIsVisible{false}
, text(screen, {0, 0, logicalExtent.w, logicalExtent.h})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(normalImage);
    children.push_back(hoveredImage);
    children.push_back(focusedImage);
    children.push_back(disabledImage);
    children.push_back(text);

    // Flag ourselves as keyboard focusable, so we can receive keyboard events.
    isFocusable = true;

    // Default to left-justifying the text within the button. The user can set
    // it otherwise if they care to.
    text.setVerticalAlignment(AUI::Text::VerticalAlignment::Center);
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Left);

    // Set the underlying text to empty string since it defaults to something
    // else.
    text.setText("");

    // Make the backgrounds we aren't using invisible.
    hoveredImage.setIsVisible(false);
    focusedImage.setIsVisible(false);
    disabledImage.setIsVisible(false);
}

void TextInput::setMargins(Margins inLogicalMargins)
{
    // Set the text widget to be the size of this widget, minus the
    // margins.
    text.setLogicalExtent(
        {inLogicalMargins.left, inLogicalMargins.top,
         (logicalExtent.w - inLogicalMargins.left - inLogicalMargins.right),
         (logicalExtent.h - inLogicalMargins.top - inLogicalMargins.bottom)});

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();
}

void TextInput::setCursorColor(const SDL_Color& inCursorColor)
{
    cursorColor = inCursorColor;
}

void TextInput::setCursorWidth(unsigned int inCursorWidth)
{
    logicalCursorWidth = inCursorWidth;
    scaledCursorWidth = ScalingHelpers::logicalToActual(logicalCursorWidth);
}

TextInput::State TextInput::getCurrentState()
{
    return currentState;
}

void TextInput::setTextFont(const std::string& relPath, int size)
{
    text.setFont(relPath, size);
}

void TextInput::setTextColor(const SDL_Color& inColor)
{
    text.setColor(inColor);
}

void TextInput::setText(std::string_view inText)
{
    // Set the text member's text.
    text.setText(inText);

    // Move the cursor to the back (seems to be the most expected behavior.)
    cursorIndex = text.asString().length();

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();
}

const std::string& TextInput::getText()
{
    return text.asString();
}

void TextInput::setOnTextChanged(std::function<void(void)> inOnTextChanged)
{
    onTextChanged = std::move(inOnTextChanged);
}

void TextInput::setOnTextCommitted(std::function<void(void)> inOnTextCommitted)
{
    onTextCommitted = std::move(inOnTextCommitted);
}

EventResult TextInput::onMouseDown(MouseButtonType buttonType, const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);

    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're disabled, ignore the event.
    else if (currentState == State::Disabled) {
        return EventResult{.wasHandled{false}};
    }

    // Note: Since we're handling a MouseDown, we'll be given focus.
    return EventResult{.wasHandled{true}};
}

void TextInput::onMouseEnter()
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If we're normal, change to hovered.
    if (currentState == State::Normal) {
        setCurrentState(State::Hovered);
    }
}

void TextInput::onMouseLeave()
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If we're hovered, unhover.
    if (currentState == State::Hovered) {
        setCurrentState(State::Normal);
    }
}

EventResult TextInput::onFocusGained()
{
    AUI_LOG_INFO("TextInput onFocusGained()");
    // Set our state to focused.
    setCurrentState(State::Focused);
    focusedInputCount++;

    // If there wasn't already a focused input, begin generating text input
    // events.
    if (focusedInputCount == 1) {
        SDL_StartTextInput();
    }

    // Reset the text cursor's state.
    // Show the text cursor immediately so the user can see where they're at.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    // Move the cursor to the end.
    cursorIndex = text.asString().length();

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    return EventResult{.wasHandled{true}};
}

void TextInput::onFocusLost()
{
    // Set our state back to normal.
    setCurrentState(State::Normal);
    focusedInputCount--;

    // If there are no other focused inputs, stop generating text input events.
    if (focusedInputCount == 0) {
        SDL_StopTextInput();
    }

    // Reset the text cursor's state.
    cursorIsVisible = false;

    // If a callback is registered, signal that the the text was committed.
    if (onTextCommitted) {
        onTextCommitted();
    }
}

//Widget* TextInput::onKeyDown(SDL_KeyboardEvent& event)
//{
//    // If we don't have focus, ignore the event.
//    if (currentState != State::Focused) {
//        return nullptr;
//    }
//
//    switch (event.keysym.sym) {
//        case SDLK_BACKSPACE: {
//            return handleBackspaceEvent();
//        }
//        case SDLK_DELETE: {
//            return handleDeleteEvent();
//        }
//        case SDLK_c: {
//            return handleCopyEvent();
//        }
//        case SDLK_x: {
//            return handleCutEvent();
//        }
//        case SDLK_v: {
//            return handlePasteEvent();
//        }
//        case SDLK_LEFT: {
//            return handleLeftEvent();
//        }
//        case SDLK_RIGHT: {
//            return handleRightEvent();
//        }
//        case SDLK_HOME: {
//            return handleHomeEvent();
//        }
//        case SDLK_END: {
//            return handleEndEvent();
//        }
//        case SDLK_RETURN: {
//            return handleEnterEvent();
//        }
//    }
//
//    return nullptr;
//}
//
//Widget* TextInput::onTextInput(SDL_TextInputEvent& event)
//{
//    // If we don't have focus, ignore the event.
//    if (currentState != State::Focused) {
//        return nullptr;
//    }
//
//    // Append the user's new character to the end of the text.
//    text.insertText(event.text, cursorIndex);
//
//    // Move the cursor forwards.
//    cursorIndex++;
//
//    // Refresh the text position to account for the change.
//    refreshTextScrollOffset();
//
//    // Make the cursor visible and reset the blink time so it stays solid
//    // while interacting.
//    cursorIsVisible = true;
//    accumulatedBlinkTime = 0;
//
//    return this;
//}

void TextInput::onTick(double timestepS)
{
    // If we have focus, blink the cursor.
    if (currentState == State::Focused) {
        // Accumulate the time passed since last tick().
        accumulatedBlinkTime += timestepS;

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

void TextInput::render()
{
    // Render our child widgets.
    Widget::render();

    // Render the text cursor, if necessary.
    if (cursorIsVisible) {
        renderTextCursor();
    }
}

bool TextInput::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Widget::refreshScaling()) {
        // Refresh our cursor size.
        scaledCursorWidth = ScalingHelpers::logicalToActual(logicalCursorWidth);

        return true;
    }

    return false;
}

Widget* TextInput::handleBackspaceEvent()
{
    // If there's any text, delete the last character.
    if (text.eraseCharacter(cursorIndex - 1)) {
        // We deleted a character, move the cursor backwards.
        cursorIndex--;

        // Make the cursor visible and reset the blink time so it stays
        // solid while interacting.
        cursorIsVisible = true;
        accumulatedBlinkTime = 0;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();

        // If a callback is registered, signal that the text was changed.
        if (onTextChanged) {
            onTextChanged();
        }

        return this;
    }

    return nullptr;
}

Widget* TextInput::handleDeleteEvent()
{
    // If there's a character after the cursor, delete it.
    if (text.eraseCharacter(cursorIndex)) {
        // Make the cursor visible and reset the blink time so it stays
        // solid while interacting.
        cursorIsVisible = true;
        accumulatedBlinkTime = 0;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();

        // If a callback is registered, signal that the the text was changed.
        if (onTextChanged) {
            onTextChanged();
        }

        return this;
    }

    return nullptr;
}

Widget* TextInput::handleCopyEvent()
{
    // If this was a CTRL+c copy command.
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text to copy.
        const std::string& textString = text.asString();
        if (textString.length() > 0) {
            // Copy the text to the clipboard.
            SDL_SetClipboardText(textString.c_str());
        }

        return this;
    }

    return nullptr;
}

Widget* TextInput::handleCutEvent()
{
    // If this was a CTRL+x cut command
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text to cut.
        const std::string& textString = text.asString();
        if (textString.length() > 0) {
            // Copy the text to the clipboard.
            SDL_SetClipboardText(textString.c_str());

            // Clear the text.
            text.setText("");

            // Move the cursor to the start.
            cursorIndex = 0;

            // Refresh the text position to account for the change.
            refreshTextScrollOffset();

            // If a callback is registered, signal that the the text was
            // changed.
            if (onTextChanged) {
                onTextChanged();
            }
        }

        return this;
    }

    return nullptr;
}

Widget* TextInput::handlePasteEvent()
{
    // If this was a CTRL+v paste command.
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text in the clipboard.
        if (SDL_HasClipboardText()) {
            // Paste the text from the clipboard to the cursor index.
            char* clipboardText = SDL_GetClipboardText();
            text.insertText(clipboardText, cursorIndex);

            // Move the cursor to the end of the inserted text.
            cursorIndex += std::strlen(clipboardText);

            SDL_free(clipboardText);

            // Refresh the text position to account for the change.
            refreshTextScrollOffset();

            // If a callback is registered, signal that the the text was
            // changed.
            if (onTextChanged) {
                onTextChanged();
            }
        }

        return this;
    }

    return nullptr;
}

Widget* TextInput::handleLeftEvent()
{
    // If we can, move the cursor left.
    bool movedCursor{false};
    if (cursorIndex > 0) {
        cursorIndex--;
        movedCursor = true;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return (movedCursor ? this : nullptr);
}

Widget* TextInput::handleRightEvent()
{
    // If we can, move the cursor right.
    bool movedCursor{false};
    if (cursorIndex < text.asString().length()) {
        cursorIndex++;
        movedCursor = true;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return (movedCursor ? this : nullptr);
}

Widget* TextInput::handleHomeEvent()
{
    // Move the cursor to the front.
    cursorIndex = 0;

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    return this;
}

Widget* TextInput::handleEndEvent()
{
    // Move the cursor to the end.
    cursorIndex = text.asString().length();

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    return this;
}

Widget* TextInput::handleEnterEvent()
{
    // If we have focus, remove it.
    if (currentState == State::Focused) {
//        removeFocus();
    }

    return this;
}

void TextInput::setCurrentState(State inState)
{
    // Set the new state.
    currentState = inState;

    // Make the associated background visible and make the rest invisible.
    normalImage.setIsVisible(false);
    hoveredImage.setIsVisible(false);
    focusedImage.setIsVisible(false);
    disabledImage.setIsVisible(false);

    switch (currentState) {
        case State::Normal: {
            normalImage.setIsVisible(true);
            break;
        }
        case State::Hovered: {
            hoveredImage.setIsVisible(true);
            break;
        }
        case State::Focused: {
            focusedImage.setIsVisible(true);
            break;
        }
        case State::Disabled: {
            disabledImage.setIsVisible(true);
            break;
        }
    }
}

void TextInput::refreshTextScrollOffset()
{
    // Get the distance from the start of the string to the cursor position.
    // Note: This position is relative to text's scaledExtent.
    SDL_Rect cursorOffsetExtent{text.calcCharacterOffset(cursorIndex)};

    // If the text isn't scrolled properly, fix it.
    SDL_Rect textExtent{text.getScaledExtent()};
    int cursorX{cursorOffsetExtent.x};
    int textOffset{text.getTextOffset()};
    if (cursorX < textExtent.x) {
        // Cursor is past the left bound, scroll right.
        textOffset += (textExtent.x - cursorX);
    }
    else if (cursorX > (textExtent.x + textExtent.w)) {
        // Cursor is past the right bound, scroll left.
        textOffset -= (cursorX - (textExtent.x + textExtent.w));
    }
    else if (textOffset < 0) {
        // There's text hanging off the left side. Are we still pushed against
        // the right bound? (Relevant after a backspace.)
        SDL_Rect lastCharOffset{text.calcCharacterOffset(text.asString().length())};
        if (lastCharOffset.x < (textExtent.x + textExtent.w)) {
            // There's a gap to fill, scroll right.
            textOffset += ((textExtent.x + textExtent.w) - lastCharOffset.x);

            // Don't let the text move farther than the left bound.
            if (textOffset > 0) {
                textOffset = 0;
            }
        }
    }

    // Set the new offset.
    text.setTextOffset(textOffset);
}

void TextInput::renderTextCursor()
{
    // Save the current draw color to re-apply later.
    SDL_Color originalColor;
    SDL_GetRenderDrawColor(Core::getRenderer(), &originalColor.r,
                           &originalColor.g, &originalColor.b,
                           &originalColor.a);

    // Calc where the cursor should be.
    SDL_Rect cursorOffsetExtent{text.calcCharacterOffset(cursorIndex)};
    cursorOffsetExtent.x += renderExtent.x;
    cursorOffsetExtent.y += renderExtent.y;
    cursorOffsetExtent.w = scaledCursorWidth;

    // Draw the cursor.
    SDL_SetRenderDrawColor(Core::getRenderer(), cursorColor.r, cursorColor.g,
                           cursorColor.b, cursorColor.a);

    SDL_RenderFillRect(Core::getRenderer(), &cursorOffsetExtent);

    // Re-apply the original draw color.
    SDL_SetRenderDrawColor(Core::getRenderer(), originalColor.r,
                           originalColor.g, originalColor.b, originalColor.a);
}

} // namespace AUI
