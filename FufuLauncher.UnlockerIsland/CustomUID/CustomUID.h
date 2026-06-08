#pragma once
#include "../Core/SharedState.h"
#include "../Config/Config.h"
#include <string>

namespace CustomUIDFeature {
    struct Il2CppString_Custom {
        void* klass;
        void* monitor;
        int length;
        wchar_t chars[1];
    };

    struct FakeString {
        void* klass;
        void* monitor;
        int length;
        wchar_t chars[64];
    };

    struct UnityColor {
        float r;
        float g;
        float b;
        float a;
    };

    typedef void (__fastcall *SetText_t)(void*, Il2CppString_Custom*, void*);
    typedef void (__fastcall *SetColor_t)(void*, UnityColor, void*);
    typedef void (__fastcall *SetFontSize_t)(void*, int, void*);

    extern SetText_t     g_oSetText;
    extern SetColor_t    g_oSetColor;
    extern SetFontSize_t g_oSetFontSize;

    void __fastcall hk_SetText(void* self, Il2CppString_Custom* value, void* method);
}
