#pragma once
#include <Windows.h>
#include <ctime>
#include <chrono>

class Utils
{
public:
    Utils() = delete;

    // wait between actions (lag, delay etc.)
    static inline void waitAction()
    {
        Sleep(10);
    }

    static void pressKey(const BYTE keyCode)
    {
        // This structure will be used to create the keyboard
        // input event.
        INPUT input{};
        // Set up a generic keyboard event.
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = 0; // hardware scan code for key
        input.ki.time = 0;
        input.ki.dwExtraInfo = NULL;

        // Press the key
        input.ki.wVk = keyCode; // virtual-key code for the key
        input.ki.dwFlags = 0; // 0 for key press
        (void)SendInput(1, &input, sizeof(input));
        input.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release

        (void)SendInput(1, &input, sizeof(input));
    }

    static void pressRightMouseButton()
    {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dx = 0;
        input.mi.dy = 0;
        input.mi.dwFlags = (MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP);
        input.mi.mouseData = 0;
        input.mi.dwExtraInfo = NULL;
        input.mi.time = 0;

        (void)SendInput(1, &input, sizeof(input));
    }

    static void moveMouseSmooth(const int& endX, const int& endY)
    {
        constexpr int stepCnt{ 100 };
        int moveX{}, moveY{};

        POINT cursor{};
        (void)GetCursorPos(&cursor);

        for (int step{ 1 }; step <= stepCnt; step++)
        {
            moveX = cursor.x + (endX - cursor.x) * step / stepCnt;
            moveY = cursor.y + (endY - cursor.y) * step / stepCnt;
            SetCursorPos(moveX, moveY);
            waitAction();
        }
    }

    /**
    *   GetCurrentSystemTime - Gets actual system time
    *   @timeInfo: Reference to your own tm variable, gets modified.
    */
    static void getCurrentSystemTime(tm& timeInfo)
    {
        const std::chrono::system_clock::time_point systemNow{ std::chrono::system_clock::now() };
        std::time_t now_c{ std::chrono::system_clock::to_time_t(systemNow) };
        (void)localtime_s(&timeInfo, &now_c); // using localtime_s as std::localtime is not thread-safe.
    };
};