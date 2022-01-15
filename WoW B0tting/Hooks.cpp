#include "Hooks.h"

void Hooks::installHooks()
{
    ////// audio
    HRESULT hr{ CoInitialize(nullptr) };
    EXIT_ON_ERROR(hr)

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
            nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            (void**)&m_pEnumerator);
    EXIT_ON_ERROR(hr)

        hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pDevice);
    EXIT_ON_ERROR(hr)

        hr = m_pDevice->Activate(__uuidof(IAudioMeterInformation),
            CLSCTX_ALL, nullptr, (void**)&m_pMeterInfo);
    EXIT_ON_ERROR(hr)

        m_wowHWND = FindWindow(nullptr, XorStr("World of Warcraft"));
    if (m_wowHWND == 0)
    {
        printf_s(XorStr("Wow not running!\n"));
        printf_s(XorStr("Error: %x \n"), GetLastError());
        system(XorStr("pause"));
        uninstallHooks();
    }

    (void)GetWindowThreadProcessId(m_wowHWND, &m_wowProcId);

    m_winEventHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_HIDE,
        nullptr,
        MouseCallback,
        m_wowProcId,
        0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (m_winEventHook == 0)
    {
        printf_s(XorStr("Wow not running!\n"));
        printf_s(XorStr("Error: %x \n"), GetLastError());
        system(XorStr("pause"));
        uninstallHooks();
    }

    if (GetWindowRect(m_wowHWND, &g::wowWindowRect) == 0)
    {
        printf_s(XorStr("Can't get window size!\n"));
        printf_s(XorStr("Error: %x \n"), GetLastError());
        system(XorStr("pause"));
        uninstallHooks();
    }
}

void Hooks::uninstallHooks()
{
    UnhookWinEvent(m_winEventHook);

    SAFE_RELEASE(m_pEnumerator)
        SAFE_RELEASE(m_pDevice)
        SAFE_RELEASE(m_pMeterInfo)
        CoUninitialize();
}

void Hooks::getAudioPeakLevel(float* peak)
{
    EXIT_ON_ERROR(m_pMeterInfo->GetPeakValue(peak))
}

void CALLBACK MouseCallback(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD time)
{
    if (hwnd == nullptr &&
        idObject == OBJID_CURSOR &&
        idChild == CHILDID_SELF) {
        switch (event) {
        case EVENT_OBJECT_HIDE:
            printf_s(XorStr("cursor hidden\n"));
            break;
        case EVENT_OBJECT_SHOW:
            g::isBobberFound = true;
            printf_s(XorStr("cursor shown\n"));
            break;
            /*case EVENT_OBJECT_NAMECHANGE:
                BobberFound = true;
                printf(XorStr("Cursor changed\n"));
                break;

            case EVENT_OBJECT_LOCATIONCHANGE: break;

            default:
                printf(XorStr("Event %0x\n"), event);
                break;*/
        }
    }
}