
// ===============================
//  UI (ImGui) — вкладки и панель управления
//
//  Содержимое:
//   - Функции вкладок: 主页/绘制/自瞄/设置/物资 等
//   - Стили/анимации/скролл (TouchScroll)
//   - Набор вспомогательных виджетов (Toggle/whitecloud)
//
//  Задачи рефакторинга (постепенно):
//   - Перевести все видимые строки через i18n::TL
//   - Убрать оставшиеся китайские лейблы/комментарии
//   - Изолировать скролл каждого Child окна
// ===============================
#ifndef __IMGUI_WHITECLOUD_H
#define __IMGUI_WHITECLOUD_H
#include <string>
#include <iostream>
#include <cmath>
#include <ctime>
#include <map>
#include <algorithm>
#include <linux/input.h>
#include "imgui.h"
#include <sstream>
#include "i18n.h"

using std::string;

ImColor tsb_color, tsb_color_end, 鸡鸡鸡;
float speed = 1.5f;
string tsb_but = "待选择";
float 透明度 = 1000.0f;
bool 开关 = true;
bool window = true, windows = true;
bool 增加;
bool 减少;
int Tab, 调色板Tab = -1;
float window_x, window_y, aevrage_now, aevrage_all, 设置_Tab, 设置_调色板_size, 灵动岛左, 灵动岛右;
float tab0, tab1, tab2, tab3, tab4, tab5, tab6, tab_0, tab_1, tab_2, tab_3, tab_4;
float 灵动岛大小 = 1.0f;
float homepage, homepages, setup, setups;
static float yj, yj1;
float 圆角;

void 开启();
void 关闭();

void 主页_1();
void 主页_2();

void 绘制_1();
void 绘制_2();
void 绘制_3();
void 绘制_4();

void 自瞄_1();
void 自瞄_2();
void 自瞄_3();
void 自瞄_4();

void 设置页面();
void 设置_0();
void 设置_1();
void 设置_2();
void 设置_3();
void 设置_4();
void 设置_5();

void 其他_1();
void 其他_2();
void 其他_3();
void 其他_4();

void 调节_1();
void 展示_1();
void 物资_1();
void 灵动岛();

int 音量键数据();
void 音量键隐藏();




struct wc {
int Tab;
float beg, end;
float 动画, 动画目前, 动画目标, 动画增加速度, 动画减少速度, 动画最小值, 动画展开百分比;
bool 动画开关, 显示内容;
ImColor color;

};


map<string, wc> wh;
extern map<string, ImColor> Styles;
wc& 位置x = wh["位置_x"];
wc& 位置y = wh["位置_y"];
wc& 窗口x = wh["window_x"];
wc& 窗口y = wh["window_y"];
wc& 设置 = wh["设置"];
wc& 调色板 = wh["调色板"];
wc& 调色板页数 = wh["调色板页数"];
wc& 动画 = wh["动画"];
wc& 圆角值 = wh["圆角值"];

wc& logo = wh["logo"];
wc& 字体大小 = wh["字体大小"];
wc& 字体位置 = wh["字体位置"];
wc& 人数位置 = wh["人数位置"];
wc& 灵动岛宽度 = wh["灵动岛宽度"];
wc& 灵动岛高度s = wh["灵动岛高度s"];
wc& 灵动岛高度x = wh["灵动岛高度x"];
wc& 圆球 = wh["圆球"];


ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
ImVec2 operator*(const ImVec2& a, const float& b) { return ImVec2(a.x * b, a.y * b); }
ImVec2 operator/(const ImVec2& a, const float& b) { return ImVec2(a.x / b, a.y / b); }
ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); } 
ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); } 
ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); } 
ImVec4 operator/(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(rhs.x != 0 ? lhs.x / rhs.x : 0, rhs.y != 0 ? lhs.y / rhs.y : 0, rhs.z != 0 ? lhs.z / rhs.z : 0, rhs.w != 0 ? lhs.w / rhs.w : 0); } 
ImVec4 operator+=(ImVec4& lhs, const ImVec4& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w; return lhs; }
ImColor operator+(const ImColor& lhs, const ImColor& rhs) { ImVec4 result = lhs.Value + rhs.Value; return ImColor(result); } 
ImColor operator-(const ImColor& lhs, const ImColor& rhs) { ImVec4 result = lhs.Value - rhs.Value; return ImColor(result); } 
ImColor operator*(const ImColor& lhs, const ImColor& rhs) { ImVec4 result = lhs.Value * rhs.Value; return ImColor(result); }
ImColor operator/(const ImColor& lhs, const ImColor& rhs) { ImVec4 result = lhs.Value / rhs.Value; return ImColor(result); }



void Lerp(float& a, const float& b, float t) {
a += (b - a) * (t * speed);
}





ImVec4 set_color(float Frequency, float TimeOffset = 0.5f, float alpha = 1.0f) {
    static float lastTime = 0.0f;
    static ImVec4 cachedColor;
    float currentTime = ImGui::GetTime();
   
    if (fabs(currentTime - lastTime) > 0.1f) {
        float r = sinf(Frequency * currentTime + 0.0f + TimeOffset) * 127.0f + 128.0f;
        float g = sinf(Frequency * currentTime + 2.0f + TimeOffset) * 127.0f + 128.0f;
        float b = sinf(Frequency * currentTime + 4.0f + TimeOffset) * 127.0f + 128.0f;
        cachedColor = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, alpha);
        lastTime = currentTime;
    }
    return cachedColor;
}

// ================== General Controls (Localization / FPS / Theme) ==================
extern float NumIo[50];

namespace ui_general {
    enum class Theme { Daily = 0, Nightly = 1 };
    static Theme g_theme = Theme::Daily;

    inline void ApplyTheme(Theme th) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        if (th == Theme::Nightly) {
            colors[ImGuiCol_Text]           = ImVec4(0.95f, 0.95f, 0.98f, 1.00f);
            colors[ImGuiCol_WindowBg]       = ImVec4(48/255.0f, 3/255.0f, 102/255.0f, 1.00f);
            colors[ImGuiCol_PopupBg]        = ImVec4(0.12f, 0.12f, 0.22f, 0.94f);
            colors[ImGuiCol_Border]         = ImVec4(0.40f, 0.40f, 0.48f, 0.31f);
            colors[ImGuiCol_FrameBg]        = ImVec4(0.24f, 0.18f, 0.33f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.32f, 0.22f, 0.45f, 1.00f);
            colors[ImGuiCol_FrameBgActive]  = ImVec4(0.42f, 0.28f, 0.58f, 1.00f);
            colors[ImGuiCol_Button]         = ImVec4(0.34f, 0.22f, 0.52f, 1.00f);
            colors[ImGuiCol_ButtonHovered]  = ImVec4(0.42f, 0.28f, 0.60f, 1.00f);
            colors[ImGuiCol_ButtonActive]   = ImVec4(0.50f, 0.34f, 0.72f, 1.00f);
            colors[ImGuiCol_SliderGrab]     = ImVec4(0.80f, 0.80f, 0.90f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]=ImVec4(0.95f, 0.95f, 1.00f, 1.00f);
            style.FrameRounding = 6.0f;
            style.GrabRounding  = 6.0f;
        } else {
            colors[ImGuiCol_Text]           = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
            colors[ImGuiCol_WindowBg]       = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
            colors[ImGuiCol_PopupBg]        = ImVec4(0.98f, 0.98f, 0.98f, 0.98f);
            colors[ImGuiCol_Border]         = ImVec4(0.70f, 0.70f, 0.75f, 0.30f);
            colors[ImGuiCol_FrameBg]        = ImVec4(0.90f, 0.90f, 0.94f, 1.00f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.86f, 0.86f, 0.90f, 1.00f);
            colors[ImGuiCol_FrameBgActive]  = ImVec4(0.80f, 0.80f, 0.88f, 1.00f);
            colors[ImGuiCol_Button]         = ImVec4(0.90f, 0.90f, 0.94f, 1.00f);
            colors[ImGuiCol_ButtonHovered]  = ImVec4(0.86f, 0.86f, 0.90f, 1.00f);
            colors[ImGuiCol_ButtonActive]   = ImVec4(0.80f, 0.80f, 0.88f, 1.00f);
            colors[ImGuiCol_SliderGrab]     = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]=ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
            style.FrameRounding = 6.0f;
            style.GrabRounding  = 6.0f;
        }
    }

    inline bool SliderFloatWithDot(const char* label, float* v, float v_min, float v_max, const char* fmt = "%.0f", ImGuiSliderFlags flags = 0) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
        float avail = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(avail);
        bool changed = ImGui::SliderFloat("##slider", v, v_min, v_max, fmt, flags);
        ImRect r(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float t = (*v - v_min) / (v_max - v_min);
        t = std::clamp(t, 0.0f, 1.0f);
        float x = r.Min.x + t * (r.GetWidth());
        float y = (r.Min.y + r.Max.y) * 0.5f;
        float base = 5.0f;
        float radius = ImGui::IsItemActive() ? base * 1.8f : (ImGui::IsItemHovered() ? base * 1.3f : base);
        dl->AddCircleFilled(ImVec2(x, y), radius, ImGui::GetColorU32(ImGuiCol_SliderGrabActive));
        ImGui::PopItemWidth();
        return changed;
    }

inline void Draw_GeneralSettings() {
        ApplyTheme(g_theme);
        ImGui::SetNextWindowSize(ImVec2(360, 220), ImGuiCond_FirstUseEver);
        ImGui::Begin(i18n::TL("Settings").c_str(), nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::TextUnformatted(i18n::TL("Language").c_str());
        ImGui::SameLine();
        bool en = (i18n::GetLanguage() == i18n::Lang::EN);
        if (ImGui::RadioButton(i18n::TL("English").c_str(), en)) i18n::SetLanguage(i18n::Lang::EN);
        ImGui::SameLine();
        bool ru = (i18n::GetLanguage() == i18n::Lang::RU);
        if (ImGui::RadioButton(i18n::TL("Russian").c_str(), ru)) i18n::SetLanguage(i18n::Lang::RU);

        ImGui::Separator();
        ImGui::TextUnformatted(i18n::TL("Theme").c_str());
        ImGui::SameLine();
        int theme_idx = (g_theme == Theme::Daily) ? 0 : 1;
        if (ImGui::RadioButton(i18n::TL("Daily").c_str(), theme_idx == 0)) g_theme = Theme::Daily;
        ImGui::SameLine();
        if (ImGui::RadioButton(i18n::TL("Nightly").c_str(), theme_idx == 1)) g_theme = Theme::Nightly;

        ImGui::Separator();
        float min_fps = 30.0f, max_fps = 240.0f;
        NumIo[12] = std::clamp(NumIo[12], min_fps, max_fps);
        SliderFloatWithDot(i18n::TL("Target FPS").c_str(), &NumIo[12], min_fps, max_fps, "%.0f");

        ImGui::End();
    }

inline void Draw_GeneralSettingsEmbedded() {
    ApplyTheme(ui_general::g_theme);
    ImGui::Separator();
    ImGui::TextUnformatted(i18n::TL("Language").c_str());
    ImGui::SameLine();
    bool en = (i18n::GetLanguage() == i18n::Lang::EN);
    if (ImGui::RadioButton(i18n::TL("English").c_str(), en)) i18n::SetLanguage(i18n::Lang::EN);
    ImGui::SameLine();
    bool ru = (i18n::GetLanguage() == i18n::Lang::RU);
    if (ImGui::RadioButton(i18n::TL("Russian").c_str(), ru)) i18n::SetLanguage(i18n::Lang::RU);

    ImGui::Separator();
    ImGui::TextUnformatted(i18n::TL("Theme").c_str());
    ImGui::SameLine();
    int theme_idx = (ui_general::g_theme == ui_general::Theme::Daily) ? 0 : 1;
    if (ImGui::RadioButton(i18n::TL("Daily").c_str(), theme_idx == 0)) ui_general::g_theme = ui_general::Theme::Daily;
    ImGui::SameLine();
    if (ImGui::RadioButton(i18n::TL("Nightly").c_str(), theme_idx == 1)) ui_general::g_theme = ui_general::Theme::Nightly;

    ImGui::Separator();
    float min_fps = 30.0f, max_fps = 240.0f;
    NumIo[12] = std::clamp(NumIo[12], min_fps, max_fps);
    ui_general::SliderFloatWithDot(i18n::TL("Target FPS").c_str(), &NumIo[12], min_fps, max_fps, "%.0f");
}
}

inline void Draw_GeneralSettings() { ui_general::Draw_GeneralSettings(); }

bool state(bool &state) {
static ImVec2 png_pos;
if (ImGui::IsItemActive()) {
if (!state) {
state = true;
png_pos = ImGui::GetWindowPos();
return false;
}
} else if (state) {
state = false;
if (png_pos.x == ImGui::GetWindowPos().x && ImGui::GetWindowPos().y == png_pos.y) {
return true;
}
}
return false;
}

void 渐变字(const string& text, ImColor colorStart, ImColor colorEnd, float size = 60, bool 居中 = true) {
ImVec2 textsize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, -1.0f, text.c_str(), text.c_str() + text.length());
size_t text_length = text.length();
ImColor current_color;
ImDrawList* draw_list = ImGui::GetWindowDrawList();

if (居中)
ImGui::SetCursorPosX( (ImGui::GetWindowWidth() - textsize.x) * 0.5f ); 
ImVec2 pos = ImGui::GetCursorScreenPos();

static const float flow_speed = 0.3f;
float flow_offset = fmod(ImGui::GetTime() * flow_speed, 1.0f);

for (size_t i = 0; i < text_length; ++i) {
float t = static_cast<float>(i) / (text_length - 1);
t = fmod(t - flow_offset, 1.0f); 

if (t < 0) t += 1.0f;

if (t < 0.5f) {
float t_normalized = t * 2.0f; 
current_color = ImColor(
colorStart.Value.x + (colorEnd.Value.x - colorStart.Value.x) * t_normalized,
colorStart.Value.y + (colorEnd.Value.y - colorStart.Value.y) * t_normalized,
colorStart.Value.z + (colorEnd.Value.z - colorStart.Value.z) * t_normalized,
colorStart.Value.w + (colorEnd.Value.w - colorStart.Value.w) * t_normalized
);
} else {
float t_normalized = (t - 0.5f) * 2.0f; 
current_color = ImColor(
colorEnd.Value.x + (colorStart.Value.x - colorEnd.Value.x) * t_normalized,
colorEnd.Value.y + (colorStart.Value.y - colorEnd.Value.y) * t_normalized,
colorEnd.Value.z + (colorStart.Value.z - colorEnd.Value.z) * t_normalized,
colorEnd.Value.w + (colorStart.Value.w - colorEnd.Value.w) * t_normalized
);
}

size_t charLength = 0;
unsigned char lead = text[i]; 

if (lead < 0x80) {
charLength = 1;
} else if ((lead >> 5) == 0x6) {
charLength = 2;
} else if ((lead >> 4) == 0xE) {
charLength = 3;
} else if ((lead >> 3) == 0x1E) {
charLength = 4;
}

ImVec2 next_pos = pos;
next_pos.x += ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, -1.0f, &text[i], &text[i] + charLength).x;

ImFont* my_custom_font = ImGui::GetIO().Fonts->Fonts.back();
draw_list->AddText(my_custom_font, size, pos, current_color, &text[i], &text[i] + charLength);

pos = next_pos;
}
ImGui::SameLine();
ImGui::Text(" ");
ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textsize.y*0.4);
}

void 圆环(ImVec2 center, float radius, ImColor color, float thickness, float luping, float current_time) {
float percentage = current_time / luping;
percentage = ImClamp(percentage, 0.0f, 1.0f);
float angle = percentage * IM_PI * 2.0f;
ImGui::GetForegroundDrawList()->PathClear();
ImGui::GetForegroundDrawList()->PathArcTo(center, radius, IM_PI * 1.5f, IM_PI * 1.5f + angle, 64);
ImGui::GetForegroundDrawList()->PathStroke(color, false, thickness);
}


void RenderFlowingRectangle() {

}


void window_set() {
    ImGuiStyle & style = ImGui::GetStyle();
    ImGui::StyleColorsLight();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);    
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);    
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);  
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(74, 146, 255, 255);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(74, 146, 255, 255);  
    style.Colors[ImGuiCol_Text] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);  
    style.WindowRounding = 12.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarSize = 0.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabMinSize = 8.0f;
    style.GrabRounding = 8.0f;
    style.ScaleAllSizes(1.0f);
}


void zhu_set() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 25.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
}

void title(const char* text, float size = 6.0f, ImVec4 color = ImVec4(0.7f, 0.7f, 0.7f, 1.0f), ImVec4 textcolor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f)) {
    ImGui::TextColored(textcolor, "%s", text);
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImVec2 start = ImVec2(cursor_pos.x, cursor_pos.y + ImGui::GetFrameHeight() / 2 - 30);
    ImVec2 end = ImVec2(start.x + ImGui::GetContentRegionAvail().x, start.y);
    ImGui::GetWindowDrawList()->AddLine(start, end, ImGui::GetColorU32(color), size);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
}

void Line(float longg = ImGui::GetContentRegionAvail().x, float size = 1.0f, ImVec4 color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f)) {
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    ImVec2 end = ImVec2(ImGui::GetCursorScreenPos().x + longg, ImGui::GetCursorScreenPos().y);
    ImGui::GetWindowDrawList()->AddLine(ImGui::GetCursorScreenPos(), end, ImGui::GetColorU32(color), size);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
}

void CenteredText(const char* text, float customScale = 1.0f) {
    ImGui::PushFont(ImGui::GetFont());
    float fontSize = ImGui::GetFontSize() * customScale;
    ImVec2 textSize = ImGui::CalcTextSize(text) * customScale;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) * 0.5f);
    ImGui::SetWindowFontScale(customScale);
    ImGui::Text("%s", text);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
}


void Checkbox(const char* label, bool* value) {
ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 9999.0f);
ImGui::Checkbox(label, value);
ImGui::PopStyleVar();
}
bool button(string name, ImVec2 size, float dingsize = 15.0f, 
            ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
            ImVec4 color1 = ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
            ImVec4 color2 = ImVec4(0.0f, 155.0f/255.0f, 255.0f/255.0f, 0.8f)) {  // 修改这里
    ImGui::PushStyleColor(ImGuiCol_Button,          color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.0f, 155.0f/255.0f, 255.0f/255.0f, 0.8f));  // 修改这里
    ImGui::PushStyleColor(ImGuiCol_Border,          color1);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0.0f, 125.0f/255.0f, 255.0f/255.0f, 0.9f));  // 修改这里
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, dingsize);
    std::string label = i18n::TL(name);
    bool clicked = ImGui::Button(label.c_str(), size);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
    return clicked;
}

void Toggle(const string& names, bool& toggle, float x = 300, float size = 45, 
            ImVec2 button_size = ImVec2(120, 56), float speed = 0.05f, 
            ImColor 未激活 = ImColor(190, 190, 190), ImColor 已激活 = ImColor(15, 150, 255)) {
    wc& name = wh[names.c_str()];
    ImVec2 originalCursorPos = ImGui::GetCursorPos();
  
    float windowWidth = ImGui::GetWindowWidth();
    float boundary = windowWidth * 0.06f;
    float buttonRightPos = windowWidth - boundary - button_size.x;
    
    ImGui::SetCursorPosX(buttonRightPos);
    
    if (ImGui::InvisibleButton(names.c_str(), button_size)) {
        toggle = !toggle; 
    }   
    ImVec2 min = ImGui::GetItemRectMin(); 
    ImVec2 max = ImGui::GetItemRectMax(); 
    float radius = (max.y - min.y) / 2.0f;
    if (toggle) {
        name.end = max.x - radius;
    } else {
        name.end = min.x + radius;
    }
    ImColor color_end = toggle ? 已激活 : 未激活;
    Lerp(name.color.Value.x, color_end.Value.x, speed);
    Lerp(name.color.Value.y, color_end.Value.y, speed);
    Lerp(name.color.Value.z, color_end.Value.z, speed);
    Lerp(name.color.Value.w, color_end.Value.w, speed);
    Lerp(name.beg, name.end, speed); 
    ImVec2 circle_center(name.beg, min.y + radius);
    circle_center.x = ImClamp(circle_center.x, min.x + radius, max.x - radius);
    
    ImGui::GetWindowDrawList()->AddRectFilled(min, max, name.color, 9999.0f, 0);
    ImGui::GetWindowDrawList()->AddCircleFilled(circle_center, radius*0.85f, ImColor(255, 255, 255));    
    
    ImGui::SetCursorPos(originalCursorPos);
    std::string label = i18n::TL(names);
    auto textSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, -1, label.c_str());
    ImVec2 textPos = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddText(NULL, size, textPos, ImColor(255, 255, 255, 255), label.c_str());
    ImGui::SetCursorPosY(originalCursorPos.y + std::max(textSize.y, button_size.y) + ImGui::GetStyle().ItemSpacing.y);
}

void SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.2f", 
                ImGuiSliderFlags flags = 0, float cursor_x = 0.0f, float width = 0.0f, float overall_offset = 0.0f) {
    ImGuiStyle& style = ImGui::GetStyle();  
    // Прозрачный фон и более жирный ползунок
    float originalFrameY = style.FramePadding.y;
    float originalGrab = style.GrabMinSize;
    ImVec4 fb = style.Colors[ImGuiCol_FrameBg]; fb.w *= 0.35f;
    ImVec4 fbH= style.Colors[ImGuiCol_FrameBgHovered]; fbH.w *= 0.45f;
    ImVec4 fbA= style.Colors[ImGuiCol_FrameBgActive];  fbA.w *= 0.50f;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, originalFrameY*3.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, fb);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, fbA);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, fbH);
    // Grab цвета — используем стиль по умолчанию
    style.GrabMinSize = originalGrab * 3.0f;

    // Подпись сверху
    std::string l = i18n::TL(std::string(label));
    ImGui::Text("%s", l.c_str());

    float totalWidth = width > 0.0f ? width : ImGui::GetContentRegionAvail().x;
    float sliderWidth = totalWidth * 0.5f; // половина стандартной длины
    if (cursor_x > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cursor_x);
    ImGui::SetNextItemWidth(sliderWidth);
    ImGui::SliderFloat(("##" + std::string(label)).c_str(), v, v_min, v_max, "", flags);

    // Значение справа от слайдера
    ImGui::SameLine();
    char valueText[32];
    if (format) snprintf(valueText, sizeof(valueText), format, *v);
    else        snprintf(valueText, sizeof(valueText), "%.2f", *v);
    std::string vs = i18n::ReplaceUnits(valueText);
    ImGui::Text("%s", vs.c_str());

    // Восстановление стиля
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(3);
    style.GrabMinSize = originalGrab;
    style.FramePadding.y = originalFrameY;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
}




void 绘制调色板() {
static ImVec2 tsb_min, tsb_max, tsb_end;
static string last_clicked_button = "";
ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
ImGui::BeginGroup();
for (int i = 0; i < Styles.size(); ++i) {
auto it = next(Styles.begin(), i);
string buttonText = "按钮 " + it->first;
ImVec4 color = it->second.Value;

ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
ImGui::PushStyleColor(ImGuiCol_Button,          color);
ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  color);
ImGui::PushStyleColor(ImGuiCol_ButtonActive,    color);
ImGui::PushStyleColor(ImGuiCol_Border,          color);


int itemsPerLine = Styles.size() / 7;
int remainder = Styles.size() % 7;

if ((i > 0) && (i % itemsPerLine == 0)) ImGui::NewLine();
if (Styles.size() - ((i + remainder) / 7 * 7) == remainder) {
ImGui::NewLine();
ImGui::NewLine();
ImGui::NewLine();
}

ImVec2 buttonPos = ImGui::GetCursorScreenPos(); 
if (ImGui::Button(buttonText.c_str(), ImVec2(50, 50))) {
last_clicked_button = it->first;
tsb_end = buttonPos;
tsb_color_end = color;
}
if (last_clicked_button == it->first) {
tsb_end = buttonPos;
}

ImGui::SameLine();
ImGui::PopStyleColor(5);
}
ImGui::EndGroup();
ImGui::PopStyleVar();


Lerp(tsb_min.x, tsb_end.x - 8, 0.05f);
Lerp(tsb_min.y, tsb_end.y - 8, 0.04f);
Lerp(tsb_max.x, tsb_end.x + 58, 0.05f);
Lerp(tsb_max.y, tsb_end.y + 58, 0.04f);


Lerp(tsb_color.Value.x, tsb_color_end.Value.x, 0.04f);
Lerp(tsb_color.Value.y, tsb_color_end.Value.y, 0.04f);
Lerp(tsb_color.Value.z, tsb_color_end.Value.z, 0.04f);
Lerp(tsb_color.Value.w, tsb_color_end.Value.w, 0.04f);

ImGui::GetWindowDrawList()->AddRectFilled( tsb_min, tsb_max, tsb_color, 12);	

}



void 调色板按钮(string names, ImColor& Color, int Tab, float 按钮x = 335, float 按钮y = 105, int 矩形位置 = 0, float 矩形圆角 = 17) {
ImVec2 min, max, textPos, textMin, textMax;

ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0, 0, 0, 0));
ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0, 0, 0, 0));
ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0, 0, 0, 0));
ImGui::PushStyleColor(ImGuiCol_Border,          ImVec4(0, 0, 0, 1));
ImGui::PushStyleColor(ImGuiCol_Text,            ImVec4(0, 0, 0, 0));
std::string btnLabel = i18n::TL(names);
if (ImGui::Button(btnLabel.c_str(), ImVec2(按钮x, 按钮y))) {
调色板Tab = Tab;
tsb_but = names;
}
ImGui::PopStyleColor();

if (调色板Tab == Tab) {
Lerp(Color.Value.x, tsb_color.Value.x, 0.04f);
Lerp(Color.Value.y, tsb_color.Value.y, 0.04f);
Lerp(Color.Value.z, tsb_color.Value.z, 0.04f);
Lerp(Color.Value.w, 透明度 / 1000, 0.04f);
} 
if (!矩形位置) {
min = ImVec2(ImGui::GetItemRectMin().x + 按钮x*0.5f, ImGui::GetItemRectMin().y + 20); 
max = ImVec2(ImGui::GetItemRectMax().x - 20, ImGui::GetItemRectMax().y - 20); 
ImGui::GetWindowDrawList()->AddRectFilled( min, max, Color, 12);	
} else {
min = ImVec2(ImGui::GetItemRectMin().x + 按钮x*0.62f, ImGui::GetItemRectMin().y + 15); 
max = ImVec2(ImGui::GetItemRectMax().x - 15, ImGui::GetItemRectMax().y - 15); 
ImGui::GetWindowDrawList()->AddRectFilled( min, max, tsb_color, 12);	
}
textMin = ImGui::GetItemRectMin();
textMax = ImVec2(ImGui::GetItemRectMin().x + (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x) / 2, ImGui::GetItemRectMin().y + (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y) / 2);
std::string names_tl = i18n::TL(names);
auto textSize = ImGui::GetFont()->CalcTextSizeA(38, FLT_MAX, -1, names_tl.c_str(), NULL, NULL);
textPos = ImVec2( textMax.x - (textMax.x - textMin.x) / 2 - (textSize.x / 2), textMax.y - (textSize.y / 2)); 
if (!矩形位置) {
textPos = ImVec2( textMax.x - (textMax.x - textMin.x) / 2 - (textSize.x / 2), textMax.y - (textSize.y / 2)); 
ImGui::GetWindowDrawList()->AddText(NULL, 38, textPos, ImColor(0, 0, 0), names_tl.c_str());
} else {
textPos = ImVec2( textMax.x - (textMax.x - textMin.x) / 2 - (textSize.x / 2) - 8, textMax.y - (textSize.y / 2)); 
std::string modPrefix = i18n::TL("修改: ");
ImGui::GetWindowDrawList()->AddText(NULL, 35, textPos, ImColor(0, 0, 0), (modPrefix + tsb_but).c_str());
}

ImGui::PopStyleColor(4);
}
void whitecloud(const vector<string>& names, float& 参数, float 居中 = 0, float 按钮x = 120, float 按钮y = 70, float 矩形圆角 = 17) {
    ImVec2 按钮 = ImVec2(按钮x, 按钮y);
    ImVec2 矩形 = ImVec2(按钮.x * 0.97f, 按钮.y * 0.95f);
    ImVec2 差距 = ImVec2(按钮.x - 矩形.x, 按钮.y - 矩形.y);
    ImVec2 窗口(按钮.x*names.size(), 按钮.y);
    ImVec2 min, max;
    wc& name = wh[names[0].c_str()];

    // 修改1: 将窗口位置设置为右侧
    if (居中)
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 窗口.x - ImGui::GetStyle().WindowPadding.x);
    
    ImGuiStyle& st = ImGui::GetStyle();
    ImVec4 bg  = st.Colors[ImGuiCol_WindowBg];
    ImVec4 btn = st.Colors[ImGuiCol_Button];
    ImGui::PushStyleColor(ImGuiCol_Border, bg);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
    ImGui::PushStyleColor(ImGuiCol_Button,          btn);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   st.Colors[ImGuiCol_ButtonHovered]);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    st.Colors[ImGuiCol_ButtonActive]);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
    
    ImGui::BeginChild(names[0].c_str(), 窗口, true);
    ImGui::PushStyleColor(ImGuiCol_Text, st.Colors[ImGuiCol_Text]);
    
    if (ImGui::Button(i18n::TL(names[0]).c_str(), ImVec2(按钮.x, 按钮.y))) {
    name.Tab = 0;
    参数 = 0;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    min = ImVec2(ImGui::GetItemRectMin().x + 差距.x, ImGui::GetItemRectMin().y + 差距.y); 
    max = ImVec2(ImGui::GetItemRectMax().x - 差距.x, ImGui::GetItemRectMax().y - 差距.y); 
    // Selected-tab bounce animation (down then up) on change
    static std::map<std::string, int> __tab_last__;
    static std::map<std::string, float> __tab_anim_t__;
    const std::string __tab_key__ = names[0];
    if (!__tab_last__.count(__tab_key__)) __tab_last__[__tab_key__] = -1;
    if (!__tab_anim_t__.count(__tab_key__)) __tab_anim_t__[__tab_key__] = 1.0f;
    if (__tab_last__[__tab_key__] != name.Tab) {
        __tab_last__[__tab_key__] = name.Tab;
        __tab_anim_t__[__tab_key__] = 0.0f;
    }
    float __tab_yofs__ = 0.0f;
    const float __tab_dur__ = 0.25f;
    if (__tab_anim_t__[__tab_key__] < __tab_dur__) {
        float __t = __tab_anim_t__[__tab_key__] / __tab_dur__;
        __tab_yofs__ = 8.0f * sinf(3.1415926f * __t);
        __tab_anim_t__[__tab_key__] = std::min(__tab_dur__, __tab_anim_t__[__tab_key__] + ImGui::GetIO().DeltaTime);
    }
    Lerp(name.beg, name.end, 0.06f);
    
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(min.x + name.beg, min.y + __tab_yofs__),
        ImVec2(max.x + name.beg, max.y + __tab_yofs__),
        ImGui::GetColorU32(st.Colors[ImGuiCol_Button]),
        矩形圆角,
        ImDrawCornerFlags_All
    );
    
    for (int i = 0; i < names.size(); i++) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
        if (i + 1 < names.size()) {
            if (ImGui::Button(i18n::TL(names[i + 1]).c_str(), ImVec2(按钮.x, 按钮.y))) {
                name.Tab = i + 1;
                参数 = i + 1;
            }
            ImGui::SameLine();
        }
        ImGui::PopStyleColor();
        if (name.Tab == i) {
            name.end = 按钮.x * i;
        }
        string bynh = i18n::TL(names[i]);
        ImVec2 居中(min.x, min.y);
        if (i > 0) {
            居中.x += 按钮.x * i;
        }
        float px = 居中.x + (max.x - min.x) / 2;
        float py = 居中.y + (max.y - min.y) / 2;
        auto textSize_bynh = ImGui::GetFont()->CalcTextSizeA(35, FLT_MAX, -1, bynh.c_str(), NULL, NULL);
        ImVec2 textPos = ImVec2(px - (textSize_bynh.x / 2), py - (textSize_bynh.y / 2));
        if (name.Tab == i) textPos.y += __tab_yofs__;

        ImGui::GetWindowDrawList()->AddText(NULL, 35, textPos, ImGui::GetColorU32(st.Colors[ImGuiCol_Text]), bynh.c_str());
    }    

    ImGui::EndChild();
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(2);
}

void DrawFlowingGradientLine(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImVec4 start_color, ImVec4 end_color, float flow_speed) {
const int num_segments = 100; 
ImVec2 direction = end - start;
ImVec2 segment = direction / (float)num_segments;

float flow_offset = fmod(ImGui::GetTime() * flow_speed, 1.0f);

ImVec2 current_position = start;
for (int i = 0; i < num_segments; ++i) {
float t = (float)i / (float)(num_segments - 1) + flow_offset;
t = fmod(t, 1.0f);

ImVec4 color = ImVec4(
start_color.x + (end_color.x - start_color.x) * t,
start_color.y + (end_color.y - start_color.y) * t,
start_color.z + (end_color.z - start_color.z) * t,
start_color.w + (end_color.w - start_color.w) * t
);
ImU32 im_color = ImGui::ColorConvertFloat4ToU32(color);
ImVec2 next_position = current_position + segment;

draw_list->AddLine(current_position, next_position, im_color, 10.0f); 

current_position = next_position;
}
}
void DrawFlowingGradientLine2(ImDrawList* draw_list, ImVec2 start, ImVec2 end, ImVec4 start_color, ImVec4 end_color, float flow_speed) {
const int num_segments = 100; 
ImVec2 direction = end - start;
ImVec2 segment = direction / (float)num_segments;

float flow_offset = fmod(ImGui::GetTime() * flow_speed, 1.0f);

ImVec2 current_position = start;
for (int i = 0; i < num_segments; ++i) {
float t = (float)i / (float)(num_segments - 1);
t = fmod(t + flow_offset, 1.0f);

ImVec4 color;
if (t < 0.5f) {

float t_normalized = t * 2.0f;
color = ImVec4(
start_color.x + (end_color.x - start_color.x) * t_normalized,
start_color.y + (end_color.y - start_color.y) * t_normalized,
start_color.z + (end_color.z - start_color.z) * t_normalized,
start_color.w + (end_color.w - start_color.w) * t_normalized
);
} else {
float t_normalized = (t - 0.5f) * 2.0f;
color = ImVec4(
end_color.x + (start_color.x - end_color.x) * t_normalized,
end_color.y + (start_color.y - end_color.y) * t_normalized,
end_color.z + (start_color.z - end_color.z) * t_normalized,
end_color.w + (start_color.w - end_color.w) * t_normalized
);
}

ImU32 im_color = ImGui::ColorConvertFloat4ToU32(color);
ImVec2 next_position = current_position + segment;

draw_list->AddLine(current_position, next_position, im_color, 10.0f); 

current_position = next_position;
}
}
string getBeijingTimeString() {
std::time_t now = std::time(nullptr);
std::tm* now_tm = std::gmtime(&now);

now_tm->tm_hour += 8;
now_tm->tm_isdst = 0;
if (now_tm->tm_hour >= 24) {
now_tm->tm_hour -= 24;
now_tm->tm_mday += 1;
}

char buffer[20];
std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", now_tm);
return std::string(buffer);
}

map<string, ImColor>Styles={{"初始化白色",ImColor(1,1,1)},{"象牙黑",ImColor(41,36,33)},{"泠灰",ImColor(128,138,135)},{"灰色",ImColor(192,192,192)},{"石板灰",ImColor(112,128,144)},{"暖灰色",ImColor(128,128,105)},{"牛马蓝色",ImColor(140,5,255)},{"桔黄色",ImColor(255,228,196)},{"cornsilk",ImColor(255,248,220)},{"蜜露橙",ImColor(240,255,240)},{"象牙色",ImColor(225,25,240)},{"亚麻色",ImColor(250,240,230)},{"navajo",ImColor(255,22,173)},{"old",ImColor(253,245,20)},{"海贝壳色",ImColor(55,195,28)},{"淡黄色",ImColor(229,222,179)},{"砖红",ImColor(156,102,31)},{"镉红",ImColor(227,23,13)},{"珊瑚色",ImColor(255,127,80)},{"耐火砖色",ImColor(178,34,34)},{"印度红",ImColor(176,23,31)},{"栗色",ImColor(176,48,96)},{"粉红",ImColor(255,192,203)},{"木莓色",ImColor(135,38,87)},{"红",ImColor(255,0,0)},{"橙红色",ImColor(250,128,114)},{"蕃茄红",ImColor(255,99,71)},{"镉橙",ImColor(255,97,3)},{"胡萝卜色",ImColor(237,145,33)},{"桔黄",ImColor(255,128,0)},{"桔红",ImColor(255,69,0)},{"香蕉色",ImColor(227,207,87)},{"镉黄",ImColor(255,153,18)},{"dougello",ImColor(235,142,85)},{"forum",ImColor(255,227,132)},{"金黄色",ImColor(255,215,0)},{"黄花色",ImColor(218,165,32)},{"瓜色",ImColor(227,168,105)},{"黄色",ImColor(255,255,0)},{"米色",ImColor(163,148,128)},{"棕色",ImColor(128,42,42)},{"锻浓黄土色",ImColor(138,54,15)},{"锻棕土色",ImColor(138,51,36)},{"巧克力色",ImColor(210,105,30)},{"肉色",ImColor(255,125,64)},{"黄褐色",ImColor(240,230,140)},{"玫瑰红",ImColor(188,143,143)},{"肖贡土色",ImColor(199,97,20)},{"标土棕",ImColor(115,74,18)},{"乌贼墨棕",ImColor(94,38,18)},{"赭色",ImColor(160,82,45)},{"马",ImColor(139,69,19)},{"沙棕色",ImColor(244,164,96)},{"棕褐色",ImColor(210,180,140)},{"蓝色",ImColor(0,0,255)},{"钴色",ImColor(61,89,171)},{"dodger",ImColor(30,144,255)},{"靛青",ImColor(8,46,84)},{"jackie",ImColor(11,23,70)},{"锰蓝",ImColor(3,168,158)},{"深蓝色",ImColor(25,25,112)},{"藏青色",ImColor(0,0,128)},{"孔雀篮色",ImColor(51,161,201)},{"浅灰蓝色",ImColor(176,224,230)},{"品蓝",ImColor(65,105,225)},{"石板蓝",ImColor(106,90,205)},{"天蓝",ImColor(135,206,235)},{"钢青色",ImColor(70,130,180)},{"土耳其玉色",ImColor(0,199,140)},{"深蓝色",ImColor(18,10,143)},{"碧绿色",ImColor(127,255,212)},{"青色",ImColor(0,255,255)},{"青绿色",ImColor(64,224,208)},{"黄绿色",ImColor(127,255,0)},{"钴绿色",ImColor(61,145,64)},{"翠绿色",ImColor(0,201,87)},{"森林绿",ImColor(34,139,34)},{"绿色",ImColor(0,255,0)},{"草地绿",ImColor(124,252,0)},{"酸橙绿",ImColor(50,205,50)},{"薄菏色",ImColor(189,252,201)},{"草绿色",ImColor(107,142,35)},{"暗绿色",ImColor(48,128,20)},{"海绿色",ImColor(46,139,87)},{"嫩绿色",ImColor(0,255,127)},{"绿土",ImColor(56,94,15)},{"紫罗蓝色",ImColor(138,43,226)},{"jasoa",ImColor(160,102,211)},{"湖紫色",ImColor(153,51,250)},{"红色",ImColor(255,0,255)},{"淡紫色",ImColor(218,112,214)},{"梅红色",ImColor(221,160,221)},{"紫色",ImColor(160,32,240)},{"艳青2",ImColor(0,255,255,255)},{"黄色2",ImColor(200,255,0,255)},{"蓝色2",ImColor(0,0,255,255)},{"橙色2",ImColor(255,165,0,255)},{"粉红2",ImColor(255,192,203,255)},{"白色2",ImColor(255,255,255,255)},{"紫色2",ImColor(15,67,255,255)},{"天蓝2",ImColor(135,206,235,255)},{"深天蓝",ImColor(0,191,255,255)},{"棕色2",ImColor(144,144,144,255)},{"空白2",ImColor(144,144,144,255)},{"粉色2",ImColor(226,145,163,255)},{"灰色2",ImColor(130,132,142,255)}};

float alpha_tm = 1.0f;
ImVec4 arr[] = {
{135/255.f,206/255.f,255/255.f,alpha_tm},
{255/255.f,0/255.f,0/255.f,alpha_tm},
{0/255.f,255/255.f,127/255.f,alpha_tm},
{123/255.f,200/255.f,0/255.f,alpha_tm},
{255/255.f,182/255.f,193/255.f,alpha_tm},
{218/255.f,112/255.f,214/255.f,alpha_tm},
{255/255.f,165/255.f,0/255.f,alpha_tm},
{153/255.f,204/255.f,255/255.f,alpha_tm},
{204/255.f,255/255.f,153/255.f,alpha_tm},
{255/255.f,200/255.f,153/255.f,alpha_tm},
{255/255.f,153/255.f,153/255.f,alpha_tm},
{153/255.f,153/255.f,204/255.f,alpha_tm},
{102/255.f,204/255.f,153/255.f,alpha_tm},
{255/255.f,102/255.f,0/255.f,alpha_tm},
{102/255.f,204/255.f,204/255.f,alpha_tm},
{153/255.f,204/255.f,255/255.f,alpha_tm},
{128/255.f,0/255.f,128/255.f,alpha_tm},
{0/255.f,128/255.f,128/255.f,alpha_tm},
{255/255.f,128/255.f,128/255.f,alpha_tm},
{255/255.f,215/255.f,0/255.f,alpha_tm},
{255/255.f,69/255.f,0/255.f,alpha_tm},
{153/255.f,50/255.f,204/255.f,alpha_tm},
{139/255.f,69/255.f,19/255.f,alpha_tm},
{0/255.f,139/255.f,139/255.f,alpha_tm},
{184/255.f,134/255.f,11/255.f,alpha_tm},
{169/255.f,169/255.f,169/255.f,alpha_tm},
{189/255.f,183/255.f,107/255.f,alpha_tm},
{233/255.f,150/255.f,122/255.f,alpha_tm},
{143/255.f,188/255.f,143/255.f,alpha_tm},
{255/255.f,0/255.f,255/255.f,alpha_tm}
};
int length = sizeof(arr)/16;

class DrawIo {
public:
inline void 绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char* str) {
ImGui::GetBackgroundDrawList()->AddText(NULL, size, {(float)(x-0.1), (float)(y-0.1)}, color1, str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size, {(float)(x+0.1), (float)(y+0.1)}, color1, str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size, {x, y}, color, str);
}

inline void 绘制字体描边(float size,int x, int y, ImVec4 color, const char* str) {
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
}


inline void CenterText(float X,float Y,const char* content,float fontSize, ImVec4 color) {
auto textSize = ImGui::CalcTextSize(content, 0, fontSize);
textSize.x *= fontSize / 25.0f;
textSize.y *= fontSize / 25.0f;
ImGui::GetBackgroundDrawList()->AddText(NULL, fontSize,ImVec2(X - textSize.x / 2, Y), ImGui::ColorConvertFloat4ToU32(color), content);  
}

inline void DrawRectFilledCorner(int x, int y, int w, int h, ImColor color, int rounding) {
ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), (color), rounding, ImDrawCornerFlags_All);
}

inline void DrawCircleFilled(int x, int y, int radius, ImVec4 color, int segments) {
ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(color), segments);
}

inline void DrawLine(int x1, int y1, int x2, int y2, ImVec4 color, int size) {
ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(color), size);
}

inline void DrawRect(int x, int y, int w, int h, ImVec4 color,int size) {
ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x+w, y+h), ImGui::ColorConvertFloat4ToU32(color), 10, ImDrawCornerFlags_All, size);
}


inline void DrawRectFilled(int x, int y, int w, int h, ImVec4 color) {
ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x+w, y+h), ImGui::ColorConvertFloat4ToU32(color), 0, 0);
}

inline void DrawCircle(float X, float Y, float radius, float a, float r, float g, float b, float thickness) {
ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(X, Y), radius, ImColor(r, g, b, a), 0, thickness);
}

inline void DrawStrokeText(int x, int y, ImVec4 color, const char* str) {
ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, 25, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
}

inline void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, ImVec4 Color, int T) {
ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(Color));
}

inline void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, ImVec4 Color, int T) {
ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(Color));
}
}DrawTf;
void TouchScroll(const bool& InertiaFlag = false)
{
    ImGuiWindow *WinPtr = ImGui::GetCurrentWindow();
    float ScrollMaxY = ImGui::GetScrollMaxY();
    if (WinPtr->SkipItems || ScrollMaxY <= 0.0f)
        return;

    static float ScrollVel = 0.0f;
    static float ScrollAccel = 0.0f;
    bool Hover = ImGui::IsWindowHovered();
    ImGuiIO &GuiIO = ImGui::GetIO();

    static ImVec2 StartTouchPos = ImVec2(0, 0);
    static bool Scroll = false;
    static float LastScrollY = 0.0f;
    static double LastTime = 0.0;

    auto UpdateScroll = [&](float deltaY, float deltaTime) {
        float CurScrollY = ImGui::GetScrollY();
        float NewScrollY = CurScrollY + deltaY;
        NewScrollY = ImClamp(NewScrollY, 0.0f, ScrollMaxY);
        ImGui::SetScrollY(NewScrollY);

        ScrollVel = deltaY / (deltaTime + 0.0001f);
        ScrollAccel = ScrollVel / (deltaTime + 0.0001f); // 计算加速度
        LastScrollY = NewScrollY;
    };

    auto GetDeltaTime = [&]() {
        double CurTime = ImGui::GetTime();
        float DeltaTime = (float)(CurTime - LastTime);
        LastTime = CurTime;
        return DeltaTime;
    };

    auto ToggleParentWindowMove = [WinPtr](bool noMove) {
        if (ImGuiWindow *ParWinPtr = WinPtr->ParentWindow)
        {
            if (noMove)
            {
                ParWinPtr->Flags |= ImGuiWindowFlags_NoMove;
            }
            else
            {
                ParWinPtr->Flags &= ~ImGuiWindowFlags_NoMove;
            }
        }
    };

    if (Hover && GuiIO.MouseDown[0] && !Scroll)
    {
        StartTouchPos = GuiIO.MousePos;
        Scroll = true;
        LastScrollY = ImGui::GetScrollY();
        LastTime = ImGui::GetTime();
        ScrollVel = 0.0f; // 重置速度
        ScrollAccel = 0.0f; // 重置加速度
        ToggleParentWindowMove(true);
    }

    if (Scroll)
    {
        float DeltaTime = GetDeltaTime();

        if (GuiIO.MouseDown[0])
        {
            float DeltaY = StartTouchPos.y - GuiIO.MousePos.y;
            UpdateScroll(DeltaY, DeltaTime);
            StartTouchPos = GuiIO.MousePos;
        }
        else
        {
            Scroll = false;
            ToggleParentWindowMove(false);
        }
    }

    // 惯性滚动部分
    if (!Scroll && fabsf(ScrollVel) > 10.0f && InertiaFlag) // 增加速度阈值
    {
        float DeltaTime = GetDeltaTime();
        float CurScrollY = ImGui::GetScrollY();
        
        // 应用惯性效果
        float NewScrollY = CurScrollY + ScrollVel * DeltaTime;
        
        // 应用阻尼效果
        float damping = 0.95f; // 阻尼系数
        ScrollVel *= powf(damping, DeltaTime * 60.0f);
        
        // 边界检查
        if (NewScrollY <= 0.0f)
        {
            NewScrollY = 0.0f;
            ScrollVel = 0.0f;
        }
        else if (NewScrollY >= ScrollMaxY)
        {
            NewScrollY = ScrollMaxY;
            ScrollVel = 0.0f;
        }
        
        ImGui::SetScrollY(NewScrollY);
        
        // 当速度很小时停止
        if (fabsf(ScrollVel) < 1.0f)
        {
            ScrollVel = 0.0f;
        }
    }
    else if (!Scroll)
    {
        ScrollVel = 0.0f;
    }
}

#endif
