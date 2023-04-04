#include "AUI/TextInput.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Ignore.h"
#include <cstring>

namespace AUI
{
TextInput::TextInput(const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, normalImage({0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage({0, 0, logicalExtent.w, logicalExtent.h})
, focusedImage({0, 0, logicalExtent.w, logicalExtent.h})
, disabledImage({0, 0, logicalExtent.w, logicalExtent.h})
, accumulatedBlinkTime{0}
, currentState{State::Normal}
, cursorColor{0, 0, 0, 255}
, logicalCursorWidth{2}
, scaledCursorWidth{ScalingHelpers::logicalToActual(logicalCursorWidth)}
, cursorIndex{0}
, cursorIsVisible{false}
, lastCommittedText{""}
, text({0, 0, logicalExtent.w, logicalExtent.h})
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

void TextInput::enable()
{
    SDL_Point cursorPosition{};
    SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));

    // Check if we're currently hovered.
    if (containsPoint(cursorPosition)) {
        setCurrentState(State::Hovered);
    }
    else {
        setCurrentState(State::Normal);
    }
}

void TextInput::disable()
{
    setCurrentState(State::Disabled);
    cursorIsVisible = false;

    // Scroll back to the front (the most expected behavior).
    cursorIndex = 0;

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();
}

void TextInput::setPadding(Padding inLogicalPadding)
{
    // Set the text widget to be the size of this widget, minus the
    // margins.
    text.setLogicalExtent(
        {inLogicalPadding.left, inLogicalPadding.top,
         (logicalExtent.w - inLogicalPadding.left - inLogicalPadding.right),
         (logicalExtent.h - inLogicalPadding.top - inLogicalPadding.bottom)});

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

void TextInput::setTextFont(const std::string& fontPath, int size)
{
    text.setFont(fontPath, size);
}

void TextInput::setTextColor(const SDL_Color& inColor)
{
    text.setColor(inColor);
}

void TextInput::setText(std::string_view inText)
{
    // Set the committed text.
    lastCommittedText = inText;
    text.setText(inText);

    // Move the cursor to the front (seems to be the most expected behavior).
    cursorIndex = 0;

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

EventResult TextInput::onMouseDown(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
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

EventResult TextInput::onMouseDoubleClick(MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
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
    // Set our state to focused.
    setCurrentState(State::Focused);

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

void TextInput::onFocusLost(FocusLostType focusLostType)
{
    // If we were disabled after being focused, do nothing.
    if (currentState == State::Disabled) {
        return;
    }

    // Set our state back to normal.
    setCurrentState(State::Normal);

    // Reset the text cursor's state.
    cursorIsVisible = false;

    // If we lost focus because of an Escape key press, revert to the last
    // committed text state.
    if (focusLostType == FocusLostType::Escape) {
        // Note: Moves the cursor to the front.
        setText(lastCommittedText);
    }
    else {
        // We lost focus for some other reason, commit the current text.
        lastCommittedText = text.asString();

        // Scroll back to the front (the most expected behavior).
        cursorIndex = 0;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();

        // If a callback is registered, signal that the text was committed.
        if (onTextCommitted) {
            onTextCommitted();
        }
    }
}

EventResult TextInput::onKeyDown(SDL_Keycode keyCode)
{
    EventResult eventResult{};
    switch (keyCode) {
        case SDLK_BACKSPACE: {
            eventResult = handleBackspaceEvent();
            break;
        }
        case SDLK_DELETE: {
            eventResult = handleDeleteEvent();
            break;
        }
        case SDLK_c: {
            eventResult = handleCopyEvent();
            break;
        }
        case SDLK_x: {
            eventResult = handleCutEvent();
            break;
        }
        case SDLK_v: {
            eventResult = handlePasteEvent();
            break;
        }
        case SDLK_LEFT: {
            eventResult = handleLeftEvent();
            break;
        }
        case SDLK_RIGHT: {
            eventResult = handleRightEvent();
            break;
        }
        case SDLK_HOME: {
            eventResult = handleHomeEvent();
            break;
        }
        case SDLK_END: {
            eventResult = handleEndEvent();
            break;
        }
        case SDLK_RETURN: {
            eventResult = handleEnterEvent();
            break;
        }
        case SDLK_ESCAPE: {
            // We let Escape key events go through since the EventRouter might
            // use them to drop focus.
            eventResult.wasHandled = false;
            break;
        }
        default: {
            // We need to handle all other key events so that parent widgets
            // don't get them while we're working with the TextInput events.
            eventResult.wasHandled = true;
            break;
        }
    }

    return eventResult;
}

EventResult TextInput::onKeyUp(SDL_Keycode keyCode)
{
    ignore(keyCode);

    // We need to handle all key events so that parent widgets don't get them
    // while we're working with the TextInput events.
    return EventResult{.wasHandled{true}};
}

EventResult TextInput::onTextInput(const std::string& inputText)
{
    // Insert the user's new input text.
    text.insertText(inputText, cursorIndex);

    // Move the cursor forwards.
    cursorIndex += inputText.length();

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    // Make the cursor visible and reset the blink time so it stays solid
    // while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return EventResult{.wasHandled{true}};
}

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

    // Call every visible child's onTick().
    Widget::onTick(timestepS);
}

void TextInput::updateLayout(const SDL_Point& startPosition,
                             const SDL_Rect& availableExtent,
                             WidgetLocator* widgetLocator)
{
    // Do the normal layout updating.
    Widget::updateLayout(startPosition, availableExtent, widgetLocator);

    // Refresh our cursor size.
    scaledCursorWidth = ScalingHelpers::logicalToActual(logicalCursorWidth);
}

void TextInput::render(const SDL_Point& windowTopLeft)
{
    // Render our child widgets.
    Widget::render(windowTopLeft);

    // Render the text cursor, if necessary.
    if (cursorIsVisible) {
        renderTextCursor(windowTopLeft);
    }
}

EventResult TextInput::handleBackspaceEvent()
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
    }

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleDeleteEvent()
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
    }

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleCopyEvent()
{
    // If this was a CTRL+c copy command.
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text to copy.
        const std::string& textString{text.asString()};
        if (textString.length() > 0) {
            // Copy the text to the clipboard.
            SDL_SetClipboardText(textString.c_str());
        }
    }

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleCutEvent()
{
    // If this was a CTRL+x cut command
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text to cut.
        const std::string& textString{text.asString()};
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
    }

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handlePasteEvent()
{
    // If this was a CTRL+v paste command.
    if (SDL_GetModState() & KMOD_CTRL) {
        // If there's text in the clipboard.
        if (SDL_HasClipboardText()) {
            // Paste the text from the clipboard to the cursor index.
            char* clipboardText{SDL_GetClipboardText()};
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
    }

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleLeftEvent()
{
    // If we can, move the cursor left.
    if (cursorIndex > 0) {
        cursorIndex--;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleRightEvent()
{
    // If we can, move the cursor right.
    if (cursorIndex < text.asString().length()) {
        cursorIndex++;

        // Refresh the text position to account for the change.
        refreshTextScrollOffset();
    }

    // Make the cursor visible and reset the blink time so it stays
    // solid while interacting.
    cursorIsVisible = true;
    accumulatedBlinkTime = 0;

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleHomeEvent()
{
    // Move the cursor to the front.
    cursorIndex = 0;

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleEndEvent()
{
    // Move the cursor to the end.
    cursorIndex = text.asString().length();

    // Refresh the text position to account for the change.
    refreshTextScrollOffset();

    return EventResult{.wasHandled{true}};
}

EventResult TextInput::handleEnterEvent()
{
    // On Enter key press, we drop focus (which will call onFocusLost() to
    // set our internal state and call onTextCommitted()).
    return EventResult{.wasHandled{true}, .dropFocus{true}};
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
        SDL_Rect lastCharOffset{text.calcCharacterOffset(
            static_cast<unsigned int>(text.asString().length()))};
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

void TextInput::renderTextCursor(const SDL_Point& windowTopLeft)
{
    // Save the current draw color to re-apply later.
    SDL_Color originalColor{};
    SDL_GetRenderDrawColor(Core::getRenderer(), &originalColor.r,
                           &originalColor.g, &originalColor.b,
                           &originalColor.a);

    // Calc where the cursor should be.
    SDL_Rect cursorOffsetExtent{text.calcCharacterOffset(cursorIndex)};
    cursorOffsetExtent.x += clippedExtent.x + windowTopLeft.x;
    cursorOffsetExtent.y += clippedExtent.y + windowTopLeft.y;
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
