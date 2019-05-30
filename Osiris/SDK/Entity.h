#pragma once

#include "ClientClass.h"
#include "../Interfaces.h"
#include "../Netvars.h"
#include "Cvar.h"
#include "Engine.h"
#include "EngineTrace.h"
#include "EntityList.h"
#include "Utils.h"
#include "Vector.h"
#include "WeaponId.h"
#include "VarMapping.h"
#include "../Memory.h"

enum class MoveType {
    NOCLIP = 8,
    LADDER = 9
};

class Entity {
public:
    template <typename T>
    constexpr auto getProperty(const char* name, const std::size_t offset = 0) const noexcept
    {
        return *reinterpret_cast<const T*>(this + netvars[name] + offset);
    }

    template <typename T>
    constexpr void setProperty(const char* name, const T& value) noexcept
    {
        *reinterpret_cast<T*>(this + netvars[name]) = value;
    }

    constexpr bool isPistol() noexcept
    {
        switch (getClientClass()->classId) {
        case ClassId::Deagle:
        case ClassId::Elite:
        case ClassId::FiveSeven:
        case ClassId::Glock:
        case ClassId::P2000:
        case ClassId::P250:
        case ClassId::Tec9:
            return true;
        default:
            return false;
        }
    }

    constexpr bool isSniperRifle() noexcept
    {
        switch (getClientClass()->classId) {
        case ClassId::Ssg08:
        case ClassId::Awp:
        case ClassId::Scar20:
        case ClassId::G3sg1:
            return true;
        default:
            return false;
        }
    }

    using matrix3x4 = float[3][4];

    constexpr bool setupBones(matrix3x4* out, int maxBones, int boneMask, float currentTime) noexcept
    {
        return callVirtualMethod<bool, matrix3x4*, int, int, float>(this + 4, 13, out, maxBones, boneMask, currentTime);
    }

    Vector getBonePosition(int bone) noexcept
    {
        static matrix3x4 boneMatrices[128];
        if (setupBones(boneMatrices, 128, 256, 0.0f))
            return Vector{ boneMatrices[bone][0][3], boneMatrices[bone][1][3], boneMatrices[bone][2][3] };
        else
            return Vector{ };
    }

    constexpr Vector getEyePosition() noexcept
    {
        Vector vec{ };
        callVirtualMethod<void, Vector&>(this, 281, vec);
        return vec;
    }

    bool isVisible(const Vector& position = { }) noexcept
    {
        auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        static Trace trace;
        interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), position ? position : getBonePosition(8) }, 0x46004009, { localPlayer }, trace);
        return trace.entity == this || trace.fraction > 0.97f;
    }

    bool isEnemy() noexcept
    {
        return memory.isOtherEnemy(this, interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()));
    }

    constexpr bool isDormant() noexcept
    {
        return callVirtualMethod<bool>(this + 8, 9);
    }

    constexpr void preDataUpdate(int updateType) noexcept
    {
        callVirtualMethod<void, int>(this + 8, 6, updateType);
    }

    constexpr void release() noexcept
    {
        return callVirtualMethod<void>(this + 8, 1);
    }

    constexpr void setDestroyedOnRecreateEntities() noexcept
    {
        return callVirtualMethod<void>(this + 8, 13);
    }

    constexpr bool isWeapon() noexcept
    {
        return callVirtualMethod<bool>(this, 163);
    }

    constexpr ClientClass* getClientClass() noexcept
    {
        return callVirtualMethod<ClientClass*>(this + 8, 2);
    }

    constexpr bool isAlive() noexcept
    {
        return callVirtualMethod<bool>(this, 153) && getProperty<int>("m_iHealth") > 0;
    }

    constexpr float getInaccuracy() noexcept
    {
        return callVirtualMethod<float>(this, 476);
    }
    
    VarMap* getVarMap() noexcept
    {
        return reinterpret_cast<VarMap*>(this + 0x24);
    }
    
    constexpr Vector getAbsOrigin() noexcept
    {
        return callVirtualMethod<Vector&>(this, 10);
    }

    constexpr void setModelIndex(int index) noexcept
    {
        callVirtualMethod<void, int>(this, 75, index);
    }

    NETVAR_OFFSET(index, "CBaseEntity", "m_bIsAutoaimTarget", 4, int);
    NETVAR(GetModelIndex, "CBaseEntity", "m_nModelIndex", unsigned);

    NETVAR(GetWeapons, "CBaseCombatCharacter", "m_hMyWeapons", std::array<int, 48>);
    PNETVAR(GetWearables, "CBaseCombatCharacter", "m_hMyWearables", int);

    NETVAR(GetLifeState, "CBasePlayer", "m_lifeState", int);
    NETVAR(GetViewModel, "CBasePlayer", "m_hViewModel[0]", int);

    NETVAR(GetViewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex", int);
    NETVAR(GetWorldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex", int);
    NETVAR(GetWorldDroppedModelIndex, "CBaseCombatWeapon", "m_iWorldDroppedModelIndex", int);
    NETVAR(GetWeaponWorldModel, "CBaseCombatWeapon", "m_hWeaponWorldModel", int);

    NETVAR(GetAccountID, "CBaseAttributableItem", "m_iAccountID", int);
    NETVAR(GetItemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex", short);
    NETVAR(GetItemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh", int);
    NETVAR(GetEntityQuality, "CBaseAttributableItem", "m_iEntityQuality", int);
    NETVAR(GetCustomName, "CBaseAttributableItem", "m_szCustomName", char[32]);
    NETVAR(GetFallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit", unsigned);
    NETVAR(GetFallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed", unsigned);
    NETVAR(GetFallbackWear, "CBaseAttributableItem", "m_flFallbackWear", float);
    NETVAR(GetFallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak", unsigned);

    NETVAR(GetOwner, "CBaseViewModel", "m_hOwner", int);
    NETVAR(GetWeapon, "CBaseViewModel", "m_hWeapon", int);
};
