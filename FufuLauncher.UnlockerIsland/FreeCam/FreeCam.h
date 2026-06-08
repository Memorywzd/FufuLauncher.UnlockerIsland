#pragma once
#include "../Core/SharedState.h"

struct SafeFogBuffer {
    __declspec(align(16)) uint8_t data[64];
    uint8_t padding[192];
};

void InitCameraMatrixAddress();
bool TryGetCameraMatrix(float& out_rightX, float& out_rightY, float& out_rightZ,
                        float& out_forwardX, float& out_forwardY, float& out_forwardZ);
void UpdateFreeCamPhysics();
void WINAPI hk_ClockPageOk(void* pThis);
void __fastcall hk_SetPos(void* pTransform, Vector3* pPos);
