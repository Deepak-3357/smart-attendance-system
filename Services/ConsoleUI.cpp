#include "ConsoleUI.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <windows.h>

namespace
{
    WORD readCurrentAttributes()
    {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(handle, &info))
        {
            return info.wAttributes;
        }

        return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    WORD defaultAttributes()
    {
        static const WORD attributes = readCurrentAttributes();
        return attributes;
    }
}

void ConsoleUI::setColor(Color color)
{
    const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD attributes = defaultAttributes();

    switch (color)
    {
    case Color::Green:
        attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case Color::Red:
        attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case Color::Yellow:
        attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case Color::Cyan:
        attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    default:
        break;
    }

    SetConsoleTextAttribute(handle, attributes);
}

void ConsoleUI::reset()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), defaultAttributes());
}

void ConsoleUI::printHeader(const std::string& title)
{
    setColor(Color::Cyan);
    std::cout << "\n=================================\n";
    std::cout << title << '\n';
    std::cout << "=================================\n";
    reset();
}

void ConsoleUI::printStartupBanner()
{
    setColor(Color::Cyan);
    std::cout << "\n=========================================\n";
    std::cout << "SMART ATTENDANCE MANAGEMENT SYSTEM\n";
    std::cout << "AI POWERED FACE RECOGNITION\n";
    std::cout << "OpenCV + LBPH + C++\n";
    std::cout << "=========================================\n";
    reset();
}

void ConsoleUI::showLoading(const std::string& message)
{
    std::cout << message;
    for (int i = 0; i < 3; ++i)
    {
        std::cout << '.';
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(180));
    }
    std::cout << "\n";
}

void ConsoleUI::printSuccess(const std::string& message)
{
    setColor(Color::Green);
    std::cout << message << '\n';
    reset();
}

void ConsoleUI::printWarning(const std::string& message)
{
    setColor(Color::Yellow);
    std::cout << message << '\n';
    reset();
}

void ConsoleUI::printError(const std::string& message)
{
    setColor(Color::Red);
    std::cout << message << '\n';
    reset();
}
