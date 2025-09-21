
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
