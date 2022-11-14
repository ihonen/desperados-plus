#include <iostream>
#include <windows.h>
#include <detours.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <atomic>
#include <thread>
#include <algorithm>

using namespace std;

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

// Game Offset: 0x00400000
void InitFunctionAddresses(DWORD game) {
    gameAddress = game;
    DVElementActorHumanTranslate = (tDVElementActorHumanTranslate)(gameAddress + 0x0009CC60);
    DVElementActorHumanCheckIfViolationOfInternationalWar = (tDVElementActorHumanCheckIfViolationOfInternationalWar)(gameAddress + 0x00093930);

    DVDoc = (tDVDoc)(gameAddress + 0x0007D1B0);

    DVConsole = (tDVConsole)(gameAddress + 0x00075DC0);
    DVConsoleExecuteCommand = (tDVConsoleExecuteCommand)(gameAddress + 0x00075DE0);

    SBDrawManagerPrintConsole = (tSBDrawManagerPrintConsole)(gameAddress + 0x0007A940);
    SBDrawManagerPrintConsoleWithInt = (tSBDrawManagerPrintConsoleWithInt)(gameAddress + 0x0010C9A6);

    DVScript = (tDVScript)(gameAddress + 0x0015B410);
    DVScriptThis = (tDVScriptThis)(gameAddress + 0x0015DAE0);
    DVScriptGetActor = (tDVScriptGetActor)(gameAddress + 0x0015CFF0);
    DVScriptGetCooper = (tDVScriptGetCooper)(gameAddress + 0x0015D4A0);
    DVScriptActivatePC = (tDVScriptActivatePC)(gameAddress + 0x0015B580);
    DVScriptDeactivatePC = (tDVScriptDeactivatePC)(gameAddress + 0x0015C620);
}

DWORD* thisDVScript = nullptr;
DWORD* thisDVConsole = nullptr;

DWORD* __fastcall DVConsole_Hook(PVOID pThis)
{
    thisDVConsole = (DWORD*)pThis;
    cout << "DVConsole is at: " << thisDVConsole << endl;
    return DVConsole(pThis);
}

void _cdecl SBDrawManagerPrintConsole_Hook(PVOID pThis, char* text)
{
    if (&text)
        cout << text << endl;
}

void _cdecl	SBDrawManagerPrintConsoleWithInt_Hook(PVOID pThis, DWORD* value, char* text)
{
    /*if (&text)
        cout << text << endl;*/
}

DWORD* __fastcall DVScript_Hook(PVOID pThis)
{
    if (thisDVScript == nullptr) {
        thisDVScript = (DWORD*)pThis;
        cout << "DVScript is at: " << thisDVScript << endl;
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
        //cout << *Health << endl;
    }
    //cout << "DVElementActorHumanCheckIfViolationOfInternationalWar: " << a1 << " - " << a2 << " = " << x << endl;
    return x;
}

// https://stackoverflow.com/questions/6736536/c-input-and-output-to-the-console-window-at-the-same-time
std::thread cinThread;

void ReadCin()
{
    string consoleInput;
    DWORD dwOldValue, dwTemp;
    bool minimiEnabled = false;

    while (consoleInput != "qqq") {
        //cin >> consoleInput;
        //cout << "> ";
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
                        cout << "Disabling control for Cooper." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Cooper." << endl;
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
                        cout << "Disabling control for Doc." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Doc." << endl;
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
                        cout << "Disabling control for Sanchez." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Sanchez." << endl;
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
                        cout << "Disabling control for Sam." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Sam." << endl;
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
                        cout << "Disabling control for Kate." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Kate." << endl;
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
                        cout << "Disabling control for Mia." << endl;
                        *enabled = 0;
                    } else {
                        cout << "Enabling control for Mia." << endl;
                        *enabled = 1;
                    }
                }
            }
        } else if (consoleInput == "partisan") {
            cout << "Giving everybody a lot of ammo." << endl;
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
                cout << "Gattling guns don't lose ammo." << endl;
                BYTE newCode[] = { 0x90, 0x90 };
                memcpy(subAddress, newCode, sizeof(newCode));
            } else {
                cout << "Gattling guns do lose ammo." << endl;
                BYTE newCode[] = { 0x2B, 0xD0 };
                memcpy(subAddress, newCode, sizeof(newCode));
            }
            VirtualProtect((LPVOID)subAddress, sizeof(DWORD), dwOldValue, &dwTemp);
            minimiEnabled = !minimiEnabled;
        } else if (consoleInput == "give all") {
            cout << "Giving a bunch of stuff to everybody." << endl;
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
            cout << "Giving Cooper his watch." << endl;
            if (thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "knife") {
            cout << "Giving Cooper his knife." << endl;
            if (thisDVConsole != nullptr) {
                DWORD* cooper = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCCC / sizeof(DWORD);
                if (*cooper) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*cooper) + 0x2C1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "snake") {
            cout << "Giving Sam a snake." << endl;
            if (thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B0 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        } else if (consoleInput == "tnt") {
            cout << "Giving Sam some TNT." << endl;
            if (thisDVConsole != nullptr) {
                DWORD* sam = reinterpret_cast<DWORD*>(*thisDVConsole) + 0xCD8 / sizeof(DWORD);
                if (*sam) {
                    BYTE* amount = reinterpret_cast<BYTE*>(*sam) + 0x2B1 / sizeof(BYTE);
                    *amount = 1;
                }
            }
        }
        
        
        /*if (consoleInput == "disable cooper") {
            cout << "Disabling control for Cooper." << endl;
            if (thisDVScript != nullptr) {
                DWORD* cooper = DVScriptGetCooper(thisDVScript);
                DVScriptDeactivatePC(cooper, thisDVScript);
            }
        } else if (consoleInput == "enable cooper") {
            cout << "Enabling control for Cooper." << endl;
            if (thisDVScript != nullptr) {
                DWORD* cooper = DVScriptGetCooper(thisDVScript);
                DVScriptActivatePC(cooper, thisDVScript);
            }
        } else if (consoleInput == "spawn_doc") {
            cout << "Spawning Doc." << endl;
            if (thisDVConsole != nullptr) {
               PVOID doc = malloc(0x2C8u);
               PVOID docChar = DVDoc(doc); // TODO: Get this from DVEngine

            }
        } else if (consoleInput == "ammo") {
            cout << "Giving Doc ammo." << endl;
            if (thisDVConsole != nullptr) {
                DWORD* doc = reinterpret_cast<DWORD*>(*thisDVConsole) + 3280 / sizeof(DWORD);
                WORD* docSniperAmmo = reinterpret_cast<WORD*>(*doc) + 704 / sizeof(WORD);
                *docSniperAmmo = 5;
            }
        }*/ else {
            if (thisDVConsole != nullptr) {
                DWORD* input = reinterpret_cast<DWORD*>(thisDVConsole) + 0x4 / sizeof(DWORD);
                memcpy(input, consoleInput.c_str(), sizeof(consoleInput));
                //cout << "Entering cheat: " << (char*)input << endl;
                DVConsoleExecuteCommand(thisDVConsole);
            }
        }
    }

    exit(0);
}

// ##### MAIN #######################################################################################

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    FILE* streamOut;
    FILE* streamIn;
    LONG errorCode = 0;

    PVOID quickAddress;
    BYTE toCpy[] = { 0xE8 };
    BYTE toCpy2[] = { 0x90, 0x90 };
    DWORD dwOldValue, dwTemp;
    
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen_s(&streamOut, "CONOUT$", "w", stdout);
        freopen_s(&streamOut, "CONOUT$", "w", stderr);
        freopen_s(&streamIn, "CONIN$", "r", stdin);
        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();

        //std::cout << "Module Address: " << ": " << (DWORD)GetModuleHandleA(NULL) << std::endl;
        InitFunctionAddresses((DWORD)GetModuleHandleA(NULL));

        cinThread = thread(ReadCin);
        break;
    case DLL_THREAD_ATTACH:
        //std::cout << "Module Address ATTACH: " << ": " << (DWORD)GetModuleHandleA(NULL) << std::endl;
        
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        // Attaching
        errorCode = DetourAttach(&(PVOID&)DVConsole, DVConsole_Hook);
        errorCode = DetourAttach(&(PVOID&)DVScript, DVScript_Hook);
        //errorCode = DetourAttach(&(PVOID&)DVElementActorHumanTranslate, DVElementActorHumanTranslate_Hook);
        //errorCode = DetourAttach(&(PVOID&)DVElementActorHumanCheckIfViolationOfInternationalWar, DVElementActorHumanCheckIfViolationOfInternationalWar_Hook);

        /*
        errorCode = DetourAttach(&(PVOID&)SBDrawManagerPrintConsole, SBDrawManagerPrintConsole_Hook);
        // Print with INT 1
        quickAddress = (PVOID)(gameAddress + 0x0010C9A6);
        errorCode = DetourAttach(&(PVOID&)quickAddress, SBDrawManagerPrintConsoleWithInt_Hook);
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
        memcpy(quickAddress, toCpy, sizeof(toCpy));
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), dwOldValue, &dwTemp);
        // Print with INT 2
        quickAddress = (PVOID)(gameAddress + 0x0018FD7E);
        errorCode = DetourAttach(&(PVOID&)quickAddress, SBDrawManagerPrintConsoleWithInt_Hook);
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
        memcpy(quickAddress, toCpy, sizeof(toCpy));
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), dwOldValue, &dwTemp);
        // Print with INT 3
        quickAddress = (PVOID)(gameAddress + 0x00190385);
        errorCode = DetourAttach(&(PVOID&)quickAddress, SBDrawManagerPrintConsoleWithInt_Hook);
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
        memcpy(quickAddress, toCpy, sizeof(toCpy));
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), dwOldValue, &dwTemp);
        // Print with INT 4 - to nop
        quickAddress = (PVOID)(gameAddress + 0x0004408A);
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
        memcpy(quickAddress, toCpy2, sizeof(toCpy2));
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), dwOldValue, &dwTemp);
        quickAddress = (PVOID)(gameAddress + 0x00132D25);
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldValue);
        memcpy(quickAddress, toCpy2, sizeof(toCpy2));
        VirtualProtect((LPVOID)quickAddress, sizeof(DWORD), dwOldValue, &dwTemp);
        */

        if (errorCode) {
            std::cerr << "ATTACH ERROR: " << errorCode << " - " << GetLastError() << std::endl;
        }
        errorCode = DetourTransactionCommit();
        if (errorCode) {
            std::cerr << "ERROR TRANSACTION: " << errorCode << " - " << GetLastError() << std::endl;
        }

    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}