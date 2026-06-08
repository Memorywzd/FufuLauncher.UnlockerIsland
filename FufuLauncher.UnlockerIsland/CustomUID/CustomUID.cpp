#include "CustomUID.h"
#include <string>

namespace CustomUIDFeature {
    SetText_t     g_oSetText    = nullptr;
    SetColor_t    g_oSetColor   = nullptr;
    SetFontSize_t g_oSetFontSize= nullptr;

    static FakeString  g_strUID      = {};
    static bool        g_ready       = false;

    static bool IsAllDigits(const wchar_t* s, int len) {
        for (int i = 0; i < len; i++) {
            if (s[i] < L'0' || s[i] > L'9') return false;
        }
        return len >= 5;
    }

    static bool HasUIDPrefix(Il2CppString_Custom* s) {
        return s->length > 5 &&
            s->chars[0] == L'U' && s->chars[1] == L'I' &&
            s->chars[2] == L'D' && s->chars[3] == L':' && s->chars[4] == L' ';
    }

    static bool IsUID(Il2CppString_Custom* str) {
        if (!str || str->length < 5 || str->length > 20) return false;
        if (IsAllDigits(str->chars, str->length)) return true;
        if (HasUIDPrefix(str) && IsAllDigits(str->chars + 5, str->length - 5)) return true;
        return false;
    }

    static void BuildFakeStrings(void* klass) {
        if (g_ready) return;

        std::string uidStr = Config::Get().custom_uid_str;

        int wLen = MultiByteToWideChar(CP_ACP, 0, uidStr.c_str(), -1, nullptr, 0);
        std::wstring wUidStr;
        if (wLen > 0) {
            wUidStr.resize(wLen - 1);
            MultiByteToWideChar(CP_ACP, 0, uidStr.c_str(), -1, &wUidStr[0], wLen);
        } else {
            wUidStr = L"999999999";
        }

        g_strUID.klass   = klass;
        g_strUID.monitor = nullptr;
        g_strUID.length  = (int)wUidStr.length();
        wcsncpy_s(g_strUID.chars, wUidStr.c_str(), _TRUNCATE);

        g_ready = true;
    }

    void __fastcall hk_SetText(void* self, Il2CppString_Custom* value, void* method) {
        if (value && IsUID(value)) {
            bool isTarget = false;
            auto getName = (tGetName)p_GetName.load();

            if (getName) {
                Il2CppString_Custom* compName = (Il2CppString_Custom*)getName(self);
                if (compName && compName->chars) {
                    if (wcsstr(compName->chars, L"TxtUID") != nullptr ||
                        wcsstr(compName->chars, L"UID") != nullptr) {
                        isTarget = true;
                    }
                }
            }

            if (isTarget) {
                BuildFakeStrings(value->klass);

                Il2CppString_Custom* rep = (Il2CppString_Custom*)&g_strUID;
                g_oSetText(self, rep, method);

                if (Config::Get().enable_custom_uid_color && g_oSetColor) {
                    UnityColor newColor = {
                        Config::Get().custom_uid_color_r,
                        Config::Get().custom_uid_color_g,
                        Config::Get().custom_uid_color_b,
                        Config::Get().custom_uid_color_a
                    };
                    g_oSetColor(self, newColor, nullptr);
                }

                return;
            }
        }

        g_oSetText(self, value, method);
    }
}
