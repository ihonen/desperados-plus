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
# define DEBUG(f, ...) \
    fprintf(stderr, "DEBUG: " f "\n", ##__VA_ARGS__)

# define INFO(f, ...) \
    fprintf(stderr, "INFO:  " f "\n", ##__VA_ARGS__)

# define WARN(f, ...) \
    fprintf(stderr, "WARN:  " f "\n", ##__VA_ARGS__)

# define ERROR(f, ...) \
    fprintf(stderr, "ERROR: " f "\n", ##__VA_ARGS__)
#else
# define DEBUG(...)
# define INFO(...)
# define WARN(...)
# define ERROR(...)
#endif

// -----------------------------------------------------------------------------

// IF __cdecl THEN invert parameter list

// OTHER
// void __thiscall DVElementActorHuman::Translate (DVElementActorHuman * this, DVSequenceElement * param_1, DVcommand param_2)
typedef void (__thiscall* tDVElementActorHumanTranslate) (PVOID, DWORD*, DWORD);
// void __thiscall DVElementActorHuman::CheckIfViolationOfInternationalWar (DVElementActorHuman * this, DVElement * param_1, DVElement * param_2)
typedef int (__stdcall* tDVElementActorHumanCheckIfViolationOfInternationalWar) (DWORD*, DWORD*);

// CHARACTERS
// DVDoc *__cdecl DVDoc::DVDoc(DVDoc *__hidden this)
typedef DWORD* (__thiscall* tDVDoc) (PVOID);

// CONSOLE
// _DWORD __cdecl DVConsole::DVConsole(DVConsole *this)
typedef DWORD* (__thiscall* tDVConsole) (PVOID);
// _DWORD __cdecl DVConsole::ExecuteCommand(DVConsole* this)
typedef DWORD (__thiscall* tDVConsoleExecuteCommand) (PVOID);

// DRAW MANAGER
// _DWORD SBDrawManager::PrintConsole(SBDrawManager *__hidden this, char *, ...)
typedef char (__cdecl* tSBDrawManagerPrintConsole) (PCHAR, PVOID);
// _DWORD SBDrawManager::PrintConsole(SBDrawManager *__hidden this, unsigned int, char *, ...)
typedef char(__cdecl* tSBDrawManagerPrintConsoleWithInt) (PCHAR, DWORD, PVOID);

// SCRIPTS
// _DWORD __cdecl DVScript::DVScript(DVScript *this)
typedef DWORD* (__thiscall* tDVScript) (PVOID);
// int DVScript::This()
typedef int* (__stdcall* tDVScriptThis) ();
// _DWORD __cdecl DVScript::GetActor(DVScript *this, int)
typedef DWORD (__cdecl* tDVScriptGetActor) (int, PVOID);
// _DWORD DVScript::GetCooper(DVScript *__hidden this)
typedef DWORD* (__cdecl* tDVScriptGetCooper) (PVOID);
// _DWORD __cdecl DVScript::Activate(DVScript *this, void *)
typedef int (__cdecl* tDVScriptActivatePC) (DWORD*, PVOID);
// _DWORD __cdecl DVScript::Deactivate(DVScript *this, void *)
typedef int (__cdecl* tDVScriptDeactivatePC) (DWORD*, PVOID);

DWORD gameAddress = 0x0;
tDVElementActorHumanTranslate DVElementActorHumanTranslate;

tDVDoc DVDoc;

tDVElementActorHumanCheckIfViolationOfInternationalWar DVElementActorHumanCheckIfViolationOfInternationalWar;
tDVConsole DVConsole;
tDVConsoleExecuteCommand DVConsoleExecuteCommand;

tSBDrawManagerPrintConsole SBDrawManagerPrintConsole;
tSBDrawManagerPrintConsoleWithInt SBDrawManagerPrintConsoleWithInt;

tDVScript DVScript;
tDVScriptThis DVScriptThis;
tDVScriptGetActor DVScriptGetActor;
tDVScriptGetCooper DVScriptGetCooper;
tDVScriptActivatePC DVScriptActivatePC;
tDVScriptDeactivatePC DVScriptDeactivatePC;

DWORD* thisDVScript = nullptr;
DWORD* thisDVConsole = nullptr;

DWORD* __fastcall DVConsole_Hook(PVOID pThis)
{
    thisDVConsole = (DWORD*)pThis;
    std::cout << "DVConsole is at: " << thisDVConsole << std::endl;
    return DVConsole(pThis);
}

void _cdecl SBDrawManagerPrintConsole_Hook(PVOID pThis, char* text)
{
    if (&text)
        std::cout << text << std::endl;
}

void _cdecl	SBDrawManagerPrintConsoleWithInt_Hook(PVOID pThis, DWORD* value, char* text)
{
    /*if (&text)
        std::cout << text << std::endl;*/
}

DWORD* __fastcall DVScript_Hook(PVOID pThis)
{
    if (thisDVScript == nullptr) {
        thisDVScript = (DWORD*)pThis;
        std::cout << "DVScript is at: " << thisDVScript << std::endl;
    }
    return DVScript(pThis);
}

void __fastcall DVElementActorHumanTranslate_Hook(PVOID pThis, PVOID unused, DWORD* actor, DWORD a3)
{
    const int DEFAULT = 23;
    const int KNOCKOUT = 45;
    const int SHOT = 47;

    DVElementActorHumanTranslate(pThis, actor, a3);
}

int __stdcall DVElementActorHumanCheckIfViolationOfInternationalWar_Hook(DWORD* a1, DWORD* a2) {

    int x = DVElementActorHumanCheckIfViolationOfInternationalWar(a1, a2);
    PVOID pDamageBase = 0;
    __asm mov pDamageBase, edi
    if (pDamageBase) {
        short* Health = reinterpret_cast<short*>(pDamageBase) + 0x16 / 2;
        //std::cout << *Health << std::endl;
    }
    //std::cout << "DVElementActorHumanCheckIfViolationOfInternationalWar: " << a1 << " - " << a2 << " = " << x << std::endl;
    return x;
}

// -----------------------------------------------------------------------------

void stdinLoop()
{
    printf(
        "****************************************************************************************************\n"
        "* DESPERADOS+ CHEAT CONSOLE\n"
        "****************************************************************************************************\n"
        "MISC\n"
        "    qqq                       Quit the game\n"
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

    std::string consoleInput;
    DWORD dwOldValue, dwTemp;
    bool minimiEnabled = false;

    while (consoleInput != "qqq") {
        //cin >> consoleInput;
        //std::cout << "> ";
        getline(std::cin >> std::ws, consoleInput);
        std::for_each(consoleInput.begin(), consoleInput.end(), [](char& c) {
            c = ::tolower(c);
        });

        
        if (consoleInput == "toggle cooper") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Cooper." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Cooper." << std::endl;
                        *enabled = 1;
                    }
                }
            } 
        } else if (consoleInput == "toggle doc") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Doc." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Doc." << std::endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle sanchez") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Sanchez." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Sanchez." << std::endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle sam") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Sam." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Sam." << std::endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle kate") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Kate." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Kate." << std::endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "toggle mia") {
            if (thisDVConsole != nullptr) {
                DWORD* hero = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*hero) {
                    BYTE* enabled = reinterpret_cast<BYTE*>(*hero) + 0xB5 / sizeof(BYTE);
                    if (*enabled) {
                        std::cout << "Disabling control for Mia." << std::endl;
                        *enabled = 0;
                    } else {
                        std::cout << "Enabling control for Mia." << std::endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "partisan") {
            std::cout << "Giving everybody a lot of ammo." << std::endl;
            if (thisDVConsole != nullptr) {
                WORD* amount;
                DWORD* cooper = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    amount = reinterpret_cast<WORD*>(*cooper) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* doc = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*doc) {
                    amount = reinterpret_cast<WORD*>(*doc) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sanchez = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*sanchez) {
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    amount = reinterpret_cast<WORD*>(*sam) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* kate = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*kate) {
                    amount = reinterpret_cast<WORD*>(*kate) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* mia = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*mia) {
                    amount = reinterpret_cast<WORD*>(*mia) + 0x146 / sizeof(WORD);
                    *amount = 32767;
                }
            }
        } else if (consoleInput == "minimi") {
            PVOID subAddress = (PVOID)(gameAddress + 0x00095D09);
            VirtualProtect((LPVOID)subAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
            if (!minimiEnabled) {
                std::cout << "Gattling guns don't lose ammo." << std::endl;
                BYTE newCode[] = { 0x90, 0x90 };
                memcpy(subAddress, newCode, sizeof(newCode));
            } else {
                std::cout << "Gattling guns do lose ammo." << std::endl;
                BYTE newCode[] = { 0x2B, 0xD0 };
                memcpy(subAddress, newCode, sizeof(newCode));
            }
            VirtualProtect((LPVOID)subAddress, sizeof(DWORD), dwOldValue, &dwTemp);
            minimiEnabled = !minimiEnabled;
        } else if (consoleInput == "give all") {
            std::cout << "Giving a bunch of stuff to everybody." << std::endl;
            if (thisDVConsole != nullptr) {
                WORD* amount;
                DWORD* doc = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD0 / sizeof(DWORD);
                if (*doc) {
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2B2 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2B4 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*doc) + 0x2C0 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sanchez = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD4 / sizeof(DWORD);
                if (*sanchez) {
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*sanchez) + 0x2B2 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    amount = reinterpret_cast<WORD*>(*sam) + 0x2B4 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* kate = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCDC / sizeof(DWORD);
                if (*kate) {
                    amount = reinterpret_cast<WORD*>(*kate) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                }
                DWORD* mia = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCE0 / sizeof(DWORD);
                if (*mia) {
                    amount = reinterpret_cast<WORD*>(*mia) + 0x2B0 / sizeof(WORD);
                    *amount = 32767;
                    amount = reinterpret_cast<WORD*>(*mia) + 0x2B8 / sizeof(WORD);
                    *amount = 32767;
                }
            }
        } else if (consoleInput == "watch") {
            std::cout << "Giving Cooper his watch." << std::endl;
            if (thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "knife") {
            std::cout << "Giving Cooper his knife." << std::endl;
            if (thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "snake") {
            std::cout << "Giving Sam a snake." << std::endl;
            if (thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "tnt") {
            std::cout << "Giving Sam some TNT." << std::endl;
            if (thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        }
        
        
        /*if (consoleInput == "disable cooper") {
            std::cout << "Disabling control for Cooper." << std::endl;
            if (thisDVScript != nullptr) {
                DWORD* cooper = DVScriptGetCooper(thisDVScript);
                DVScriptDeactivatePC(cooper, thisDVScript);
            }
        } else if (consoleInput == "enable cooper") {
            std::cout << "Enabling control for Cooper." << std::endl;
            if (thisDVScript != nullptr) {
                DWORD* cooper = DVScriptGetCooper(thisDVScript);
                DVScriptActivatePC(cooper, thisDVScript);
            }
        } else if (consoleInput == "spawn_doc") {
            std::cout << "Spawning Doc." << std::endl;
            if (thisDVConsole != nullptr) {
               PVOID doc = malloc(0x2C8u);
               PVOID docChar = DVDoc(doc); // TODO: Get this from DVEngine

            }
        } else if (consoleInput == "ammo") {
            std::cout << "Giving Doc ammo." << std::endl;
            if (thisDVConsole != nullptr) {
                DWORD* doc = reinterpret_cast<DWORD*>(*thisDVConsole) + 3280 / sizeof(DWORD);
                WORD* docSniperAmmo = reinterpret_cast<WORD*>(*doc) + 704 / sizeof(WORD);
                *docSniperAmmo = 5;
            }
        }*/ else {
            if (thisDVConsole != nullptr) {
                DWORD* input = reinterpret_cast<DWORD*>(thisDVConsole) + 0x4 / sizeof(DWORD);
                memcpy(input, consoleInput.c_str(), sizeof(consoleInput));
                //std::cout << "Entering cheat: " << (char*)input << std::endl;
                DVConsoleExecuteCommand(thisDVConsole);
            }
        }
    }

    exit(0);
}

// -----------------------------------------------------------------------------

// https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time
static std::thread stdinLoopThread;

void initStdinLoop()
{
    DEBUG("Entering stdin loop");
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
    gameAddress = baseAddr;

    // -----
    
    DVElementActorHumanTranslate =
        (tDVElementActorHumanTranslate)
        (baseAddr + 0x0009CC60);
    
    DVElementActorHumanCheckIfViolationOfInternationalWar =
        (tDVElementActorHumanCheckIfViolationOfInternationalWar)
        (baseAddr + 0x00093930);

    DVDoc =
        (tDVDoc)
        (baseAddr + 0x0007D1B0);

    DVConsole =
        (tDVConsole)
        (baseAddr + 0x00075DC0);

    DVConsoleExecuteCommand =
        (tDVConsoleExecuteCommand)
        (baseAddr + 0x00075DE0);

    SBDrawManagerPrintConsole =
        (tSBDrawManagerPrintConsole)
        (baseAddr + 0x0007A940);
    
    SBDrawManagerPrintConsoleWithInt =
        (tSBDrawManagerPrintConsoleWithInt)
        (baseAddr + 0x0010C9A6);

    DVScript =
        (tDVScript)
        (baseAddr + 0x0015B410);
    
    DVScriptThis =
        (tDVScriptThis)
        (baseAddr + 0x0015DAE0);
    
    DVScriptGetActor =
        (tDVScriptGetActor)
        (baseAddr + 0x0015CFF0);

    DVScriptGetCooper =
        (tDVScriptGetCooper)
        (baseAddr + 0x0015D4A0);

    DVScriptActivatePC =
        (tDVScriptActivatePC)
        (baseAddr + 0x0015B580);

    DVScriptDeactivatePC =
        (tDVScriptDeactivatePC)
        (baseAddr + 0x0015C620);
}

// -----------------------------------------------------------------------------

static void initHooks()
{
    // I don't fully understand why, but this function is allowed to return FALSE.
    DetourRestoreAfterWith();

    if (DetourTransactionBegin() != 0)
    {
        ERROR("DetourTransactionBegin(): %i", (int)GetLastError());
        return;
    }

    if (DetourUpdateThread(GetCurrentThread()) != 0)
    {
        ERROR("DetourUpdateThread(): %i", (int)GetLastError());
        return;
    }

    if (DetourAttach(&(PVOID&)DVConsole, DVConsole_Hook) != 0
        || DetourAttach(&(PVOID&)DVScript, DVScript_Hook) != 0)
    {
        ERROR("DetourAttach(): %i", (int)GetLastError());
        return;
    }

    if (DetourTransactionCommit() != 0)
    {
        ERROR("DetourTransactionCommit(): %i", (int)GetLastError());
        return;
    }
}

// -----------------------------------------------------------------------------

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DEBUG("DLL_PROCESS_ATTACH");
        initConsole();
        initFunctionAddresses((DWORD)GetModuleHandleA(NULL));
        initStdinLoop();
        break;

    case DLL_THREAD_ATTACH:
        DEBUG("DLL_THREAD_ATTACH");
        initHooks();
        break;

    case DLL_THREAD_DETACH:
        DEBUG("DLL_THREAD_DETACH");
        break;

    case DLL_PROCESS_DETACH:
        DEBUG("DLL_PROCESS_DETACH");
        break;
    }

    return TRUE;
}
