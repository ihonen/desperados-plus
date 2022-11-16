#include <windows.h>
#include <detours.h>

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>

// -----------------------------------------------------------------------------

#ifdef DP_DEV_BUILD
# define PRINT_DEBUG(f, ...) fprintf(stderr, "DEBUG: " f "\n", ##__VA_ARGS__)
# define PRINT_INFO(f, ...)  fprintf(stderr, "INFO:  " f "\n", ##__VA_ARGS__)
# define PRINT_WARN(f, ...)  fprintf(stderr, "WARN:  " f "\n", ##__VA_ARGS__)
# define PRINT_ERROR(f, ...) fprintf(stderr, "ERROR: " f "\n", ##__VA_ARGS__)
#else
# define PRINT_DEBUG(...)
# define PRINT_INFO(...)
# define PRINT_WARN(...)
# define PRINT_ERROR(...)
#endif

// -----------------------------------------------------------------------------

// NOTE: IF __cdecl THEN invert parameter list

// -----------------------------------------------------------------------------
// CONSOLE
// -----------------------------------------------------------------------------

// _DWORD __cdecl DVConsole::DVConsole(DVConsole* this)
typedef DWORD* (__thiscall* tDVConsole_Ctor)(PVOID);

// _DWORD __cdecl DVConsole::ExecuteCommand(DVConsole* this)
typedef DWORD (__thiscall* tDVConsole_ExecuteCommand)(PVOID);

// -----------------------------------------------------------------------------
// DRAW MANAGER
// -----------------------------------------------------------------------------

// _DWORD SBDrawManager::PrintConsole(SBDrawManager* __hidden this, char*, ...)
typedef char (__cdecl* tSBDrawManager_PrintConsole)(PCHAR, PVOID);

// _DWORD SBDrawManager::PrintConsole(SBDrawManager* __hidden this, unsigned, char*, ...)
typedef char (__cdecl* tSBDrawManager_PrintConsoleWithInt)(PCHAR, DWORD, PVOID);

// -----------------------------------------------------------------------------
// SCRIPTS
// -----------------------------------------------------------------------------

// _DWORD __cdecl DVScript::DVScript(DVScript* this)
typedef DWORD* (__thiscall* tDVScript_Ctor)(PVOID);

// int DVScript::This()
typedef int* (__stdcall* tDVScript_This)();

// _DWORD __cdecl DVScript::GetActor(DVScript* this, int)
typedef DWORD (__cdecl* tDVScript_GetActor)(int, PVOID);

// _DWORD DVScript::GetCooper(DVScript* __hidden this)
typedef DWORD* (__cdecl* tDVScript_GetCooper)(PVOID);

// _DWORD __cdecl DVScript::Activate(DVScript* this, void*)
typedef int (__cdecl* tDVScript_ActivatePC)(DWORD*, PVOID);

// _DWORD __cdecl DVScript::Deactivate(DVScript* this, void*)
typedef int (__cdecl* tDVScript_DeactivatePC)(DWORD*, PVOID);

// -----------------------------------------------------------------------------
// CHARACTERS
// -----------------------------------------------------------------------------

// DVDoc* __cdecl DVDoc::DVDoc(DVDoc* __hidden this)
typedef DWORD* (__thiscall* tDVDoc_Ctor)(PVOID);

// -----------------------------------------------------------------------------
// OTHER
// -----------------------------------------------------------------------------

// void __thiscall DVElementActorHuman::Translate(DVElementActorHuman* this, DVSequenceElement* param_1, DVcommand param_2)
typedef void (__thiscall* tDVElementActorHuman_Translate)(PVOID, DWORD*, DWORD);

// void __thiscall DVElementActorHuman::CheckIfViolationOfInternationalWar(DVElementActorHuman* this, DVElement* param_1, DVElement* param_2)
typedef int (__stdcall* tDVElementActorHuman_CheckIfViolationOfInternationalWar)(DWORD*, DWORD*);

// -----------------------------------------------------------------------------

static tDVElementActorHuman_CheckIfViolationOfInternationalWar DVElementActorHuman_CheckIfViolationOfInternationalWar;
static tDVElementActorHuman_Translate                          DVElementActorHuman_Translate;

static tDVDoc_Ctor                                             DVDoc_Ctor;

static tDVConsole_Ctor                                         DVConsole_Ctor;
static tDVConsole_ExecuteCommand                               DVConsole_ExecuteCommand;

static tSBDrawManager_PrintConsole                             SBDrawManager_PrintConsole;
static tSBDrawManager_PrintConsoleWithInt                      SBDrawManager_PrintConsoleWithInt;

static tDVScript_Ctor                                          DVScript_Ctor;
static tDVScript_This                                          DVScript_This;
static tDVScript_GetActor                                      DVScript_GetActor;
static tDVScript_GetCooper                                     DVScript_GetCooper;
static tDVScript_ActivatePC                                    DVScript_ActivatePC;
static tDVScript_DeactivatePC                                  DVScript_DeactivatePC;

// -----------------------------------------------------------------------------

static DWORD  s_gameBaseAddr   = 0x0;
static DWORD* s_thisDVScript  = nullptr;
static DWORD* s_thisDVConsole = nullptr;

// -----------------------------------------------------------------------------

DWORD* __fastcall DVConsole_Ctor_Hook(PVOID pThis)
{
    s_thisDVConsole = (DWORD*)pThis;
    PRINT_DEBUG("thisDVConsole: %p", s_thisDVConsole);

    return DVConsole_Ctor(pThis);
}

DWORD* __fastcall DVScript_Ctor_Hook(PVOID pThis)
{
    if (s_thisDVScript == nullptr)
    {
        s_thisDVScript = (DWORD*)pThis;
        PRINT_DEBUG("thisDVScript: %p", s_thisDVScript);
    }

    return DVScript_Ctor(pThis);
}

// -----------------------------------------------------------------------------

void stdinLoop()
{
    printf(
        "****************************************************************************************************\n"
        "* DESPERADOS+ CHEAT CONSOLE\n"
        "****************************************************************************************************\n"
        "MISC\n"
        "    quit                      Quit the game\n"
        "CHEATS\n"
        "    clint                     Win mission\n"
        "    zeus                      Kill enemies using view cone cursor\n"
        "    hollow man                Toggle invisibility\n"
        "    fidel castro              Play all dialogs\n"
        "    timeless                  Alternative pause mode (Default key: P)\n"
        "    powerman                  Unknown (I've got a big gun...)\n"
        "    partisan                  Everybody gets a lot of ammo\n"
        "    minimi                    Toggle gattling guns losing ammo\n"
        "    give all                  Everybody gets a bunch of everything\n"
        "    knife                     Cooper gets his knife\n"
        "    watch                     Cooper gets his watch\n"
        "    snake                     Sam gets a snake\n"
        "    tnt                       Sam gets some TNT\n"
        "    jackal                    Doc gets a lot of sniper bullets\n"
        "    schneider                 Mia gets a lot of peanuts\n"
        "    behind the ennemy horse   Set all heroes' health to 50\n"
        "DEBUG\n"
        "    show me all               Display map geometry\n"
        "    whats my destiny          Display all objectives in pause menu\n"
        "    epitaph                   Display all victory and failure conditions\n"
        "    supersonic                Display ranges of walk noise of the PCs (will probably crash the game)\n"
        "    medic                     Unknown debug view (arrow keys change top value)\n"
        "****************************************************************************************************\n"
    );

    DWORD       dwOldValue    = 0;
    DWORD       dwTemp        = 0;
    bool        minimiEnabled = false;
    std::string consoleInput;
    
    while (consoleInput != "quit")
    {
        getline(std::cin >> std::ws, consoleInput);
        std::for_each(
            consoleInput.begin(),
            consoleInput.end(),
            [](char& c) { c = static_cast<char>(::tolower(c)); }
        );
        
        if (consoleInput == "toggle cooper") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Cooper.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Cooper.\n";
                        *enabled = 1;
                    }
                }
            } 
        } else if (consoleInput == "toggle doc") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Doc.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Doc.\n";
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle sanchez") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Sanchez.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Sanchez.\n";
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle sam") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Sam.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Sam.\n";
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle kate") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Kate.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Kate.\n";
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle mia") {
            if (s_thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Mia.\n";
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Mia.\n";
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "partisan") {
            std::cout << "Giving everybody a lot of ammo.\n";
            if (s_thisDVConsole != nullptr) {
                WORD* amount;
                DWORD* cooper = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    amount = reinterpret_cast<WORD*>(*cooper) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* doc = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*doc) {
                    amount = reinterpret_cast<WORD*>(*doc) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sanchez = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*sanchez) {
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sam = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    amount = reinterpret_cast<WORD*>(*sam) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* kate = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*kate) {
                    amount = reinterpret_cast<WORD*>(*kate) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* mia = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*mia) {
                    amount = reinterpret_cast<WORD*>(*mia) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
            }
        } else if (consoleInput == "minimi") {
            PVOID subAddress = (PVOID)(s_gameBaseAddr + 0x00095D09);
            VirtualProtect((LPVOID)subAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
            if (!minimiEnabled) {
                std::cout << "Gatling guns have unlimited ammo.\n";
                BYTE newCode[] = { 0x90, 0x90 };
                memcpy(subAddress, newCode, sizeof(newCode));
            } else {
                std::cout << "Gatling guns have limited ammo.\n";
                BYTE newCode[] = { 0x2B, 0xD0 };
                memcpy(subAddress, newCode, sizeof(newCode));
            }
            VirtualProtect((LPVOID)subAddress, sizeof(DWORD), dwOldValue, &dwTemp);
            minimiEnabled = !minimiEnabled;
        } else if (consoleInput == "give all") {
            std::cout << "Giving a bunch of stuff to everybody.\n";
            if (s_thisDVConsole != nullptr) {
                WORD* amount;
                DWORD* doc = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*doc) {
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2B2 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2B4 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2C0 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sanchez = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*sanchez) {
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x2B2 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sam = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    amount = reinterpret_cast<WORD*>(*sam) + 0x2B4 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* kate = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*kate) {
                    amount = reinterpret_cast<WORD*>(*kate) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* mia = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*mia) {
                    amount = reinterpret_cast<WORD*>(*mia) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*mia) + 0x2B8 / sizeof(WORD);
                    *amount = 32767;
                }
            }
        } else if (consoleInput == "watch") {
            std::cout << "Giving Cooper his watch.\n";
            if (s_thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "knife") {
            std::cout << "Giving Cooper his knife.\n";
            if (s_thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "snake") {
            std::cout << "Giving Sam a snake.\n";
            if (s_thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "tnt") {
            std::cout << "Giving Sam some TNT.\n";
            if (s_thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*s_thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else {
            if (s_thisDVConsole != nullptr) {
                DWORD* input = reinterpret_cast<DWORD*>(s_thisDVConsole) + 0x4 / sizeof(DWORD);
                memcpy(input, consoleInput.c_str(), sizeof(consoleInput));
                PRINT_DEBUG("Calling DVConsole::ExecuteCommand with '%s'", (char*)input);
                PRINT_DEBUG("Return value: %i", (int)DVConsole_ExecuteCommand(s_thisDVConsole));
            }
        }
    }

    exit(EXIT_SUCCESS);
}

// -----------------------------------------------------------------------------

// https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time
static std::thread stdinLoopThread;

void initStdinLoop()
{
    PRINT_DEBUG("Entering stdin loop");
    stdinLoopThread = std::thread(stdinLoop);
}

static void initConsole()
{
    FILE* consoleOut = NULL;
    FILE* consoleIn  = NULL;

    AllocConsole();
    freopen_s(&consoleOut, "CONOUT$", "w", stdout);
    freopen_s(&consoleOut, "CONOUT$", "w", stderr);
    freopen_s(&consoleIn,  "CONIN$",  "r", stdin);
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
}

static void initFunctionAddresses(DWORD baseAddr)
{
    s_gameBaseAddr = baseAddr;

    // -----
    
    DVElementActorHuman_Translate =
        (tDVElementActorHuman_Translate)
        (baseAddr + 0x0009CC60);
    
    DVElementActorHuman_CheckIfViolationOfInternationalWar =
        (tDVElementActorHuman_CheckIfViolationOfInternationalWar)
        (baseAddr + 0x00093930);

    DVDoc_Ctor =
        (tDVDoc_Ctor)
        (baseAddr + 0x0007D1B0);

    DVConsole_Ctor =
        (tDVConsole_Ctor)
        (baseAddr + 0x00075DC0);

    DVConsole_ExecuteCommand =
        (tDVConsole_ExecuteCommand)
        (baseAddr + 0x00075DE0);

    SBDrawManager_PrintConsole =
        (tSBDrawManager_PrintConsole)
        (baseAddr + 0x0007A940);
    
    SBDrawManager_PrintConsoleWithInt =
        (tSBDrawManager_PrintConsoleWithInt)
        (baseAddr + 0x0010C9A6);

    DVScript_Ctor =
        (tDVScript_Ctor)
        (baseAddr + 0x0015B410);
    
    DVScript_This =
        (tDVScript_This)
        (baseAddr + 0x0015DAE0);
    
    DVScript_GetActor =
        (tDVScript_GetActor)
        (baseAddr + 0x0015CFF0);

    DVScript_GetCooper =
        (tDVScript_GetCooper)
        (baseAddr + 0x0015D4A0);

    DVScript_ActivatePC =
        (tDVScript_ActivatePC)
        (baseAddr + 0x0015B580);

    DVScript_DeactivatePC =
        (tDVScript_DeactivatePC)
        (baseAddr + 0x0015C620);
}

// -----------------------------------------------------------------------------

static void initHooks()
{
    // I don't fully understand why, but this function is allowed to return FALSE.
    DetourRestoreAfterWith();

    if (DetourTransactionBegin() != 0)
    {
        PRINT_ERROR("DetourTransactionBegin(): error code: %i", (int)GetLastError());
        return;
    }

    if (DetourUpdateThread(GetCurrentThread()) != 0)
    {
        PRINT_ERROR("DetourUpdateThread(): error code: %i", (int)GetLastError());
        return;
    }

    if (DetourAttach(&(PVOID&)DVConsole_Ctor, DVConsole_Ctor_Hook) != 0
        || DetourAttach(&(PVOID&)DVScript_Ctor, DVScript_Ctor_Hook) != 0)
    {
        PRINT_ERROR("DetourAttach(): error code: %i", (int)GetLastError());
        return;
    }

    if (DetourTransactionCommit() != 0)
    {
        PRINT_ERROR("DetourTransactionCommit(): error code: %i", (int)GetLastError());
        return;
    }
}

// -----------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        PRINT_DEBUG("DLL_PROCESS_ATTACH");
        initConsole();
        initFunctionAddresses((DWORD)GetModuleHandleA(NULL));
        initStdinLoop();
        break;

    case DLL_THREAD_ATTACH:
        PRINT_DEBUG("DLL_THREAD_ATTACH");
        initHooks();
        break;

    case DLL_THREAD_DETACH:
        PRINT_DEBUG("DLL_THREAD_DETACH");
        // TODO: Handle.
        break;

    case DLL_PROCESS_DETACH:
        PRINT_DEBUG("DLL_PROCESS_DETACH");
        // TODO: Handle.
        break;
    }

    return TRUE;
}
