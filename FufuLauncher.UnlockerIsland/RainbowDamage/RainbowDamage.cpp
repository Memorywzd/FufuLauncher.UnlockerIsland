#include "RainbowDamage.h"

namespace RainbowDamageFeature {
    static Color g_palette[] = {
        {0.2f, 0.9f, 0.1f, 1.0f},
        {1.0f, 0.3f, 0.3f, 1.0f},
        {0.3f, 0.5f, 1.0f, 1.0f},
        {1.0f, 0.85f, 0.1f, 1.0f},
        {0.8f, 0.2f, 1.0f, 1.0f},
        {0.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 0.5f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
    };
    static constexpr int PALETTE_COUNT = sizeof(g_palette) / sizeof(Color);
    static volatile int g_colorIdx = 0;

    FnGetColorList g_oGetColorA = nullptr;
    FnGetColorArr  g_oGetColorB = nullptr;
    FnGetColorIdx  g_oGetColor1 = nullptr;
    FnGetColorIdx  g_oGetColor2 = nullptr;
    FnGetColorIdx  g_oGetColor3 = nullptr;
    FnGetColorIdx  g_oGetColor4 = nullptr;

    static Color GetTargetColor() {
        if (Config::Get().rainbow_damage_mode == 1) {
            int fixedIdx = Config::Get().rainbow_fixed_color_idx % PALETTE_COUNT;
            return g_palette[fixedIdx];
        }
        return g_palette[g_colorIdx];
    }

    void __fastcall HookGetColorA(Color* ret, void* self, void* list, int idx, void* method) {
        g_oGetColorA(ret, self, list, idx, method);
        *ret = GetTargetColor();
    }
    void __fastcall HookGetColorB(Color* ret, void* self, void* arr, int idx, void* method) {
        g_oGetColorB(ret, self, arr, idx, method);
        *ret = GetTargetColor();
    }
    void __fastcall HookGetColor1(Color* ret, void* self, int idx, void* method) {
        g_oGetColor1(ret, self, idx, method);
        *ret = GetTargetColor();
    }
    void __fastcall HookGetColor2(Color* ret, void* self, int idx, void* method) {
        g_oGetColor2(ret, self, idx, method);
    }
    void __fastcall HookGetColor3(Color* ret, void* self, int idx, void* method) {
        g_oGetColor3(ret, self, idx, method);
        *ret = GetTargetColor();
    }
    void __fastcall HookGetColor4(Color* ret, void* self, int idx, void* method) {
        g_oGetColor4(ret, self, idx, method);
    }

    DWORD WINAPI ColorCycleThread(LPVOID) {
        while (true) {
            Sleep(2000);
            if (Config::Get().rainbow_damage_mode == 0) {
                g_colorIdx = (g_colorIdx + 1) % PALETTE_COUNT;
            }
        }
        return 0;
    }
}
