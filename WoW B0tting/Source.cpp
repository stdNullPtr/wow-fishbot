#include <windows.h>
#include <iostream>
#include <ctime>
#include "Utils.h"
#include "Hooks.h"
#include "Timer.h"

void waitAndListenForFish();
void peekMessageLoop();
void clearMouseChangeMessages();
void findBobberSpirral();

int main()
{
    // set foreground window to wow and add checks if we are there TODO
    // after throwing, wait a second and start listening TODO
    // wow sound to 80%, device sound to max
    for (size_t timer = 5; timer > 0; timer--)
    {
        system("cls");
        printf_s(XorStr("Starting in %d seconds, open wow window!\n"), timer);
        Sleep(1 * 1000); // 1second
    }

    Hooks::getInstance().installHooks();

    const RECT& rect = g::wowWindowRect;
    const int centerX{ rect.left + (rect.right - rect.left) / 2 };
    const int centerY{ rect.top + (rect.bottom - rect.top) / 2 };
    // move a bit above the center since the bobber will be somewhere in that location (above center)
    constexpr int moveUpDist{ 250 };

    Timer t;

    while (!GetAsyncKeyState(VK_INSERT))
    {
        // Hold alt and set foreground, it will work hacky
        /*if (!SetForegroundWindow(gHWnd))
        {
            printf(XorStr("Can't set foreground!\n"));
            system(XorStr("pause"));
            CleanupAndExit();
        }
        WaitAction();*/

        if (t.elapsed() >= 30.0 * 60.0) // 30 minutes
        {
            printf_s(XorStr("Applying lure...\n"));
            Utils::pressKey('2'); // lure
            Sleep(5 * 1000);
            t.reset();
        }

        printf_s(XorStr("Total Splashes: %ld\n"), g::splashesCnt);
        printf_s(XorStr("Total Failed splashes: %ld\n"), g::didntHearSplashCnt);

        Utils::moveMouseSmooth(centerX, centerY - moveUpDist);

        Utils::pressKey('1');
        Sleep(500); // wait after casting (bobber land)

        findBobberSpirral();
    }

    Hooks::getInstance().uninstallHooks();

    return 0;
}

void waitAndListenForFish()
{
    time_t startTime{ time(nullptr) };
    static constexpr time_t totalFishingTime{ 23 }; // end loop after this time has elapsed (fishing time is 22.0)
    const time_t endTime{ startTime + totalFishingTime };

    float peak{ };
    while (true)
    {
        if (startTime >= endTime)
        {
            g::didntHearSplashCnt++;
            break;
        }

        // Get peak audio level
        Hooks::getInstance().getAudioPeakLevel(&peak);
        if (peak > 0.1f)
        {
            printf_s(XorStr("SPLASH -> sound level: %f\n"), peak);
            // wait for bobber to stop moving
            //Sleep(50);

            Utils::pressRightMouseButton();
            g::splashesCnt++;
            // wait for loot
            Sleep(1 * 1000);
            break;
        }

        // get new time for loop timer
        startTime = time(nullptr);
        // slow down loop
        Utils::waitAction();
    }
}

// Looking for bobber. particularly mouse change message.
void peekMessageLoop()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&msg);
    }
}

// Get rid of excess mouse change messages
void clearMouseChangeMessages()
{
    peekMessageLoop();
    g::isBobberFound = false;
}

void findBobberSpirral()
{
    static constexpr int moves{ 800 };
    static constexpr float angleCoef{ 0.1f };

    int moveX{}, moveY{};
    float angle{ 0.0f };

    POINT cursor;
    GetCursorPos(&cursor);

    clearMouseChangeMessages();
    // begin spiral! 
    for (int step{ 0 }; step < moves; step++)
    {
        angle = angleCoef * step;
        moveX = static_cast<int>((step * cos(angle)) + cursor.x);
        moveY = static_cast<int>((step * sin(angle)) + cursor.y);

        SetCursorPos(moveX, moveY);
        peekMessageLoop();

        if (g::isBobberFound)
        {
            waitAndListenForFish();
            break;
        }

        Utils::waitAction();
    }
}

//void Log()
//{
//    CURSORINFO ci = { sizeof(ci) };
//    GetCursorInfo(&ci);
//    printf(XorStr("showing = %d, suppressed = %d, pos = (%d, %d), handle = %p, Bobber found: %s\n"),
//        !!(ci.flags & CURSOR_SHOWING),
//        !!(ci.flags & CURSOR_SUPPRESSED),
//        ci.ptScreenPos.x,
//        ci.ptScreenPos.y,
//        ci.hCursor,
//        BobberFound ? "true" : "false");
//}