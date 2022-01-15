#pragma once
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include "XorCompileTime.h"
#include "Globals.h"

//#pragma comment(lib, "winmm.lib")

#define SAFE_RELEASE(obj)  \
              if ((obj) != nullptr)  \
                { (obj)->Release(); (obj) = nullptr; }

#define EXIT_ON_ERROR(hr) if (FAILED(hr)) { uninstallHooks(); }

class Hooks
{
public:
    //singleton
    static Hooks& getInstance()
    {
        static Hooks instance;    // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }

    void installHooks(); // function to install our hook
    void uninstallHooks(); // function to uninstall our hook
    void getAudioPeakLevel(float* peak);

private:
    Hooks() = default;
    Hooks(const Hooks&) = delete;              // Don't Implement
    void operator=(const Hooks&) = delete;     // Don't implement
    // Note: Scott Meyers mentions in his Effective Modern
    // C++ book, that deleted functions should generally
    // be public as it results in better error messages
    // due to the compilers behavior to check accessibility
    // before deleted status

    HWINEVENTHOOK m_winEventHook{ nullptr };
    HWND m_wowHWND{ nullptr };
    DWORD m_wowProcId{};

    // audio
    IMMDeviceEnumerator* m_pEnumerator{ nullptr };
    IMMDevice* m_pDevice{ nullptr };
    IAudioMeterInformation* m_pMeterInfo{ nullptr };
};

void CALLBACK MouseCallback(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD time);