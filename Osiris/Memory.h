#pragma once

#include <sstream>
#include <type_traits>
#include <Windows.h>
#include <Psapi.h>

class ClientMode;
class Entity;
class Input;
class ItemSchema;
class MoveHelper;
class MoveData;
class ViewRender;

struct GlobalVars;
struct GlowObjectManager;
struct Vector;
struct Trace;

class Memory {
public:
    Memory() noexcept;

    uintptr_t present;
    uintptr_t reset;

    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;

    bool* disablePostProcessing;

    std::add_pointer_t<void __fastcall(const char*)> loadSky;
    std::add_pointer_t<void __fastcall(const char*, const char*)> setClanTag;
    int* smokeCount;
    uintptr_t cameraThink;
    std::add_pointer_t<bool __stdcall(const char*)> acceptMatch;
    std::add_pointer_t<bool __cdecl(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__thiscall* getSequenceActivity)(void*, int);
    uintptr_t scopeArc;
    uintptr_t scopeLens;
    bool(__thiscall* isOtherEnemy)(Entity*, Entity*);
    uintptr_t hud;
    int*(__thiscall* findHudElement)(uintptr_t, const char*);
    int(__thiscall* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSchema* __cdecl()> itemSchema;
    void(__thiscall* setAbsOrigin)(Entity*, const Vector&);
    uintptr_t listLeaves;
    int* dispatchSound;
    std::add_pointer_t<bool __cdecl(float, float, float, float, float, float, Trace&)> traceToExit;
    ViewRender* viewRender;
    uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<bool __stdcall(const char*, const char*)> submitReport;
    uintptr_t test;
    uintptr_t test2;
    uint8_t* fakePrime;
    std::add_pointer_t<void __cdecl(const char* msg, ...)> debugMsg;
    float* vignette;
    int(__thiscall* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
private:
    template <typename T = uintptr_t>
    static auto findPattern(const wchar_t* module, const char* pattern, size_t offset = 0) noexcept
    {
        if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(module), &moduleInfo, sizeof(moduleInfo))) {
            auto start{ static_cast<const char*>(moduleInfo.lpBaseOfDll) };
            auto end{ start + moduleInfo.SizeOfImage };

            auto first{ start };
            auto second{ pattern };

            while (first < end && *second) {
                if (*first == *second || *second == '?') {
                    first++;
                    second++;
                } else {
                    first = ++start;
                    second = pattern;
                }
            }

            if (!*second)
                return reinterpret_cast<T>(const_cast<char*>(start) + offset);
        }
        MessageBoxA(NULL, (std::ostringstream{ } << "Failed to find pattern in " << module << '!').str().c_str(), "Osiris", MB_OK | MB_ICONWARNING);
     
        exit(EXIT_FAILURE);
    }
};

extern Memory memory;
