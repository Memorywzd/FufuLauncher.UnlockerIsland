#include "Paimon.h"
#include "../Patterns/Patterns.h"
#include "../Config/Config.h"
#include "../Core/Utils.h"
#include <iostream>
#include <ctime>

void HandlePaimon() {
    auto& cfg = Config::Get();
    if (!cfg.display_paimon_v1) return;

    auto FindString = (tFindString)p_FindString.load();
    auto FindGameObject = (tFindGameObject)p_FindGameObject.load();
    auto SetActive = (tSetActive)o_SetActive.load();
    auto GetActive = (tGetActive)p_GetActive.load();

    if (!FindString || !FindGameObject || !SetActive || !GetActive) {
        return;
    }

    static float lastSearchTime = 0.0f;
    float currentTime = (float)clock() / CLOCKS_PER_SEC;

    if (currentTime - lastSearchTime > 2.0f) {
        lastSearchTime = currentTime;

        SafeInvoke([&] {
            Il2CppString* paimonStr = FindString(GameStrings::PaimonPath);
            Il2CppString* profileStr = FindString(GameStrings::ProfileLayerPath);

            if (paimonStr && profileStr) {
                void* paimonObj = FindGameObject(paimonStr);
                void* profileObj = FindGameObject(profileStr);

                if (paimonObj && profileObj) {
                    bool profileOpen = GetActive(profileObj);

                    if (profileOpen) {
                        SetActive(paimonObj, false);
                    } else {
                        SetActive(paimonObj, true);
                    }
                }
            }
        });
    }
}

bool CheckResistInBeyd(bool cache) {
    return false;

    if (cache) {
        return g_ResistInBeyd;
    }

    uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
    auto _FindString = (tFindString)p_FindString.load();
    auto _FindGameObject = (tFindGameObject)p_FindGameObject.load();

    std::string getTextStr = Offsets::GetText;
    std::string getComponentStr = Offsets::GetComponent;
    uintptr_t getTextOffsetVal = 0x15B61F60;
    uintptr_t getComponentOffsetVal = 0x15C45190;

    auto _GetText = (tGetText)(base + getTextOffsetVal);
    auto _GetComponent = (tGetComponent)(base + getComponentOffsetVal);

    if (!_FindString || !_FindGameObject || !_GetText || !_GetComponent) {
        return true;
    }

    Il2CppString* uidStrObj = _FindString(GameStrings::UIDPathWatermark);
    Il2CppString* textStrObj = _FindString("Text");
    if (uidStrObj)
    {
        void* uidObj = _FindGameObject(uidStrObj);
        if (uidObj)
        {
            void* textComponent = _GetComponent(uidObj, textStrObj);
            if (textComponent)
            {
                Il2CppString* textValue = _GetText(textComponent);
                if (textValue)
                {
                    const wchar_t* textChars = textValue->chars;
                    const wchar_t* resistText = L"GUID";
                    return wcsstr(textChars, resistText) != nullptr;
                }
            }
        }

        return false;
    }

    return false;
}

void WINAPI hk_ActorManagerCtor(void* pThis) {
    g_ActorManagerInstance = pThis;
    auto orig = (tActorManagerCtor)o_ActorManagerCtor.load();
    if (orig) orig(pThis);
}

void WINAPI hk_SetUID(void* pThis, uint32_t uid) {
    if (uid > 10000000 && g_CurrentUID == 0) {
        g_CurrentUID = uid;
        std::cout << "[+] UID (from SetUID Hook): " << uid << '\n';
    }
    auto orig = (tSetUid)o_SetUid.load();
    if (orig) orig(pThis, uid);
}

void UpdatePaimonV2() {
    static ULONGLONG lastLogTick = 0;
    ULONGLONG currentTick = GetTickCount64();
    bool canLog = (currentTick - lastLogTick > 2000);

    auto& cfg = Config::Get();

    if (!cfg.display_paimon_v2 || cfg.display_paimon_v1) {
        return;
    }

    if (!g_ActorManagerInstance) {
        if (canLog) {
            std::cout << "[PaimonV2_Blocker] g_ActorManagerInstance is NULL!" << std::endl;
            lastLogTick = currentTick;
        }
        return;
    }

    auto GetGlobalActor = (tGetGlobalActor)p_GetGlobalActor.load();
    auto GetActive = (tGetActive)p_GetActive.load();
    auto FindString = (tFindString)p_FindString.load();
    auto FindGameObject = (tFindGameObject)p_FindGameObject.load();
    auto AvatarPaimonAppear = (tAvatarPaimonAppear)p_AvatarPaimonAppear.load();

    if (!GetGlobalActor || !GetActive || !FindString || !FindGameObject || !AvatarPaimonAppear) {
        if (canLog) {
            std::cout << "[PaimonV2_Blocker] Required function pointers are missing!" << std::endl
                      << " -> GetGlobalActor: " << GetGlobalActor << std::endl
                      << " -> GetActive: " << GetActive << std::endl
                      << " -> FindString: " << FindString << std::endl
                      << " -> FindGameObject: " << FindGameObject << std::endl
                      << " -> AvatarPaimonAppear: " << AvatarPaimonAppear << std::endl;
            lastLogTick = currentTick;
        }
        return;
    }

    static ULONGLONG lastCheckTick = 0;
    static ULONGLONG checkInterval = 2000;

    if (currentTick - lastCheckTick < checkInterval) {
        return;
    }
    lastCheckTick = currentTick;
    lastLogTick = currentTick;

    SafeInvoke([&] {
        Il2CppString* paimonStr = FindString(GameStrings::PaimonPath);
        Il2CppString* diveStr = FindString(GameStrings::DivePaimonPath);
        Il2CppString* beydStr = FindString(GameStrings::BeydPaimonPath);

        void* paimonObj = paimonStr ? FindGameObject(paimonStr) : nullptr;
        void* diveObj = diveStr ? FindGameObject(diveStr) : nullptr;
        void* beydObj = beydStr ? FindGameObject(beydStr) : nullptr;

        std::cout << "[PaimonV2_Log] Objects -> Normal: " << paimonObj
                  << " | Dive: " << diveObj
                  << " | Beyd: " << beydObj << std::endl;

        if (!paimonObj && !diveObj && !beydObj) {
            std::cout << "[PaimonV2_Log] All objects NULL. Scene has no Paimon." << std::endl;
            return;
        }

        bool isPaimonActive = paimonObj && GetActive(paimonObj);
        bool isDiveActive = diveObj && GetActive(diveObj);
        bool isBeydActive = beydObj && GetActive(beydObj);

        std::cout << "[PaimonV2_Log] ActiveState -> Normal: " << isPaimonActive
                  << " | Dive: " << isDiveActive
                  << " | Beyd: " << isBeydActive << std::endl;

        if (isPaimonActive || isDiveActive || isBeydActive) {
            checkInterval = 5000;
            std::cout << "[PaimonV2_Log] A Paimon is currently active. Aborting awake." << std::endl;
            return;
        }

        checkInterval = 2000;

        void* globalActor = GetGlobalActor(g_ActorManagerInstance);
        std::cout << "[PaimonV2_Log] GlobalActor ptr: " << globalActor << std::endl;

        if (globalActor) {
            std::cout << "[PaimonV2_Log] Executing AvatarPaimonAppear..." << std::endl;
            AvatarPaimonAppear(globalActor, nullptr, true);
        } else {
            std::cout << "[PaimonV2_Log] ERROR: GlobalActor is NULL." << std::endl;
        }
    });
}
