// ===============================
//  Вспомогательные функции ESP/инициализация
//
//  Содержимое:
//   - Преобразования World→Screen
//   - GetBase(): подключение к процессу/модулю UE и получение матрицы
//   - Сохранение/загрузка конфигурации (/data/adb/zk_config.bin)
//   - DrawHealth: отрисовка полосы HP/ID
//
//  Замечания:
//   - Оффсеты UE вынесены в include/Tools/offsets.h
//   - Guard на контекст ImGui при применении масштабов шрифта
// ===============================
#include "../i18n.h"
#include "../ImGui/imgui.h"
#include "../../include/Tools/offsets.h"
#include <algorithm>
extern ImVec2 g_zkBasePos;
extern float g_ui_alpha;

void 物资函数(const function<string(const string&)> itemFunction, int drawIoIndex, int maxDistance, int fontSize = 30) {
string name;
name = itemFunction(wz.Name);
if (物资[drawIoIndex] &&!name.empty()) {
if (wz.Distance < maxDistance) {
    // Localize by class name if possible
    std::string loc = i18n::FromClassName(wz.Name);
    if (!loc.empty()) name = loc;
    name += "[";
    name += to_string((int)wz.Distance);
    name += (i18n::GetLanguage() == i18n::Lang::RU ? "м]" : "m]");
auto textSize = ImGui::CalcTextSize(name.c_str(), 0, fontSize);
ImGui::GetForegroundDrawList()->AddText(NULL, fontSize, { wz.ScreenPos.x - (textSize.x / 2), wz.ScreenPos.y }, 物资颜色[drawIoIndex], name.c_str());
}
}
}
void DrawLogo(ImTextureID ID,ImVec2 center, float size) {
ImGui::SetCursorPos({size*2, size*2});
ImGui::GetWindowDrawList()->AddImageRounded(ID,ImVec2(center.x - size, center.y - size), ImVec2(center.x + size, center.y + size), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), 999);
}
FTransform getBone(uintptr_t addr) {
FTransform transform;
driver->read(addr, reinterpret_cast<void*>(&transform), 4 * 11);
return transform;
}
void GetDistance(Vec3 Object, Vec3 Self, float *Distance) {
float DistanceX = pow(Object.x - Self.x, 2);
float DistanceY = pow(Object.y - Self.y, 2);
float DistanceZ = pow(Object.z - Self.z, 2);
*Distance = sqrt(DistanceX + DistanceY + DistanceZ) * 0.01f;
}
Vec2 rotateCoord(float angle, float objRadar_x, float objRadar_y) {
Vec2 radarCoordinate;
float s = sin(angle * PI / 180);
float c = cos(angle * PI / 180);
radarCoordinate.x = objRadar_x * c + objRadar_y * s;
radarCoordinate.y = -objRadar_x * s + objRadar_y * c;
return radarCoordinate;
}
void GetClassName(char *Name, long int address, int id) {
long int ClassNameadd = driver->read<uintptr_t>(address + (id / 0x4000) * 0x8);
long int ClassNameaddr = driver->read<uintptr_t>(ClassNameadd + (id % 0x4000) * 0x8);
driver->read(ClassNameaddr + 0xC, Name, 64);
}
char* 获取类名(long addr) {
int id = driver->read<int>(addr + 0x18);
long ye = driver->read<uintptr_t>(fanhua.Gname + (id / 0x4000) * 8);
long xu = driver->read<uintptr_t>(ye + (id % 0x4000) * 8);
static char Item[256];
driver->read(xu + 0xc, Item, sizeof(Item));
return Item;
}
void getUTF8(UTF8 * buf, long namepy) {
UTF16 buf16[16] = { 0 };
driver->read(namepy, buf16, 28);
UTF16 *pTempUTF16 = buf16;
UTF8 *pTempUTF8 = buf;
UTF8 *pUTF8End = pTempUTF8 + 32;
while (pTempUTF16 < pTempUTF16 + 28) {
if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End) {
*pTempUTF8++ = (UTF8) * pTempUTF16;
} else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End) {
*pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
} else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End) {
*pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
*pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
} else {
break;
}
pTempUTF16++;
}
}
Vec2 WorldIsScreen(Vec3 obj, float ViewW) {
float x = screen_x/2 + (fanhua.matrix[0] * obj.x + fanhua.matrix[4] * obj.y + fanhua.matrix[8] * obj.z + fanhua.matrix[12]) / ViewW * screen_x/2;
float y = screen_y/2 - (fanhua.matrix[1] * obj.x + fanhua.matrix[5] * obj.y + fanhua.matrix[9] * obj.z + fanhua.matrix[13]) / ViewW * screen_y/2;
return Vec2(x, y);
}
Vec2 WorldToScreen(Vec3 obj, float a[16], float b) {
Vec2 bscreen;
float camear = fanhua.matrix[3] * obj.x + fanhua.matrix[7] * obj.y + fanhua.matrix[11] * obj.z + fanhua.matrix[15];
if (camear <= 0.01f) {
    bscreen.x = 0;
    bscreen.y = 0;
    return bscreen;
}
bscreen.x = screen_x/2 + (fanhua.matrix[0] * obj.x + fanhua.matrix[4] * obj.y + fanhua.matrix[8] * obj.z + fanhua.matrix[12]) / camear * screen_x/2;
bscreen.y = screen_y/2 - (fanhua.matrix[1] * obj.x + fanhua.matrix[5] * obj.y + fanhua.matrix[9] * obj.z + fanhua.matrix[13]) / camear * screen_y/2;
return bscreen;
}
void WorldToScreen(Vec2 *bscreen, float *camea, float *w, Vec3 obj) { 
float camear = fanhua.matrix[3] * obj.x + fanhua.matrix[7] * obj.y + fanhua.matrix[11] * obj.z + fanhua.matrix[15];
*camea = camear;
if (camear <= 0.01f) {
    bscreen->x = 0;
    bscreen->y = 0;
    *w = 0;
    return;
}
bscreen->x = screen_x/2 + (fanhua.matrix[0] * obj.x + fanhua.matrix[4] * obj.y + fanhua.matrix[8] * obj.z + fanhua.matrix[12]) / camear * screen_x/2;
bscreen->y = screen_y/2 - (fanhua.matrix[1] * obj.x + fanhua.matrix[5] * obj.y + fanhua.matrix[9] * obj.z + fanhua.matrix[13]) / camear * screen_y/2;
float bscreenZ = screen_y/2 - (fanhua.matrix[1] * obj.x + fanhua.matrix[5] * obj.y + fanhua.matrix[9] * (obj.z + 165) + fanhua.matrix[13]) / camear * screen_y/2;
float bscreenz = bscreen->y - bscreenZ;
*w = (bscreen->y - bscreenZ) / 2;
}
void DrawBone(ImVec2 start, ImVec2 end) {
if (!isnan(start.x) && !isnan(start.y) && !isnan(end.x) && !isnan(end.x)) {
double dx = start.x - end.x;
double dy = start.y - end.y;
if (sqrt(dx * dx + dy * dy) > 100)
return;
if (fanhua.IsBot) {
ImGui::GetForegroundDrawList()->AddLine(start, end, 绘制颜色[8], 绘制粗细[5]);
} else {
ImGui::GetForegroundDrawList()->AddLine(start, end, 绘制颜色[7], 绘制粗细[6]);
}
}
}
int GetBase() {
static bool initialized = false;
static int result = 0;
if (initialized) {
return result;
}
// Try known package names in order
const char* pkgs[] = {
    "com.tencent.ig",
    "com.pubg.krmobile",
    "com.vng.pubgmobile",
    "com.rekoo.pubgm",
    "com.tencent.tmgp.pubgmhd"
};
pid_t pid_found = -1;
for (size_t i=0;i<sizeof(pkgs)/sizeof(pkgs[0]);++i) {
    pid_t p = driver->GetPid((char*)pkgs[i]);
    if (p > 0 && p < 100000) { pid_found = p; break; }
}
if (pid_found <= 0) {
    return 0;
}
driver->initialize(pid_found);
fanhua.pid = pid_found;

// Get libUE4 base using driver (handles RT IOCTL or HOOK maps internally)
fanhua.libUE4 = driver->getModuleBase("libUE4.so");
if (!fanhua.libUE4) {
    return 0;
}
memset(fanhua.matrix, 0, sizeof(fanhua.matrix));
// Standard matrix chain for current versions (see offsets.h)
using namespace zk_offsets;
uintptr_t world = driver->read<uintptr_t>(fanhua.libUE4 + kUWorld);
if (!world) { return 0; }
uintptr_t world_c0 = driver->read<uintptr_t>(world + kWorldToMat_C0);
if (!world_c0) { return 0; }
fanhua.Matrixs = world_c0 + kMat_Proj;

initialized = true;
result = 1;
return result;
}
FILE *numSave = nullptr;
void NumIoSave(const char * /*name*/) {
    if (numSave == nullptr) {
        std::string SaveFile = "/data/adb/zk_config.bin";
        numSave = fopen(SaveFile.c_str(), "wb+");
    }
    if (!numSave) return;
    fseek(numSave, 0, SEEK_SET);
    // Core toggles and numbers
    fwrite(DrawIo, sizeof(bool) * 50, 1, numSave);
    fwrite(NumIo, sizeof(float) * 50, 1, numSave);
    fwrite(draw_style, sizeof(float) * 20, 1, numSave);
    fwrite(&绘制粗细, sizeof(float) * 16, 1, numSave);
    fwrite(&绘制颜色, sizeof(ImColor) * 16, 1, numSave);
    fwrite(&物资, sizeof(bool) * 50, 1, numSave);
    fwrite(&物资颜色, sizeof(ImColor) * 50, 1, numSave);
    // UI/animation extras
    fwrite(&playerAnim, sizeof(NumberAnimation), 1, numSave);
    fwrite(&botAnim, sizeof(NumberAnimation), 1, numSave);
    fwrite(&currentAnimationMode, sizeof(AnimationMode), 1, numSave);
    fwrite(&showAnimationSettings, sizeof(bool), 1, numSave);
    fwrite(&globalFontScale, sizeof(float), 1, numSave);
    fwrite(&录屏时长, sizeof(float), 1, numSave);
    fwrite(&speed, sizeof(float), 1, numSave);
    // Additional persisted settings
    int lang = (int)i18n::GetLanguage();
    fwrite(&lang, sizeof(int), 1, numSave);
    fwrite(&g_zkBasePos, sizeof(ImVec2), 1, numSave);
    fwrite(&g_ui_alpha, sizeof(float), 1, numSave);
    fflush(numSave);
    fsync(fileno(numSave));
}
void NumIoLoad(const char * /*name*/) {
    if (numSave == nullptr) {
        std::string SaveFile = "/data/adb/zk_config.bin";
        numSave = fopen(SaveFile.c_str(), "rb+");
    }
    if (numSave != nullptr) {
        fseek(numSave, 0, SEEK_SET);
        fread(DrawIo, sizeof(bool) * 50, 1, numSave);
        fread(NumIo, sizeof(float) * 50, 1, numSave);
        fread(draw_style, sizeof(float) * 20, 1, numSave);
        fread(&绘制粗细, sizeof(float) * 16, 1, numSave);
        fread(&绘制颜色, sizeof(ImColor) * 16, 1, numSave);
        fread(&物资, sizeof(bool) * 50, 1, numSave);
        fread(&物资颜色, sizeof(ImColor) * 50, 1, numSave);
        fread(&playerAnim, sizeof(NumberAnimation), 1, numSave);
        fread(&botAnim, sizeof(NumberAnimation), 1, numSave);
        fread(&currentAnimationMode, sizeof(AnimationMode), 1, numSave);
        fread(&showAnimationSettings, sizeof(bool), 1, numSave);
        fread(&globalFontScale, sizeof(float), 1, numSave);
        fread(&录屏时长, sizeof(float), 1, numSave);
        fread(&speed, sizeof(float), 1, numSave);
        // Additional persisted settings
        int lang = (int)i18n::GetLanguage();
        fread(&lang, sizeof(int), 1, numSave);
        i18n::SetLanguage((i18n::Lang)lang);
        fread(&g_zkBasePos, sizeof(ImVec2), 1, numSave);
        fread(&g_ui_alpha, sizeof(float), 1, numSave);
        // Apply font scale only if ImGui context is ready
        if (ImGui::GetCurrentContext() != nullptr) {
            globalFontSettings.SetScale(globalFontScale);
            globalFontSettings.ApplyScale(ImGui::GetIO());
        }
    } else {
        CleanData();
    }
}
static ImTextureID GetWeaponTexture()
{
    if (!DrawIo[10]) return (ImTextureID)0;
    const int rawWeaponId = GetWeapon(fanhua.scwq);
    int iconIndex = (rawWeaponId >= 10) ? rawWeaponId / 10 : rawWeaponId;
    constexpr int kWeaponTextureCount = sizeof(weapon) / sizeof(weapon[0]);
    if (iconIndex < 0 || iconIndex >= kWeaponTextureCount) {
        iconIndex = 0;
    }
    ImTextureID tex = weapon[0];
    if (weapon[iconIndex] != (ImTextureID)0) {
        tex = weapon[iconIndex];
    } else if (weapon[1] != (ImTextureID)0) {
        tex = weapon[1];
    }
    return tex;
}

void DrawHealth(Vec2 box, float w, float iHealth, uint32_t TeamID, char* name) {
    if (!DrawIo[6]) return;
    std::string lname = name ? std::string(name) : std::string("");
    if (lname == "人机") lname = i18n::TL("Bot");
    if (TeamID <= 0 || iHealth <= 0 || lname.empty()) return;

    const float clampedHealth = std::clamp(iHealth, 0.0f, 100.0f);
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    ImDrawList* fg = ImGui::GetForegroundDrawList();

    const float x = box.x;
    const int style = static_cast<int>(draw_style[1]);
    const ImTextureID weaponTex = GetWeaponTexture();

    auto drawWeaponIcon = [&](ImVec2 min, ImVec2 max, float rounding, ImU32 tint = IM_COL32_WHITE) {
        if (!DrawIo[10] || weaponTex == (ImTextureID)0) return;
        bg->AddRectFilled(min - ImVec2(2, 2), max + ImVec2(2, 2), ImColor(0, 0, 0, 90), rounding + 2.0f);
        fg->AddImageRounded(weaponTex, min, max, ImVec2(0, 0), ImVec2(1, 1), tint, rounding);
    };

    switch (style) {
        case 0: {
            float y = box.y - 30.0f;
            char combinedText[96];
            snprintf(combinedText, sizeof(combinedText), "%d.%s", TeamID, lname.c_str());
            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(20, FLT_MAX, -1, combinedText);
            DrawTf.绘制字体描边(20, x - (textSize.x / 2.0f), y - textSize.y - 5.0f,
                            ImVec4{255, 255, 255, 255}, combinedText);

            ImVec2 barMin(x - 37.0f, y - 2.0f);
            ImVec2 barMax(x + 37.0f, y + 8.0f);
            bg->AddRect(barMin, barMax, ImColor(173, 216, 230, 255), 0.0f, 0, 1.5f);
            if (clampedHealth > 0.0f) {
                float fill = (clampedHealth * 0.7f);
                ImVec2 fillEnd(x - 35.0f + fill, y + 3.0f);
                bg->AddLine(ImVec2(x - 35.0f, y + 3.0f), fillEnd, ImColor(255, 255, 255, 255), 6.0f);
            }
            for (int i = 0; i < 5; ++i) {
                float left = x - 35.0f + i * 14.0f;
                bg->AddRect(ImVec2(left, y), ImVec2(left + 14.0f, y + 6.0f), ImColor(0, 0, 0, 255), 0.0f, 0, 1.5f);
            }

            if (DrawIo[10] && weaponTex != (ImTextureID)0) {
                ImVec2 iconMin(x - 42.0f, y - 52.0f);
                ImVec2 iconMax(x + 42.0f, y - 4.0f);
                fg->AddImage(weaponTex, iconMin, iconMax);
            }
            break;
        }
        case 1: {
            const float barWidth = std::min(220.0f, std::max(140.0f, w * 0.8f));
            const float barHeight = 12.0f;
            const float barTop = box.y - 42.0f;
            ImVec2 barMin(x - barWidth * 0.5f, barTop);
            ImVec2 barMax(x + barWidth * 0.5f, barTop + barHeight);

            ImColor bgCol(0, 90, 0, 120);
            ImColor fillCol(0, 200, 0, 200);
            bg->AddRectFilled(barMin, barMax, bgCol, barHeight / 2.0f, ImDrawFlags_RoundCornersAll);
            const float fillWidth = barWidth * (clampedHealth / 100.0f);
            ImVec2 fillMax(barMin.x + fillWidth, barMax.y);
            bg->AddRectFilled(barMin, fillMax, fillCol, barHeight / 2.0f,
                              (fillWidth >= barWidth) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);

            char info[96];
            snprintf(info, sizeof(info), "%d • %s", TeamID, lname.c_str());
            ImVec2 infoSize = ImGui::GetFont()->CalcTextSizeA(18, barWidth, -1, info);
            float textX = barMin.x + (barWidth - infoSize.x) * 0.5f;
            float textY = barMin.y + (barHeight - infoSize.y) * 0.5f - 1.0f;
            DrawTf.绘制字体描边(18, textX, textY, ImVec4{255, 255, 255, 255}, info);

            if (DrawIo[10] && weaponTex != (ImTextureID)0) {
                ImVec2 iconMin(x - 34.0f, barMin.y - 46.0f);
                ImVec2 iconMax(x + 34.0f, barMin.y - 4.0f);
                drawWeaponIcon(iconMin, iconMax, 10.0f);
            }
            break;
        }
        case 2: {
            const float cardWidth = std::min(250.0f, std::max(170.0f, w * 0.85f));
            const float cardHeight = 60.0f;
            const float cardTop = box.y - 70.0f;
            ImVec2 cardMin(x - cardWidth * 0.5f, cardTop);
            ImVec2 cardMax(x + cardWidth * 0.5f, cardTop + cardHeight);

            bg->AddRectFilled(cardMin + ImVec2(0, 4), cardMax + ImVec2(0, 4), ImColor(0, 0, 0, 45), 20.0f);
            bg->AddRectFilled(cardMin, cardMax, ImColor(248, 250, 255, 235), 20.0f);
            bg->AddRect(cardMin, cardMax, ImColor(220, 225, 235, 255), 20.0f, 0, 1.4f);

            float iconSize = 36.0f;
            ImVec2 iconMin(cardMin.x + 18.0f, cardMin.y + (cardHeight - iconSize) * 0.5f - 2.0f);
            ImVec2 iconMax(iconMin.x + iconSize, iconMin.y + iconSize);
            if (DrawIo[10] && weaponTex != (ImTextureID)0) {
                bg->AddRectFilled(iconMin - ImVec2(6, 6), iconMax + ImVec2(6, 6), ImColor(230, 235, 245, 255), 14.0f);
                drawWeaponIcon(iconMin, iconMax, 12.0f, IM_COL32_WHITE);
            }

            const float textLeft = iconMax.x + 12.0f;
            const float textRight = cardMax.x - 16.0f;
            const float maxTitleWidth = std::max(0.0f, textRight - textLeft);
            fg->AddText(ImGui::GetFont(), 21, ImVec2(textLeft, cardMin.y + 12.0f), ImColor(35, 38, 45, 255), lname.c_str());

            char subtitle[64];
            snprintf(subtitle, sizeof(subtitle), "T%d   %dm", TeamID, static_cast<int>(fanhua.Distance));
            fg->AddText(ImGui::GetFont(), 18, ImVec2(textLeft, cardMin.y + 32.0f), ImColor(95, 100, 120, 255), subtitle);

            const float barWidth = maxTitleWidth;
            const float barHeight = 6.0f;
            ImVec2 barMin(textLeft, cardMax.y - 16.0f);
            ImVec2 barMax(textLeft + barWidth, barMin.y + barHeight);
            bg->AddRectFilled(barMin, barMax, ImColor(210, 214, 220, 255), barHeight / 2.0f);
            const float fillWidth = barWidth * (clampedHealth / 100.0f);
            ImVec2 fillMax(barMin.x + fillWidth, barMax.y);
            bg->AddRectFilled(barMin, fillMax, ImColor(52, 199, 89, 255), barHeight / 2.0f,
                              (fillWidth >= barWidth) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);
            break;
        }
        default:
            break;
    }
}
