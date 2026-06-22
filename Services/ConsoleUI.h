#pragma once

#include <string>

namespace ConsoleUI
{
    enum class Color
    {
        Default,
        Green,
        Red,
        Yellow,
        Cyan
    };

    void setColor(Color color);
    void reset();
    void printHeader(const std::string& title);
    void printStartupBanner();
    void showLoading(const std::string& message);
    void printSuccess(const std::string& message);
    void printWarning(const std::string& message);
    void printError(const std::string& message);
}
