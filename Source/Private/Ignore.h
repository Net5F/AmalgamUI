#pragma once

namespace AUI
{

/**
 * Provides a function to suppress "unused parameter" warnings when the
 * unuse is intentional. (Most commonly, when it's a temporary implementation.)
 */
template<class T>
void ignore(const T&)
{
}

} // namespace AUI
