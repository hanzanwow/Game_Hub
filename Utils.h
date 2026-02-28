#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdlib>

// Changing terminal text colors.
namespace Utils
{
    struct Color
    {
        // Static inline constants allow these to be used without instantiating the struct.
        inline static constexpr const char *CYAN = "\033[36m";
        inline static constexpr const char *BLUE = "\033[34m";
        inline static constexpr const char *MAGENTA = "\033[35m";
        inline static constexpr const char *BOLD_MAGENTA = "\033[1;35m"; // Bold and Magenta
        inline static constexpr const char *RESET = "\033[0m";           // Resets color to default
        inline static constexpr const char *GREEN = "\033[32m";
        inline static constexpr const char *RED = "\033[31m";
        inline static constexpr const char *YELLOW = "\033[33m";
    };
    // Color::RED
}
#endif