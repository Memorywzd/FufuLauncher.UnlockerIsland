#pragma once
#include "../Core/SharedState.h"

void HandlePaimon();
bool CheckResistInBeyd(bool cache = true);
void WINAPI hk_ActorManagerCtor(void* pThis);
void WINAPI hk_SetUID(void* pThis, uint32_t uid);
void UpdatePaimonV2();
