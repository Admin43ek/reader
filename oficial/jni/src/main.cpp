#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <malloc.h>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using std::string;
using std::to_string;
using std::vector;
extern bool DrawIo[50];
extern float NumIo[50];
extern bool sym_0001[50];
void NumIoSave(const char* name);
void NumIoLoad(const char* name);
int GetBase();

#include <draw.h>
#include <TouchHelper.h>
#include "Android_draw/sym_0002.h"
#include "Android_draw/sym_0003/touxiang.h"
#include "wanbai.h"
#include "sym_0004.h"
#include "sym_0005.h"
#include "imgui_ui.h"
#include "ImGui/imgui.h"
#include "i18n.h"
#include "add/ThreadPool.h"
#include "add/DataReader.h"
#include "CPUaffinity/timer.h"
int style_zt = 0;
int style_zx = 0;
int style_bw = 0;
int style_cd = 0;
int style_idx = 0;

// Global base position for ZK overlay (used by boot animation and main UI)
ImVec2 g_zkBasePos = ImVec2(40, 80);
// Matrix connect status to tint buttons
enum class ConnStatus { None, Ok, Fail };
static ConnStatus g_conn_status = ConnStatus::None;
static bool TryConnectMatrix() {
    int r = GetBase();
    g_conn_status = (r==1) ? ConnStatus::Ok : ConnStatus::Fail;
    return r==1;
}

static void PushConnectButtonColors() {
    ImVec4 ok  = ImVec4(0.65f, 0.92f, 0.65f, 1.0f); // very light green
    ImVec4 okH = ImVec4(0.70f, 0.95f, 0.70f, 1.0f);
    ImVec4 okA = ImVec4(0.60f, 0.90f, 0.60f, 1.0f);
    ImVec4 er  = ImVec4(0.90f, 0.25f, 0.25f, 1.0f);
    ImVec4 erH = ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
    ImVec4 erA = ImVec4(0.85f, 0.20f, 0.20f, 1.0f);
    if (g_conn_status == ConnStatus::Ok) {
        ImGui::PushStyleColor(ImGuiCol_Button, ok);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, okH);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, okA);
    } else if (g_conn_status == ConnStatus::Fail) {
        ImGui::PushStyleColor(ImGuiCol_Button, er);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, erH);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, erA);
    } else {
        // default theme colors
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_Button));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }
}

static std::string GetConnectLabel() {
    return (g_conn_status == ConnStatus::Ok) ? i18n::TL("Matrix Acquired")
                                            : i18n::TL("Connect Data");
}

// Fancy hash frame drawer for popups (blue/black alternating '#')
static void DrawHashFrame(ImVec2 min, ImVec2 max) {
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    float step = ImGui::CalcTextSize("#").x;
    int cols = (int)((max.x - min.x)/step);
    ImU32 blue = ImColor(50,120,255,255);
    ImU32 black= ImColor(0,0,0,255);
    // top and bottom rows
    for (int i=0;i<=cols;i++){
        float x = min.x + i*step;
        dl->AddText(ImVec2(x, min.y), (i%2)?blue:black, "#");
        dl->AddText(ImVec2(x, max.y - ImGui::GetTextLineHeight()), (i%2)?blue:black, "#");
    }
    // left/right columns
    int rows = (int)((max.y - min.y)/ImGui::GetTextLineHeight());
    for (int j=0;j<=rows;j++){
        float y = min.y + j*ImGui::GetTextLineHeight();
        dl->AddText(ImVec2(min.x, y), (j%2)?blue:black, "#");
        dl->AddText(ImVec2(max.x - step, y), (j%2)?blue:black, "#");
    }
}

// Developer Items mode globals (after ImGui is available)
bool g_items_dev_mode = false; // toggles drawing raw item names
char g_dev_watch_name[128] = {0};
float g_dev_watch_best = 1e12f;
bool g_items_draw_overrides = false; // after loading from file

struct ItemOverrideRec { std::string name; ImColor col; std::string label; };
static std::vector<ItemOverrideRec> g_item_overrides;

static void AppendItemOverrideLine(const char* itemName) {
    if (!itemName || !*itemName) return;
    FILE* f = fopen("/data/adb/items.txt", "a");
    if (!f) return;
    fprintf(f, "%s 255 255 255 Object\n", itemName);
    fclose(f);
}

static void LoadItemOverridesFile() {
    g_item_overrides.clear();
    std::ifstream in("/data/adb/items.txt");
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0]=='#') continue;
        std::istringstream iss(line);
        std::string name; int r=255,g=255,b=255; std::string label;
        if (!(iss >> name >> r >> g >> b)) continue;
        std::string rest; std::getline(iss, rest);
        if (!rest.empty() && rest[0]==' ') rest.erase(0,1);
        label = rest.empty()? name : rest;
        ItemOverrideRec rec; rec.name = name; rec.col = ImColor(r,g,b,255); rec.label = label;
        g_item_overrides.emplace_back(std::move(rec));
    }
}

// C-style helper for hook.h to check override by exact name
bool FindItemOverrideC(const char* name, ImU32* out_col, char* out_label, int label_cap) {
    if (!g_items_draw_overrides) return false;
    if (!name) return false;
    for (const auto& rec : g_item_overrides) {
        if (rec.name == name) {
            if (out_col) *out_col = rec.col;
            if (out_label && label_cap>0) {
                strncpy(out_label, rec.label.c_str(), label_cap-1);
                out_label[label_cap-1] = '\0';
            }
            return true;
        }
    }
    return false;
}
struct NumberAnimation {
    string current;
    string next;
    float progress = 0.0f;
    bool animating = false;
};

static int touchCount = 0;
enum AnimationMode {
    ANIM_HORIZONTAL,
    ANIM_VERTICAL,
    ANIM_BEZIER
};
static float errorBgWidth = 0.0f;
static float errorBgHeight = 0.0f;
static float targetBgWidth = 0.0f;
static float targetBgHeight = 0.0f;
static float bgAnimationSpeed = 0.1f;

static time_t libUE4SuccessTime = 0;
bool windowVisible = true;
float animationScale = 0.0f;
float targetScale = 0.0f;
float animationDamping = 0.07f;
bool tabChanging = false;
float tabOpacity = 1.0f;
float targetTabOpacity = 1.0f;
float tabDamping = 0.03f;

float cloud = 0.0f;
float white = 0.0f;
void InitAnimationData() {
    wh["win_x_0"] = {0.0f, 0.0f};
    wh["win_x_1"] = {0.0f, 0.0f};
    wh["win_x_2"] = {0.0f, 0.0f};
    wh["win_x_3"] = {0.0f, 0.0f};
    wh["win_x_4"] = {0.0f, 0.0f};
    wh["win_y_0"] = {0.0f, 0.0f};
    wh["win_y_1"] = {0.0f, 0.0f};
    wh["win_y_2"] = {0.0f, 0.0f};
    wh["win_y_3"] = {0.0f, 0.0f};
    wh["win_y_4"] = {0.0f, 0.0f};
}

void UpdateAnimation(float &value, float target, float damping) {
    value += (target - value) * damping;
    if (fabs(target - value) < 0.005f) {
        value = target;
    }
}

// Nightly theme for the entire UI
static void ApplyNightlyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text]           = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_WindowBg]       = ImVec4(0.10f, 0.10f, 0.12f, 0.98f);
    colors[ImGuiCol_ChildBg]        = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg]        = ImVec4(0.12f, 0.12f, 0.14f, 0.94f);  // combo/list popup background
    colors[ImGuiCol_FrameBg]        = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_FrameBgActive]  = ImVec4(0.35f, 0.35f, 0.45f, 1.00f);
    colors[ImGuiCol_Header]         = ImVec4(0.35f, 0.35f, 0.40f, 0.60f);  // selected item background (gray glow)
    colors[ImGuiCol_HeaderHovered]  = ImVec4(0.45f, 0.45f, 0.50f, 0.80f);
    colors[ImGuiCol_HeaderActive]   = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
    colors[ImGuiCol_Button]         = ImVec4(0.22f, 0.22f, 0.28f, 1.00f);
    colors[ImGuiCol_ButtonHovered]  = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive]   = ImVec4(0.35f, 0.35f, 0.45f, 1.00f);
    style.FrameRounding = 8.0f;
    style.GrabRounding  = 8.0f;
    style.WindowRounding = 14.0f;
}

// Single root window that expands/collapses from the ZK button
float g_ui_alpha = 1.0f; // global UI transparency (excludes ZK button)
// Draw top-center combined counter of bots + players
// ===============================
//  sym_0006 sym_0007 (sym_0008+sym_0009)
//   - sym_0010 sym_0011 sym_0012 sym_0013
//   - sym_0014 sym_0015, sym_0016 sym_0017 sym_0018 sym_0019 sym_0020 sym_0021
//   - sym_0022 = PlayerCount + BotCount
// ===============================
static void DrawTopCenterCounter()
{
    int players = fanhua.PlayerCount;
    int bots    = fanhua.BotCount;
    int total   = players + bots;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", total);

    float font_px = 48.0f;
    ImVec2 size = ImGui::GetFont()->CalcTextSizeA(font_px, FLT_MAX, -1.0f, buf, nullptr, nullptr);
    float x = (screen_x - size.x) * 0.5f;
    float y = 28.0f; // near top

    // sym_0010 sym_0011; sym_0023 — sym_0024 sym_0025 sym_0026 sym_0027
    ImColor col = (players > 0) ? ImColor(255, 0, 0, 128) : ImColor(144, 238, 144, 128);

    // Slight shadow for readability
    ImGui::GetForegroundDrawList()->AddText(nullptr, font_px, ImVec2(x+1, y+1), ImColor(0,0,0,110), buf);
    ImGui::GetForegroundDrawList()->AddText(nullptr, font_px, ImVec2(x,   y   ), col, buf);
}

// Placeholder for future boot animation
static bool RenderBootAnimation()
{
    static double t0 = -1.0;
    static bool done = false;
    if (done) return false;
    double now = ImGui::GetTime();
    if (t0 < 0.0) t0 = now;
    float T = (float)(now - t0);
    const float T_max = 5.8f; // +1s extension
    if (T >= T_max) { done = true; return false; }

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    // Fullscreen darken
    dl->AddRectFilled(ImVec2(0,0), ImVec2(screen_x, screen_y), ImColor(0,0,0,200));

    // Layout constants
    float cx = screen_x * 0.5f;
    float cy = screen_y * 0.5f;
    ImFont* font = ImGui::GetFont();
    float base = (float)ImMin(screen_x, screen_y);
    float Z_px = ImClamp(base * 0.18f, 48.0f, 140.0f);
    float Ker_px = Z_px * 0.70f;

    auto ease = [](float t){ return t*t*(3.0f-2.0f*t); };
    auto clamp01 = [](float v){ return v<0?0:(v>1?1:v); };

    // Phase timings
    float tZ_in = clamp01(T / 0.5f);
    float tReveal = clamp01((T - 0.5f) / 1.4f); // 0.5..1.9s
    float tCenter = clamp01((T - 1.9f) / 0.4f); // 1.9..2.3s
    float tCols = clamp01((T - 2.3f) / 2.3f);   // 2.3..4.6s (longer)
    float tFade = clamp01((T - 4.6f) / 1.2f);   // 4.6..5.8s

    // Common positions for Z
    ImVec2 Zsize = ImGui::CalcTextSize("Z");
    ImVec2 Zpos = ImVec2(cx - (Zsize.x * Z_px / ImGui::GetFontSize())*0.5f, cy - Z_px*0.5f);
    float Z_right = Zpos.x + (ImGui::CalcTextSize("Z").x * Z_px / ImGui::GetFontSize());
    // Draw only by phase to avoid overlap
    if (T < 0.5f) {
        float zAlpha = ease(tZ_in);
        float zScale = 0.8f + 0.2f * ease(tZ_in);
        dl->AddText(font, Z_px * zScale, Zpos, ImColor(255,255,255, (int)(255*zAlpha)), "Z");
    }

    // Kernel reveal from Z right edge, with 'l' first (only before centering starts)
    if (tReveal > 0.0f && tCenter <= 0.0f) {
        const char* letters = "Kernel"; int n = 6;
        float tight = Ker_px * 0.10f; // negative kerning between Z and K
        float wordTargetX = Z_right - tight;
        float widths[6]; float totalW = 0.0f;
        for (int i=0;i<n;i++){ widths[i] = ImGui::CalcTextSize(std::string(1, letters[i]).c_str()).x * Ker_px / ImGui::GetFontSize(); totalW += widths[i]; }
        int order[6] = {5,4,3,2,1,0}; // indices in letters: 5:l,4:e,3:n,2:r,1:e,0:K
        float lt_base = 0.0f, lt_step = 0.10f; // 100ms between
        float accX = 0.0f;
        // Also draw Z fixed (opaque) during reveal
        dl->AddText(font, Z_px, Zpos, ImColor(255,255,255,255), "Z");
        for (int i=0;i<6;i++){
            int idx = i; // final placement order
            int rank = 0; for(int r=0;r<6;r++){ if(order[r]==idx){ rank = r; break; } }
            float per = clamp01((tReveal - (lt_base + lt_step*rank)) / 0.25f); // each letter slides in ~250ms
            per = ease(per);
            float x0 = Z_right + Ker_px*0.6f; // offscreen start
            float x1 = wordTargetX + accX;
            float x = x0 + (x1 - x0) * per;
            ImVec2 p = ImVec2(x, cy - Ker_px*0.5f);
            dl->AddText(font, Ker_px, p, ImColor(255,255,255,(int)(255*per)), std::string(1, letters[idx]).c_str());
            accX += widths[idx];
        }
    }

    // Center ZKernel as one word (after reveal)
    float centerPer = ease(tCenter);
    if (centerPer > 0.0f && tCols <= 0.0f){
        std::string zker = "ZKernel";
        ImVec2 wsize = ImGui::CalcTextSize(zker.c_str());
        float Wpx = wsize.x * Ker_px / ImGui::GetFontSize();
        float tight = Ker_px * 0.10f;
        float wordTargetX = Z_right - tight;
        float startX = wordTargetX; // current left of word
        float endX = cx - Wpx*0.5f;
        float X = startX + (endX - startX) * centerPer;
        float up = 0.0f; // will lift later during columns
        // Draw with tight kerning between Z and K
        // Draw 'Z'
        dl->AddText(font, Ker_px, ImVec2(X, cy - Ker_px*0.5f - up), ImColor(255,255,255,255), "Z");
        float Zw = ImGui::CalcTextSize("Z").x * Ker_px / ImGui::GetFontSize();
        // Draw 'Kernel' with negative offset
        dl->AddText(font, Ker_px, ImVec2(X + Zw - tight, cy - Ker_px*0.5f - up), ImColor(255,255,255,255), "Kernel");
    }

    // Columns reveal from bottom center of word
    float colsPer = ease(tCols);
    if (colsPer > 0.0f && (T < 4.2f)){
        std::string zker = "ZKernel";
        ImVec2 wsize = ImGui::CalcTextSize(zker.c_str());
        float Wpx = wsize.x * Ker_px / ImGui::GetFontSize();
        float X = cx - Wpx*0.5f;
        float Y = cy - Ker_px*0.5f;
        // bottom of word
        float bottomY = Y + Ker_px;
        // lift word slightly while columns grow
        float lift = 12.0f * colsPer;
        dl->AddText(font, Ker_px, ImVec2(X, Y - lift), ImColor(255,255,255,255), "Z");
        float Zw = ImGui::CalcTextSize("Z").x * Ker_px / ImGui::GetFontSize();
        dl->AddText(font, Ker_px, ImVec2(X + Zw - (Ker_px*0.10f), Y - lift), ImColor(255,255,255,255), "Kernel");
        // two columns
        const char* L[2] = {"@ZKERNEL","@DEMONSCHEATS"};
        const char* R[2] = {"@Adm1n43k","@Night_demon6"};
        float colPad = 8.0f;
        float rowH = ImGui::CalcTextSize("@W").y * 1.1f;
        // compute dynamic centers to avoid overlap based on text widths
        float leftMax = ImMax(ImGui::CalcTextSize(L[0]).x, ImGui::CalcTextSize(L[1]).x);
        float rightMax = ImMax(ImGui::CalcTextSize(R[0]).x, ImGui::CalcTextSize(R[1]).x);
        float spacing = ImGui::CalcTextSize("WW").x; // ~width of 2 chars as gap
        float cx0 = cx - (spacing*0.5f) - leftMax * 0.5f;
        float cx1 = cx + (spacing*0.5f) + rightMax * 0.5f;
        for (int i=0;i<2;i++){
            float rowPer = clamp01((colsPer - 0.15f*i)/0.85f);
            float dy = (rowH+colPad) * (i+1) * rowPer;
            ImVec2 Lsz = ImGui::CalcTextSize(L[i]);
            ImVec2 Rsz = ImGui::CalcTextSize(R[i]);
            dl->AddText(nullptr, 0.0f, ImVec2(cx0 - Lsz.x*0.5f, bottomY - lift + dy), ImColor(255,255,255,255), L[i]);
            dl->AddText(nullptr, 0.0f, ImVec2(cx1 - Rsz.x*0.5f, bottomY - lift + dy), ImColor(255,255,255,255), R[i]);
        }
    }

    // Reabsorb 'Kernel' back into 'Z' (reverse reveal)
    if (T >= 4.2f && T < 5.0f){
        float tReabs = clamp01((T - 4.2f)/0.8f);
        tReabs = ease(tReabs);
        std::string zker = "ZKernel";
        ImVec2 wsize = ImGui::CalcTextSize(zker.c_str());
        float Wpx = wsize.x * Ker_px / ImGui::GetFontSize();
        float X = cx - Wpx*0.5f;
        float Y = cy - Ker_px*0.5f;
        dl->AddText(font, Ker_px, ImVec2(X, Y), ImColor(255,255,255,255), "Z");
        float Zw = ImGui::CalcTextSize("Z").x * Ker_px / ImGui::GetFontSize();
        float tight = Ker_px * 0.10f;
        float kernelLeft = X + Zw - tight;
        float Z_right2 = X + Zw;
        const char* letters = "Kernel"; int n=6;
        float widths[6]; for(int i=0;i<n;i++) widths[i]= ImGui::CalcTextSize(std::string(1,letters[i]).c_str()).x * Ker_px / ImGui::GetFontSize();
        int order[6] = {0,1,2,3,4,5};
        float accX=0.0f;
        for (int i=0;i<6;i++){
            int idx = i;
            float per = clamp01((tReabs - 0.10f*i)/0.25f);
            per = ease(per);
            float sX = kernelLeft + accX;
            float eX = Z_right2 + Ker_px*0.6f;
            float x = sX + (eX - sX) * per;
            float a = 1.0f - per;
            dl->AddText(font, Ker_px, ImVec2(x, Y), ImColor(255,255,255,(int)(255*a)), std::string(1,letters[idx]).c_str());
            accX += widths[idx];
        }
    }
    // Fly-in ZK button with grey background to final overlay spot
    if (T >= 5.0f){
        float tFly = clamp01((T - 5.0f)/0.8f);
        tFly = ease(tFly);
        float btn = Ker_px * (1.1f - 0.2f*tFly);
        ImVec2 start = ImVec2(cx - btn*0.5f, cy - btn*0.5f);
        ImVec2 end   = ImVec2(g_zkBasePos.x - btn * (1.0f/3.0f), g_zkBasePos.y - btn * (1.0f/3.0f));
        ImVec2 pos = ImVec2(start.x + (end.x - start.x)*tFly, start.y + (end.y - start.y)*tFly);
        ImU32 gb = ImColor(30,30,30, (int)(220*tFly));
        dl->AddRectFilled(pos, ImVec2(pos.x+btn, pos.y+btn), gb, btn*0.3f);
        float labelPx = btn * 0.6f;
        ImVec2 zsz = ImGui::CalcTextSize("Z");
        float zw = zsz.x * labelPx / ImGui::GetFontSize();
        ImVec2 lp = ImVec2(pos.x + (btn-zw)/2.0f - labelPx*0.08f, pos.y + (btn-labelPx)/2.0f);
        dl->AddText(font, labelPx, lp, ImColor(255,255,255,255), "Z");
        dl->AddText(font, labelPx, ImVec2(lp.x + zw - labelPx*0.10f, lp.y), ImColor(255,255,255,255), "K");
    }
    return true;
}

struct ZKButtonResult {
    bool clicked;
    bool active;
};

static ZKButtonResult DrawZKLogoButton(const ImVec2& size) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 9999.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.28f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));
    bool pressed = ImGui::Button("##ZKButton", size);
    ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    ImVec2 center = ImVec2((rect.Min.x + rect.Max.x) * 0.5f, (rect.Min.y + rect.Max.y) * 0.5f);
    ImFont* font = ImGui::GetFont();
    const float font_px = font->FontSize * 1.8f;
    ImVec2 zsz = font->CalcTextSizeA(font_px, FLT_MAX, 0.0f, "Z");
    ImVec2 ksz = font->CalcTextSizeA(font_px, FLT_MAX, 0.0f, "K");
    float overlap = ImMin(zsz.x, ksz.x) * 0.12f;
    float total = zsz.x + ksz.x - overlap;
    ImVec2 zpos(center.x - total * 0.5f, center.y - zsz.y * 0.5f);
    ImVec2 kpos(zpos.x + zsz.x - overlap, center.y - ksz.y * 0.5f);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 shadowOffset(1.0f, 1.0f);
    drawList->AddText(font, font_px, zpos + shadowOffset, ImColor(0, 0, 0, 120), "Z");
    drawList->AddText(font, font_px, kpos + shadowOffset, ImColor(0, 0, 0, 120), "K");
    drawList->AddText(font, font_px, zpos, ImColor(255, 255, 255, 255), "Z");
    drawList->AddText(font, font_px, kpos, ImColor(255, 255, 255, 255), "K");
    bool active = ImGui::IsItemActive();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    return {pressed, active};
}

static void RenderZKRoot() {
    ApplyNightlyTheme();
    // Apply global alpha to the whole UI (we'll override only around the ZK button)
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_ui_alpha);
    // Console-based key+driver flow is used. No ImGui modals here.
    enum class ZKState { Minimized, Expanding, Expanded, Collapsing };
    static ZKState state = ZKState::Expanded;
    static float t = 1.0f; // 0..1
    const ImVec2 collapsed(80,80);
    // Adaptive expanded size based on language and left menu label widths
    float baseW = 1080.0f, baseH = 440.0f;
    const char* menuLabels[] = {
        i18n::TL("Main").c_str(),
        i18n::TL("ESP").c_str(),
        i18n::TL("Aimbot").c_str(),
        i18n::TL("Items").c_str(),
        i18n::TL("ImGui Settings").c_str()
    };
    float maxLabelW = 0.0f;
    for (int i=0;i<5;i++) { ImVec2 ts = ImGui::CalcTextSize(menuLabels[i]); if (ts.x > maxLabelW) maxLabelW = ts.x; }
    float leftW_base = 160.0f;
    float leftW_lang = ImMax(leftW_base, maxLabelW + 40.0f);
    if (i18n::GetLanguage() == i18n::Lang::EN) leftW_lang += ImGui::CalcTextSize("WW").x; // add ~2 chars in EN
    float langFactor = (i18n::GetLanguage() == i18n::Lang::RU) ? 1.08f : 1.0f;
    ImVec2 expanded(baseW * langFactor + (leftW_lang - leftW_base), baseH);
    float dt = ImGui::GetIO().DeltaTime;
    float speed = 6.0f;
    if (state == ZKState::Expanding) { t = ImClamp(t + speed*dt, 0.0f, 1.0f); if (t>=1.0f) state = ZKState::Expanded; }
    if (state == ZKState::Collapsing){ t = ImClamp(t - speed*dt, 0.0f, 1.0f); if (t<=0.0f) state = ZKState::Minimized; }
    float te = t*t*(3.0f-2.0f*t);
    ImVec2 cur(collapsed.x + (expanded.x-collapsed.x)*te, collapsed.y + (expanded.y-collapsed.y)*te);

    ImGui::SetNextWindowPos(g_zkBasePos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(cur, ImGuiCond_Always);
    ImGui::Begin("ZKRoot", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBringToFrontOnFocus);

    // Compute ZK button size and animation (drawn later as overlay protruding outside window)
    static float zkPressScale = 1.0f;
    static bool  zkWasActive = false;
    static float zkHoldSec   = 0.0f;
    float dt2 = ImGui::GetIO().DeltaTime;
    float zkTargetScale = zkWasActive ? 1.2f : 1.0f; // sym_0028 sym_0029 sym_0025 sym_0030
    zkPressScale += (zkTargetScale - zkPressScale) * ImClamp(12.0f * dt2, 0.0f, 1.0f);
    // sym_0031 sym_0032 sym_0033 ZK sym_0034 sym_0035 2 sym_0036
    ImVec2 zkBase = (t<0.2f)? ImVec2(96,96) : ImVec2(88,88);
    ImVec2 zkSize = ImVec2(zkBase.x * zkPressScale, zkBase.y * zkPressScale);

    // (Removed minimize/close header buttons)

    // Content with scroll
    if (te>0.1f) {
        float leftW = leftW_lang; ImVec2 avail = ImGui::GetContentRegionAvail();
        // Top action bar inside the window; align content to the right of the ZK inside overlap
        float fracOutside = 1.0f/3.0f; float insideOverlap = zkSize.y * (1.0f - fracOutside);
        float barH = insideOverlap + 8.0f;
        ImGui::BeginChild("ZK_TopBar", ImVec2(avail.x, barH), false, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoBackground);
        float leftOffset = zkSize.x * (1.0f - fracOutside) + 12.0f;
        ImGui::SetCursorPosX(leftOffset);
        float availBar = ImGui::GetContentRegionAvail().x;
        float spacing = 8.0f;
        float btnW = (availBar - spacing) * 0.5f;
        float btnH = barH - 10.0f;
        {
            PushConnectButtonColors();
            std::string lbl = GetConnectLabel();
            bool pressed = ImGui::Button(lbl.c_str(), ImVec2(btnW, btnH));
            ImGui::PopStyleColor(3);
            if (pressed) TryConnectMatrix();
        }
        ImGui::SameLine();
        if (ImGui::Button(i18n::TL("Kill").c_str(), ImVec2(btnW, btnH))) { exit(0); }
        ImGui::EndChild();
        ImVec2 avail2 = ImGui::GetContentRegionAvail();
        // sym_0037 sym_0038 sym_0039 90°: sym_0040 sym_0041 sym_0042 sym_0043, sym_0044 sym_0045
        ImGui::BeginChild("ZK_TopTabs", ImVec2(avail2.x, 64), true, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse);
        float tabsBtnH = 52.0f; bool first=true;
        auto TabBtn = [&](const char* label, int id){ if (!first) ImGui::SameLine(); first=false; if (ImGui::Button(label, ImVec2(0, tabsBtnH))) Tab=id; };
        TabBtn(i18n::TL("Main").c_str(), 6);
        TabBtn(i18n::TL("ESP").c_str(), 1);
        TabBtn(i18n::TL("Aimbot").c_str(), 2);
        TabBtn(i18n::TL("Items").c_str(), 4);
        TabBtn(i18n::TL("ImGui Settings").c_str(), 7);
        ImGui::EndChild();
        const char* rightName = (Tab==6)?"ZK_Right_Main" : (Tab==1)?"ZK_Right_ESP" : (Tab==2)?"ZK_Right_Aim" : (Tab==4)?"ZK_Right_Items" : (Tab==7)?"ZK_Right_ImGui" : "ZK_Right_Other";
        ImGui::BeginChild(rightName, ImVec2(avail2.x, avail2.y-4-64), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (Tab==6) {
            // Enable touch-driven scroll on Android for the main settings tab
            TouchScroll(true);
            ImGui::Text("%s", i18n::TL("Settings").c_str());
            ImGui::Separator();
            ImGui::Text("%s", i18n::TL("Language").c_str()); ImGui::SameLine();
            bool en = (i18n::GetLanguage() == i18n::Lang::EN);
            if (ImGui::RadioButton(i18n::TL("English").c_str(), en)) i18n::SetLanguage(i18n::Lang::EN);
            ImGui::SameLine();
            bool ru = (i18n::GetLanguage() == i18n::Lang::RU);
            if (ImGui::RadioButton(i18n::TL("Russian").c_str(), ru)) i18n::SetLanguage(i18n::Lang::RU);
            ImGui::Separator();
            {
                PushConnectButtonColors();
                std::string conn_lbl = GetConnectLabel();
                bool pressed = ImGui::Button(conn_lbl.c_str(), ImVec2(-1, 64));
                ImGui::PopStyleColor(3);
                if (pressed) TryConnectMatrix();
            }
            ImGui::Separator();
            ImGui::Text("%s", i18n::TL("Target FPS").c_str());
            SliderFloat ("Target FPS", &NumIo[12], 30.0f, 240.0f, "%.0f");
            if (ImGui::Button(i18n::TL("ScreenShot").c_str(), ImVec2(-1, 64))) {
                time_t now = time(nullptr);
                string fn = "/sdcard/screenshot_" + to_string((long long)now) + ".png";
                string cmd = "screencap -p " + fn + " &";
                system(cmd.c_str());
            }
            static bool rec=false; static float recSec=30.0f;
            ImGui::SliderFloat("Duration", &recSec, 5.0f, 180.0f, "%.0fs");
            if (ImGui::Button(rec? "Stop recording" : "Start recording", ImVec2(-1,64))) {
                rec=!rec;
                if(rec){ int rn=rand()%99+1; string file="/sdcard/zk_"+to_string(rn)+".mp4"; string cmd="screenrecord --time-limit "+to_string((int)recSec)+" "+file+" &"; system(cmd.c_str()); }
                else { system("pkill -l SIGINT screenrecord"); }
            }
            if (ImGui::Button("Save config", ImVec2(-1, 48))) { NumIoSave("ZKCFG"); }
            if (ImGui::Button("Load config", ImVec2(-1, 48))) { NumIoLoad("ZKCFG"); }
        } else if (Tab==1) {
            // ESP settings page (full feature toggles)
            sym_0046();
        } else if (Tab==2) {
            // Aimbot full settings restored
            sym_0047();
        } else if (Tab==4) {
            // Restore legacy/complete Items controls
            ImGui::Text("%s", i18n::TL("sym_0048 sym_0049").c_str());
            ImGui::Separator();
            // Reuse the original Items renderer to bring back all functions
            sym_0050();
            // Extra toggles for ammo and magazines (separate from vehicles)
            ImGui::Separator();
            ImGui::TextUnformatted(i18n::TL("Weapons").c_str());
            Toggle("  Ammo", sym_0001[17]);
            Toggle("  Magazine", sym_0001[18]);
            // Airdrop control (separate)
            Toggle("  Airdrop", DrawIo[91]);
            ImGui::Separator();
            ImGui::Checkbox(i18n::TL("Developer: show raw item names").c_str(), &g_items_dev_mode);
            ImGui::TextUnformatted(i18n::TL("Use top-center buttons to add/load items").c_str());
        } else if (Tab==7) {
            // ImGui settings page
            ImGui::Text("%s", i18n::TL("ImGui Settings").c_str());
            ImGui::Separator();
            ImGui::Text("%s", i18n::TL("Transparency").c_str());
            ImGui::SliderFloat("Alpha", &g_ui_alpha, 0.10f, 1.0f, "%.2f");
        }
        ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleVar(); // global alpha

    // Draw ZK button as an overlay window protruding 1/3 outside top-left corner
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f); // always fully visible
    ImGui::SetNextWindowPos(ImVec2(g_zkBasePos.x - zkSize.x * (1.0f/3.0f), g_zkBasePos.y - zkSize.y * (1.0f/3.0f)), ImGuiCond_Always);
    ImGui::SetNextWindowSize(zkSize, ImGuiCond_Always);
    ImGuiWindowFlags oflags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("ZKOverlay", nullptr, oflags);
    ZKButtonResult btn = DrawZKLogoButton(ImGui::GetContentRegionAvail());
    if (!btn.active) zkHoldSec = 0.0f;
    zkWasActive = btn.active;
    static ImVec2 pressMouse, pressBasePos;
    static bool   draggingZK = false;
    constexpr float dragThreshold = 6.0f;

    if (ImGui::IsItemActivated()) {
        pressMouse = ImGui::GetMousePos();
        pressBasePos = g_zkBasePos;
        draggingZK = false;
    }

    if (btn.active) {
        ImVec2 delta = ImGui::GetMousePos() - pressMouse;
        if (!draggingZK && (fabsf(delta.x) > dragThreshold || fabsf(delta.y) > dragThreshold)) {
            draggingZK = true;
        }
        if (draggingZK) {
            g_zkBasePos = ImVec2(pressBasePos.x + delta.x, pressBasePos.y + delta.y);
        }
    }

    if (ImGui::IsItemDeactivated()) {
        if (!draggingZK) {
            state = (state==ZKState::Expanded||state==ZKState::Expanding) ?
                ZKState::Collapsing : ZKState::Expanding;
        }
        draggingZK = false;
    }
    static ImVec2 dragMouse, dragPos;
    if (btn.active && ImGui::IsMouseClicked(0)) { dragMouse = ImGui::GetMousePos(); dragPos = g_zkBasePos; }
    bool dragging = btn.active && ImGui::IsMouseDragging(0, 2.0f);
    if (dragging) { ImVec2 d = ImGui::GetMousePos()-dragMouse; g_zkBasePos = ImVec2(dragPos.x+d.x, dragPos.y+d.y); }
    if ((btn.clicked || ImGui::IsItemClicked(0)) && !dragging) {
        state = (state==ZKState::Expanded||state==ZKState::Expanding)? ZKState::Collapsing: ZKState::Expanding;
    }
    ImGui::End();
    ImGui::PopStyleVar();
}
static AnimationMode currentAnimationMode = ANIM_HORIZONTAL;
static bool showAnimationSettings = false;

NumberAnimation playerAnim;
NumberAnimation botAnim;
static float playerCountAnim = 0.0f;
static float botCountAnim = 0.0f;
static int prevPlayerCount = 0;
static int prevBotCount = 0;
static time_t countChangeTime = 0;
static bool isDotsMoving = false;
static float dotsAnimationProgress = 0.0f;
static ImVec2 dotsStartPos[3];
static ImVec2 dotsEndPos[3];
static time_t dotsAnimationStartTime = 0;
vector<string> lyrics;
int currentLyricIndex = 0;
float lyricScrollPos = 0.0f;
float lyricScrollSpeed = 30.0f;
bool isLyricPlaying = false;
time_t lyricStartTime = 0;
timer RenderingFPS;
static float g_last_fps = 0.0f;
static bool isRecordingCircleTouched = false;
float sym_0051 = false;
static bool isTimerRunning = false;
static int timerStartTime = 0;
static float sym_0052 = 30;
float globalFontScale = 1.3f; 
struct FontSettings {
    float scale = 1.0f;
    float minScale = 0.5f;
    float maxScale = 2.0f;
    bool dirty = false;
    
    void SetScale(float newScale) {
        scale = std::clamp(newScale, minScale, maxScale);
        dirty = true;
    }
    
    void ApplyScale(ImGuiIO& io) {
        if (dirty) {
            io.FontGlobalScale = scale;
            dirty = false;
        }
    }
};

static FontSettings globalFontSettings;

ImTextureID logo_png, aim_png;
bool sym_0053;
bool DrawIo[50];
float NumIo[50];
bool sym_0001[50];
ImColor sym_0054[50];
ImColor sym_0055[16];
float sym_0056[16];
float draw_style[20];
float fwjl = NumIo[3];
DataReader reader;
ThreadPool threadPool(4);
Data fanhua;
WuziData wz;
string wht, flp;
void Draw_Meun();
void AimBotAuto();
void NumIoLoad(const char *name);
void Draw_Main(ImDrawList *Draw);
// Removed game launch helper to avoid unintended starts and terminal output
// void sym_0057() { }

const char* color_codes[] = {
    "\033[31;1m",
    "\033[32;1m",
    "\033[33;1m",
    "\033[34;1m",
    "\033[35;1m"
    "\033[36;1m"
};


int main() {
    wht = "2";
    
    flp = "2";
    screen_config(); 
    init_screen_x = screen_x + screen_y;
    init_screen_y = screen_y + screen_x;
    if(!init_vulkan(init_screen_x, init_screen_y, false)) {
        exit(0);
    }

    draw_style[10] = 5;
    NumIoLoad("sym_0058");

    if (draw_style[10] == 5) {
        draw_style[10] = 1;
        // game auto-start removed
    }
    srand((unsigned int)time(NULL));
    int random_index = rand() % 6;
    printf("%s", color_codes[random_index]);
    
    // suppress terminal output
    if(!init_vulkan(init_screen_x, init_screen_y, false)) {
        exit(0);
    }
lyrics = {
    "sym_0059",
    "sym_0060",
    "sym_0061",
    "sym_0062",
    "sym_0063",
    "sym_0064",
    "sym_0065",
    "sym_0066",
    "sym_0067",
    "sym_0068",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072",
    "sym_0073",
    "sym_0074",
    "sym_0075",
    "sym_0076",
    "sym_0063",
    "sym_0064",
    "sym_0065",
    "sym_0066",
    "sym_0067",
    "sym_0068",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072",
    "sym_0063",
    "sym_0064",
    "sym_0065",
    "sym_0066",
    "sym_0067",
    "sym_0068",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072",
    "sym_0069 sym_0070",
    "sym_0071 sym_0072"
};
    lyrics.clear();
    InitAnimationData();
    ImGui_init();
    TouchScreenHandle();
    std::thread aim_thread(AimBotAuto);
    aim_thread.detach();
    window_set();
    logo_png = VK.ImAgeHeadFile(touxiang, sizeof(touxiang));
    sym_0077();
    RenderingFPS.AotuFPS_init();
    RenderingFPS.setAffinity();

    ImGui_ImplAndroid_NewFrame(init_screen_x, init_screen_y);

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();
       
        RenderingFPS.SetFps((unsigned int)std::max(1.0f, NumIo[12]));
        float frame_ms = RenderingFPS.AotuFPS();
        if (frame_ms > 0.0f && frame_ms < 1000.0f) {
            g_last_fps = 1000.0f / frame_ms;
        }
        VK_Begin();
        if (fanhua.pid > 0 && fanhua.libUE4 > 0)
            Draw_Main(ImGui::GetForegroundDrawList());
        Draw_Meun();
        VK_End();
        (void)start; // no manual sleep here; Vulkan loop paced by timer
    }
    shutdown();
    return 0;
    
}
void CleanData() {
draw_style[1] = 0;
// Default: all features off; user enables explicitly
DrawIo[1] = false;
DrawIo[2] = false;
DrawIo[3] = false;
DrawIo[5] = false;
DrawIo[6] = false;
DrawIo[7] = false;
DrawIo[8] = false;
DrawIo[10] = false;
DrawIo[14] = false;
DrawIo[17] = false;
DrawIo[20] = false;
DrawIo[21] = false;
DrawIo[25] = false;
DrawIo[31] = false;
DrawIo[32] = false;
DrawIo[37] = false;
DrawIo[40] = false;
DrawIo[44] = false;
DrawIo[45] = false;
DrawIo[46] = false;
DrawIo[49] = false;
DrawIo[88] = false;
DrawIo[91] = false;
DrawIo[910] = false;
// Reset item toggles
for (int i=0;i<50;++i) sym_0001[i] = false;
sym_0054[0] = ImColor(255, 255, 255, 255);
sym_0054[1] = ImColor(255, 255, 255, 255);
sym_0054[2] = ImColor(255, 255, 255, 255);
sym_0054[3] = ImColor(255, 255, 255, 255);
sym_0054[4] = ImColor(255, 255, 255, 255);
sym_0054[5] = ImColor(255, 255, 255, 255);
sym_0054[6] = ImColor(255, 255, 255, 255);
sym_0054[7] = ImColor(255, 255, 255, 255);
// Neutral default colors
sym_0054[8] = ImColor(255, 255, 255, 255);
sym_0054[9] = ImColor(255, 255, 255, 255);
sym_0054[10] = ImColor(255, 255, 255, 255);
sym_0054[11] = ImColor(255, 255, 255, 255);
sym_0054[12] = ImColor(255, 255, 255, 255);
sym_0054[13] = ImColor(255, 255, 255, 255);
sym_0054[14] = ImColor(255, 255, 255, 255);
sym_0054[15] = ImColor(255, 255, 255, 255);
sym_0054[16] = ImColor(255, 255, 255, 255);
sym_0054[17] = ImColor(255, 255, 255, 255);
sym_0054[18] = ImColor(255, 255, 255, 255);
sym_0054[19] = ImColor(255, 255, 255, 255);
sym_0054[20] = ImColor(255, 255, 255, 255);
sym_0055[0] = ImColor(255, 0, 0, 150);
sym_0055[1] = {1.0f,0.0f,0.0f,1.0f};
sym_0055[2] = ImColor(255,255,255,255);
sym_0055[3] = ImColor(0,0,0,25);
sym_0055[4] = ImColor(0,0,0,0);
sym_0055[5] = ImColor(255,0,0,255);
sym_0055[6] = ImColor(0,255,0,255);
sym_0055[7] = ImColor(173,216,230,255);
sym_0055[8] = ImColor(255,255,255,255);
sym_0056[1] = 1.5f;
sym_0056[2] = 1.5f;
sym_0056[3] = 1.5f;
sym_0056[4] = 1.5f;
sym_0056[5] = 2.5f;
sym_0056[6] = 2.5f;
NumIo[1] = 300.0f;  
NumIo[2] = 400.0f;
NumIo[3] = 120.0f;
NumIo[4] = 17.0f;
NumIo[6] = 1400.0f;
NumIo[5] = 650.0f;
NumIo[7] = 300.0f;  
NumIo[8] = 0.0f;  
NumIo[9] = 4.4f; 
NumIo[10] = 0.0f;  
NumIo[11] = 600.0f;
NumIo[12] = 90;
NumIo[13] = 0.0f;
NumIo[14] = 1.0f;
NumIo[15] = 0.0f;
NumIo[16] = 100.0f;
NumIo[17] = 400.0f;
NumIo[19] = 150.0f;
NumIo[20] = 0.0f;
NumIo[21] = 0.0f;
NumIo[22] = 1.0f;
NumIo[23] = 255.0f;
draw_style[3] = 120;
draw_style[7] = 1;
draw_style[8] = 100;
draw_style[9] = 100;
draw_style[12] = 0;
style_bw = 0;
style_zx = 0;
style_idx = 0;
style_cd = 0;


}

#include "Android_draw/sym_0078.h"
#define AIMBOT_SEPARATED
#include "Aim/Aim.h"
int AimCount, MaxPlayerCount, Gmin = -1;
float zm_x, zm_y;
bool IsAimLongAim = false;
char AimName[32];
Vec2 vpvp;
AimStruct Aim[100];

void Draw_Main(ImDrawList *Draw) {
    if (draw_style[10] == 0) {
      system("rm -rf /data/sym_0079");
      exit(1);
    } else if (draw_style[10] == 1) {
    // Reset developer watch each frame before scanning objects
    g_dev_watch_best = 1e12f; g_dev_watch_name[0] = '\0';
    #include "Android_draw/hook.h"
}
if(DrawIo[45] && fanhua.TotalUp){
if (DrawIo[25] && DrawIo[45] && Gmin != -1) {
Draw->AddCircle({screen_x / 2, screen_y / 2}, fwjl, ImColor(255,255,255,255), 0, 2.0f);
} else if(DrawIo[45]) {
Draw->AddCircle({screen_x / 2, screen_y / 2}, NumIo[3], ImColor(255,255,255,255), 0, 2.0f); 
}
}
if(DrawIo[44]){
if (Gmin != -1) {
Draw->AddLine({screen_x / 2, screen_y / 2}, {vpvp.x, vpvp.y}, ImColor(255,255,255,255), 3.0f);
}
}
if (DrawIo[37] && Gmin != -1) {
   
    float distanceToTarget = sqrtf(powf(vpvp.x - screen_x/2, 2) + powf(vpvp.y - screen_y/2, 2));
    if (distanceToTarget < NumIo[3]) {
        if (fanhua.IsBot == 1) {
            sym_0055[5] = ImColor(255, 255, 0);
            sym_0055[6] = ImColor(255, 255, 0);
        } else {
            sym_0055[5] = ImColor(255, 255, 0);
            sym_0055[6] = ImColor(255, 255, 0);
        }
    } else {
        sym_0055[5] = ImColor(255, 255, 255);
        sym_0055[6] = ImColor(255, 255, 255);
    }
}
if (DrawIo[21]) {
string ssf;  
ssf += i18n::TL("sym_0080，sym_0081");
auto textSize = ImGui::CalcTextSize(ssf.c_str(), 0, 32);
Draw->AddRectFilled({0,0}, {screen_x, screen_y},ImColor(0,0,0,110));
Draw->AddRectFilled({NumIo[6] - NumIo[7] / 2, screen_y - NumIo[5] + NumIo[7] / 2}, {NumIo[6] + NumIo[7] / 2, screen_y - NumIo[5] - NumIo[7] / 2}, sym_0055[0]); 
Draw->AddText(NULL, 32, {NumIo[6] - (textSize.x / 2), screen_y - NumIo[5]}, ImColor(255, 255, 255), ssf.c_str());
}
char *CasName;

for (int i = 0; i < wz.Count; i++)
{

if (wz.w>0){
if (DrawIo[11]&&GetVehicleInfo(wz.Name, &CasName)) {//sym_0082
string name;
// Prefer localized class label if available
std::string loc = i18n::FromClassName(wz.Name);
name += loc.empty() ? std::string(CasName) : loc;
name += "[";
name += to_string((int)wz.Distance);
name += (i18n::GetLanguage() == i18n::Lang::RU ? "sym_0083]" : "m]");
auto textSize = ImGui::CalcTextSize(name.c_str(),0, 20);
Draw->AddText(NULL, 20,{wz.ScreenPos.x-(textSize.x / 2), wz.ScreenPos.y}, ImColor(255, 255, 255, 255), name.c_str());
}
}
if (wz.w>0){
if (DrawIo[91]&&sym_0084(wz.Name, &CasName)) {
string name;
std::string loc = i18n::FromClassName(wz.Name);
name += loc.empty() ? std::string(CasName) : loc;
name += "[";
name += to_string((int)wz.Distance);
name += (i18n::GetLanguage() == i18n::Lang::RU ? "sym_0083]" : "m]");
auto textSize = ImGui::CalcTextSize(name.c_str(),0, 20);
Draw->AddText(NULL, 20,{wz.ScreenPos.x-(textSize.x / 2), wz.ScreenPos.y}, ImColor(255, 255, 255, 255), name.c_str());
}
}

            }
            
if (DrawIo[7]) {
ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(NumIo[1] - NumIo[19], NumIo[2] - NumIo[19]),  ImVec2(NumIo[1] + NumIo[19], NumIo[2] + NumIo[19]), ImColor(255, 255, 255, 70), 10.0f );
ImGui::GetForegroundDrawList()->AddRect({NumIo[1] - NumIo[19],NumIo[2]-NumIo[19]},{NumIo[1] +NumIo[19],NumIo[2]+NumIo[19]},ImColor(255,255,255,125),10,0,4.0f);

ImVec2 pointss[4] = {
ImVec2(NumIo[1] - 10.0f, NumIo[2] + 14.0f),
ImVec2(NumIo[1], NumIo[2] - 12.0f),
ImVec2(NumIo[1] + 10.0f, NumIo[2] + 14.0f),
ImVec2(NumIo[1], NumIo[2] + 8.0f)
};


   float centerX = NumIo[1];
    float centerY = NumIo[2];
    float radius = NumIo[19];
    
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    
    drawList->AddCircleFilled(ImVec2(centerX, centerY), 8.0f, ImColor(0, 255, 0, 255));
    
    ImVec2 topPoint(centerX, centerY - radius);
    drawList->AddLine(ImVec2(centerX, centerY), topPoint, ImColor(0, 255, 0, 255), 2.0f);
   
    float arrowSize = 10.0f;
    drawList->AddTriangleFilled(
        ImVec2(topPoint.x - arrowSize, topPoint.y + arrowSize),
        ImVec2(topPoint.x + arrowSize, topPoint.y + arrowSize),
        topPoint,
        ImColor(0, 255, 0, 255)
    );
    }

}

void sym_0085() {
    struct LerpItem {
        float& beg;
        float& end;
        float factor;
    };   
    LerpItem items[] = {
        {sym_0086.beg, sym_0086.end, 0.111f},
        {sym_0087.beg, sym_0087.end, 0.1f},
        {sym_0088.beg, sym_0088.end, 0.05f},
        {sym_0089.beg, sym_0089.end, 0.1f},
        {sym_0090.beg, sym_0090.end, 0.06f},
        {sym_0091.beg, sym_0091.end, 12.0f},
        {sym_0092.beg, sym_0092.end, 0.06f},
        {sym_0093.beg, sym_0093.end, 0.06f},
        {homepage, homepages, 0.06f},
        {setup, setups, 0.08f},
        {sym_0094.beg, sym_0094.end, 0.04f},
        {sym_0095.beg, sym_0095.end, 0.125f},
        {sym_0096.beg, sym_0096.end, 0.1f},
        {sym_0097.beg, sym_0097.end, 0.1f},
        {sym_0098.beg, sym_0098.end, 0.125f},
        {sym_0099.beg, sym_0099.end, 0.125f}
    };    
    for (auto& item : items) {
        Lerp(item.beg, item.end, item.factor);
    }
}

float sym_0100(float t, float p0, float p1, float p2, float p3) {
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    float res = uuu * p0;
    res += 3 * uu * t * p1;
    res += 3 * u * tt * p2;
    res += ttt * p3;
    return res;
}

void Draw_Meun() {
    // Boot animation: draw only animation; UI appears after it's done
    if (RenderBootAnimation()) return;
    // Top-center counter overlay
    DrawTopCenterCounter();
    // Developer overlay buttons under the counter
    if (g_items_dev_mode) {
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::SetNextWindowPos(ImVec2(screen_x*0.5f, 28.0f + 58.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::Begin("DevItemBar", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize);
        const char* watch = (g_dev_watch_name[0] != '\0') ? g_dev_watch_name : "(no target)";
        if (ImGui::Button(watch, ImVec2(320, 40))) {
            AppendItemOverrideLine(watch);
        }
        ImGui::SameLine();
        if (ImGui::Button(i18n::TL("Load items").c_str(), ImVec2(160, 40))) {
            LoadItemOverridesFile();
            g_items_draw_overrides = true;
        }
        ImGui::End();
    }
    // FPS overlay (top-right) when enabled in ESP
    if (DrawIo[40]) {
        float shownFps = g_last_fps > 0.0f ? g_last_fps : ImGui::GetIO().Framerate;
        char fpsText[48];
        snprintf(fpsText, sizeof(fpsText), "FPS: %.0f", shownFps);
        ImVec2 ts = ImGui::CalcTextSize(fpsText);
        ImGui::GetForegroundDrawList()->AddText(
            ImVec2(screen_x - ts.x - 12.0f, 12.0f), ImColor(255,255,255,255), fpsText);
    }
    // Permanent TG signature (bottom-left), cannot be disabled
    {
        const char* tg = "TG@ZKERNEL";
        ImVec2 ts = ImGui::CalcTextSize(tg);
        float x = 12.0f;
        float y = screen_y - ts.y - 12.0f;
        ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y), ImColor(255,0,0,255), tg);
    }
    RenderZKRoot();
    return;
   if (DrawIo[20]) {
        float fps = ImGui::GetIO().Framerate;
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "DrawFps: %.1f", fps);
        ImVec2 textSize = ImGui::CalcTextSize(fpsText);
        
        ImVec2 fpsPos(50.0f, screen_y - textSize.y - 100.0f);
        ImGui::GetForegroundDrawList()->AddText(
            fpsPos,
            ImColor(255, 255, 255, 255),
            fpsText
        );
        char aimText[128];
        snprintf(aimText, sizeof(aimText), 
                "Aim: %.1f", 
                NumIo[3]);
        ImVec2 aimTextSize = ImGui::CalcTextSize(aimText);
        ImVec2 aimPos(50.0f, fpsPos.y + textSize.y + 10.0f);
        ImGui::GetForegroundDrawList()->AddText(
            aimPos,
            ImColor(255, 255, 255, 255),
            aimText
        );
        }
    targetScale = sym_0101 ? 1.0f : 0.0f;
    UpdateAnimation(animationScale, targetScale, animationDamping);
    if (!sym_0101 && animationScale < 0.01f) {
        animationScale = 0.0f;
    }
    float animatedWindowWidth = 980.0f * animationScale;
    float animatedWindowHeight = 700.0f * animationScale;    
    sym_0102();
    sym_0085();
    if (sym_0101) {
        sym_0088.end = 10;
        sym_0090.end = 1;
        sym_0103.beg = 0;
    } else {
        sym_0088.end = 0;
        sym_0090.end = 0;
        sym_0103.beg = 50;
        if (Tab == 3) {
            sym_0086.end = 0 - sym_0093.beg;
        } else {
            sym_0086.end = 0;
        }
        sym_0087.end = 0;
    }
    if (animationScale > 0.01f) {
        float sym_0104 = (screen_x - animatedWindowWidth + sym_0092.beg - sym_0093.beg) / 2.0f;
        float sym_0105 = (screen_y - animatedWindowHeight) / 2.0f;
        if (sym_0090.beg < 0.01f) windows = 0; 
        else windows = 1;
        
        if (sym_0090.beg > 0.99f) sym_0106 = 12; 
        else sym_0106 = 12;
        
        if (sym_0090.beg > 0.9f) {
            sym_0086.end = 980.0f;
            sym_0087.end = 700.0f;
        } else {
            if (Tab == 3) {
                sym_0086.end = 50 - sym_0093.beg;
            } else {
                sym_0086.end = 50;
            }
            sym_0087.end = 50;
        }
        cloud += (white - cloud) * (0.05f * speed);
        for (auto& item : wh) {
            Lerp(item.second.beg, item.second.end, 0.05f * speed);
        }
    if (Tab == 0) {
        white = 1000;
        wh["win_x_0"].end = 0;
        wh["win_x_1"].end = 0;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 750;
        wh["win_y_2"].end = 750;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 1) {
        white = 45;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 0;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 45;
        wh["win_y_2"].end = 750;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 2) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 45;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 4) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 50;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 15;
        wh["win_y_3"].end = 45;
        wh["win_y_4"].end = 750;
    } else if (Tab == 6) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 50;
        wh["win_x_3"].end = 50;
        wh["win_x_4"].end = 0;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 15;
        wh["win_y_3"].end = 15;
        wh["win_y_4"].end = 45;
    }


        ImGui::SetNextWindowPos({sym_0104, sym_0105}, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(animatedWindowWidth - sym_0092.beg + sym_0093.beg, animatedWindowHeight), ImGuiCond_Always);
        if (window) {
            zhu_set();
            ImGui::Begin("sym_0107", &window, 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoBackground | 
            ImGuiWindowFlags_NoMove);

        ImGui::PopStyleVar(); 
        ImGui::PopStyleColor();
        aevrage_all = window_x / 6.302521f; 
        aevrage_now = ImGui::GetWindowSize().x / 6.302521f; 
        tab0 = window_y * 0;
        tab1 = window_y * 1;
        tab2 = window_y * 2;
        tab3 = window_y * 3;
        tab4 = window_y * 4;
        tab5 = window_y * 5;
        tab6 = window_y * 6;

        ImGui::SetCursorPos(ImVec2(0, 20));
        ImGui::BeginChild("sym_0108", ImVec2(aevrage_now*1.5, 670), true);
        ImDrawFlags roundingFlags = ImDrawFlags_RoundCornersLeft;
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetWindowPos(), 
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            ImColor(230, 230, 230, 255),
            15.0f,
            roundingFlags
        );
        
        ImVec2 windowPos = ImGui::GetWindowPos();
        // Window controls: Minimize and Close (Windows-style)
        ImGui::SetCursorPos(ImVec2(10, 8));
        // Minimize button (gray, Win10/11 style)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.85f, 0.88f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.90f, 0.93f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80f, 0.80f, 0.85f, 1.0f));
        if (ImGui::Button("-", ImVec2(32, 24))) { window = false; }
        ImGui::SameLine();
        // Close button (red, Win10/11 style)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80f, 0.15f, 0.15f, 1.0f));
        if (ImGui::Button("X", ImVec2(32, 24))) { exit(0); }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleColor(3);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); 

        if (button("Main", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 6;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("sym_0109 sym_0110", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 1;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("sym_0111 sym_0112", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 2;        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("sym_0048 sym_0049", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 4;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
 // Settings moved above
        ImGui::EndChild();

        const float spacing = -2.0f;
        const float rightContentWidth = aevrage_now*4.4 - spacing;
        
        ImGui::SetCursorPos(ImVec2(aevrage_now*1.5 + spacing, 20));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 20.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        
       ImGui::BeginChild("sym_0113", ImVec2(rightContentWidth, 670), true, 
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetWindowPos(), 
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            ImColor(255, 255, 255, 255),
            15.0f,
            ImDrawFlags_RoundCornersRight
        );

        static float win_y = ImGui::GetCursorPosY();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_0"].beg/2, win_y + 15)); 
        ImGui::BeginChild("sym_0114", ImVec2(rightContentWidth - wh["win_x_0"].beg, 720), true);
        if (Tab == 0) sym_0115(); 
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_1"].beg/2, win_y + wh["win_y_1"].beg));
        ImGui::BeginChild("sym_0116", ImVec2(rightContentWidth - wh["win_x_1"].beg, 690), true);
        if (Tab == 1) sym_0046();
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_2"].beg/2, win_y + wh["win_y_2"].beg));
        ImGui::BeginChild("sym_0117", ImVec2(rightContentWidth - wh["win_x_2"].beg, 690), true);
        if (Tab == 2) { }
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_3"].beg/2, win_y + wh["win_y_3"].beg));
        ImGui::BeginChild("sym_0118", ImVec2(rightContentWidth - wh["win_x_3"].beg, 690), true);
        if (Tab == 4) sym_0050();
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_4"].beg/2, win_y + wh["win_y_4"].beg));
        ImGui::BeginChild("sym_0119", ImVec2(rightContentWidth - wh["win_x_4"].beg, 690), true);
        if (Tab == 6) sym_0120();
        ImGui::EndChild();
         if (Tab == 6) ui_general::Draw_GeneralSettingsEmbedded();
        
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::End();       
}
if (!sym_0101 && animationScale <= 0.0f) {
windows = 0;
}
}
}


bool sym_0121[20];
void sym_0115() {
    TouchScroll(true);
    CenteredText(i18n::TL("sym_0122 sym_0123").c_str(), 1.5f);
    ImGui::Spacing();
    // Main shows embedded settings
    ui_general::Draw_GeneralSettingsEmbedded();
    return;

    const float childWidth = ImGui::GetContentRegionAvail().x * 0.9f;
    const float childPosX = (ImGui::GetContentRegionAvail().x - childWidth) / 2.0f;
    ImGui::SetCursorPosX(childPosX);
    
    ImGui::BeginChild("sym_0124", ImVec2(childWidth, 170), true, ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetWindowPos(), 
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, 
                  ImGui::GetWindowPos().y + ImGui::GetWindowSize().y), 
            ImColor(230, 230, 230, 150), 
            20.0f
        );
        
        ImGui::Spacing();
        CenteredText(i18n::TL("sym_0125").c_str(), 1.0f);
        std::string noticeText = i18n::TL("sym_0126\sym_0127、sym_0128\sym_0129、sym_0130");
        CenteredText(noticeText.c_str(), 1.0f);
    }
    ImGui::EndChild();
    static bool firstPlay = true;
    if (firstPlay) {
        isLyricPlaying = true;
        lyricStartTime = time(nullptr);
        firstPlay = false;
    }
    
    struct LyricState {
        float alpha = 0.0f;
        float scale = 1.0f;
        float yOffset = 0.0f;
        ImColor color;
        bool isCurrent = false;
        bool isPrevious = false;
        time_t fadeStartTime = 0;
    };
    
    static std::vector<LyricState> lyricStates(lyrics.size());
    time_t currentTime = time(nullptr);
    float elapsed = difftime(currentTime, lyricStartTime);
    
    if (elapsed >= 3.5f) {
        lyricStartTime = currentTime;
        
        if (currentLyricIndex >= 0 && currentLyricIndex < lyrics.size()) {
            lyricStates[currentLyricIndex].isCurrent = false;
            lyricStates[currentLyricIndex].isPrevious = true;
            lyricStates[currentLyricIndex].fadeStartTime = currentTime;
        }
              
        currentLyricIndex = (currentLyricIndex + 1) % lyrics.size();
        
        lyricStates[currentLyricIndex].isCurrent = true;
        lyricStates[currentLyricIndex].isPrevious = false;
        lyricStates[currentLyricIndex].alpha = 1.0f;
        lyricStates[currentLyricIndex].scale = 1.5f;
        lyricStates[currentLyricIndex].color = ImColor(255, 0, 0, 255);
    }
    
    for (int i = 0; i < lyrics.size(); i++) {
        if (lyricStates[i].isPrevious) {
            float fadeElapsed = difftime(currentTime, lyricStates[i].fadeStartTime);
            float fadeProgress = fadeElapsed / 1.0f;
            
            if (fadeProgress >= 1.0f) {
                lyricStates[i].isPrevious = false;
                lyricStates[i].alpha = 0.0f;
            } else {
                lyricStates[i].alpha = 1.0f - fadeProgress;
                lyricStates[i].scale = 1.0f + (0.3f * fadeProgress);
                lyricStates[i].yOffset = 30.0f * fadeProgress;
            }
        }
    }
 
    float animationSpeed = 5.0f * ImGui::GetIO().DeltaTime;
    for (int i = 0; i < lyrics.size(); i++) {
        if (!lyricStates[i].isPrevious) {
            lyricStates[i].alpha = ImLerp(lyricStates[i].alpha, (lyricStates[i].isCurrent) ? 1.0f : 0.7f, animationSpeed);
            lyricStates[i].scale = ImLerp(lyricStates[i].scale, (lyricStates[i].isCurrent) ? 1.3f : 1.0f, animationSpeed);
            lyricStates[i].yOffset = ImLerp(lyricStates[i].yOffset, (lyricStates[i].isCurrent) ? 0.0f : 30.0f, animationSpeed);
        }
    }
    ImVec2 windowPos = ImGui::GetWindowPos();
    float startX = windowPos.x + 20.0f;
    float startY = windowPos.y + 260.0f;
    if (!lyrics.empty() && currentLyricIndex >= 0 && currentLyricIndex < lyrics.size()) {
        const char* currentLyric = lyrics[currentLyricIndex].c_str();
        float currentFontSize = 40.0f * lyricStates[currentLyricIndex].scale;
        ImGui::GetWindowDrawList()->AddText(
            NULL, 
            currentFontSize, 
            ImVec2(startX, startY + lyricStates[currentLyricIndex].yOffset), 
            ImColor(lyricStates[currentLyricIndex].color.Value.x, 
                   lyricStates[currentLyricIndex].color.Value.y, 
                   lyricStates[currentLyricIndex].color.Value.z, 
                   lyricStates[currentLyricIndex].color.Value.w * lyricStates[currentLyricIndex].alpha), 
            currentLyric
        );
        for (int i = 1; i <= 3; i++) {
            int nextIndex = (currentLyricIndex + i) % lyrics.size();
            if (nextIndex >= 0 && nextIndex < lyrics.size()) {
                const char* nextLyric = lyrics[nextIndex].c_str();
                float nextFontSize = currentFontSize * 0.8f;
                float yOffset = 80.0f * i;
                
                ImGui::GetWindowDrawList()->AddText(
                    NULL, 
                    nextFontSize, 
                    ImVec2(startX, startY + yOffset), 
                    ImColor(0, 0, 0, static_cast<int>(200 * (1.0f - (i * 0.2f)))), 
                    nextLyric
                );
            }
        }
    }
}

void sym_0046() {
    TouchScroll(true);
    CenteredText(i18n::TL("sym_0109 sym_0110").c_str(), 1.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
      
    Toggle(i18n::TL("  sym_0131").c_str(),DrawIo[1]);
    
    Toggle(i18n::TL("  sym_0132").c_str(),DrawIo[3]);
    
    Toggle(i18n::TL("  sym_0133").c_str(),DrawIo[2]);
    
    Toggle(i18n::TL("  sym_0134").c_str(),DrawIo[5]);
    
    Toggle(i18n::TL("  sym_0135").c_str(),DrawIo[7]);
    
    Toggle(i18n::TL("  sym_0136").c_str(),DrawIo[8]);
    
    Toggle(i18n::TL("  sym_0137").c_str(),DrawIo[17]);
    Toggle(i18n::TL("  sym_0138(sym_0139)").c_str(),DrawIo[31]);
    Toggle(i18n::TL("  sym_0140").c_str(),DrawIo[46]);
    
    // Removed: back-aim WIP toggle (always on or deprecated)
    
    Toggle(i18n::TL("  sym_0141").c_str(),DrawIo[32]);
    
    Toggle(i18n::TL("  sym_0142").c_str(),DrawIo[10]);
    
    Toggle(i18n::TL("  sym_0143").c_str(),DrawIo[14]);
    
    // Removed experimental color toggle
    
    Toggle(i18n::TL("  sym_0144").c_str(),DrawIo[6]);
      
    Toggle(i18n::TL("  FPS").c_str(),DrawIo[40]);
    vector<string> backStyleOpts = { i18n::TL("Circle"), i18n::TL("Arrow") };
    whitecloud(backStyleOpts, draw_style[2], 0, 100.0f);
    vector<string> styleOpts = { i18n::TL("Grid"), i18n::TL("Banner"), i18n::TL("iOS Card") };
    whitecloud(styleOpts, draw_style[1], 0, 120.0f);
    ImGui::PopStyleVar(3);
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0145").c_str());
    SliderFloat (i18n::TL("sym_0146").c_str(), &NumIo[1], 10, 3200, "%1.f");
    SliderFloat (i18n::TL("sym_0147").c_str(), &NumIo[2], 10, 1440, "%1.f");
    SliderFloat (i18n::TL("sym_0148").c_str(), &NumIo[19], 10, 300, "%1.f");
    SliderFloat (i18n::TL("sym_0149").c_str(), &NumIo[16], 70, 150, "%.0f%%", 5);
    SliderFloat (i18n::TL("sym_0150").c_str(), &NumIo[91], 10, 300, "%1.f");
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0151").c_str());
    Toggle("  sym_0152",DrawIo[1000]);
    SliderFloat ("sym_0153", &NumIo[49], 0, 100, "%1.f");
}
void sym_0047() {
    TouchScroll(true);
    CenteredText(i18n::TL("sym_0154 sym_0111").c_str(), 1.5f);
    
    Toggle("  sym_0155",DrawIo[20]);
    Toggle("  sym_0156",DrawIo[45]);
    Toggle("  sym_0157",DrawIo[21]);
    Toggle("  sym_0158",DrawIo[49]);
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0159").c_str());
    // Target bone selection (combo)
    const char* boneLabels[] = {
        "Head","Chest","Pelvis","Left shoulder","Right shoulder",
        "Left elbow","Right elbow","Left wrist","Right wrist",
        "Left thigh","Right thigh","Left knee","Right knee",
        "Left ankle","Right ankle"
    };
    int boneIdx = (int)NumIo[8];
    if (boneIdx < 1 || boneIdx > 15) boneIdx = 1;
    std::string current = i18n::TL(boneLabels[boneIdx-1]);
    if (ImGui::BeginCombo(i18n::TL("Target bone").c_str(), current.c_str())) {
        for (int i=0;i<15;i++){
            bool selected = (boneIdx == (i+1));
            std::string label = i18n::TL(boneLabels[i]);
            if (ImGui::Selectable(label.c_str(), selected)) boneIdx = i+1;
            if (selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    NumIo[8] = (float)boneIdx;
    SliderFloat ("sym_0160", &NumIo[3], 10.0f, 800.0f, "%1.f");
    SliderFloat ("sym_0161", &NumIo[9], 1.0f, 50.0f, "%0.1f");
    SliderFloat ("sym_0162", &NumIo[4], 1.0f, 30.0f, "%0.1f");
    SliderFloat ("sym_0163", &NumIo[7], 50.01f, 400.0f, "%0.2f");
    SliderFloat ("sym_0164", &NumIo[14], 0.1f, 5.0f, "%.1f", 5);
    SliderFloat ("sym_0165", &NumIo[22], 0.1f, 5.0f, "%.1f", 5);
    // Trigger mode: 0=Firing,1=Aiming,2=Either,3=Always (maps to Aim.h switch on NumIo[0])
    ImGui::Text("%s", i18n::TL("Trigger").c_str());
    ImGui::SameLine();
    if (ImGui::RadioButton(i18n::TL("Firing").c_str(), (int)NumIo[0]==0)) NumIo[0]=0;
    ImGui::SameLine();
    if (ImGui::RadioButton(i18n::TL("Aiming").c_str(), (int)NumIo[0]==1)) NumIo[0]=1;
    ImGui::SameLine();
    if (ImGui::RadioButton(i18n::TL("Either").c_str(), (int)NumIo[0]==2)) NumIo[0]=2;
    ImGui::SameLine();
    if (ImGui::RadioButton(i18n::TL("Always").c_str(), (int)NumIo[0]==3)) NumIo[0]=3;
}

void sym_0120() {
    TouchScroll(true);
    CenteredText(i18n::TL("Main").c_str(), 1.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 10));
    SliderFloat("sym_0166", &sym_0056[2], 0.0f, 15.0f, "%0.2f", 5);
    SliderFloat("sym_0167", &sym_0056[1], 0.0f, 15.0f, "%0.2f", 5);
    SliderFloat("sym_0168", &sym_0056[3], 0.0f, 15.0f, "%0.2f", 5);
    SliderFloat("sym_0169", &sym_0056[4], 0.0f, 15.0f, "%0.2f", 5);
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0170").c_str());
    ImGui::Spacing();
    SliderFloat("sym_0171", &sym_0052, 5.0f, 180.0f, "%.0f秒");
    if (button(sym_0051 ? "sym_0173" : "sym_0174", 
    ImVec2(ImGui::GetContentRegionAvail().x, 60), 
    15.0f, 
    ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
    sym_0051 ? ImVec4(1.0f, 0.2f, 0.2f, 1.0f) : ImVec4(0.2f, 0.8f, 0.2f, 1.0f))) {
    sym_0051 = !sym_0051;
    if (sym_0051) {
    int randomNum = rand() % 99 + 1;
    string fileName = "/sdcard/sym_0175" + to_string(randomNum) + ".mp4";
    string command = "screenrecord --time-limit " + to_string(sym_0052) + " " + fileName + " &";
    system(command.c_str());
    timerStartTime = static_cast<int>(ImGui::GetTime());
    isTimerRunning = true;
       } 
    else {
    system("pkill -l SIGINT screenrecord");
    isTimerRunning = false;
        }
    }    
    ImGui::Spacing();
    if (sym_0051) {
    string timerText = i18n::TL("sym_0176") + ": " + to_string(static_cast<int>(ImGui::GetTime() - timerStartTime)) + i18n::TL("sym_0177");
    ImGui::TextColored(ImColor(1.0f, 0.2f, 0.2f, 1.0f), "%s", timerText.c_str());
    } 
    else {
    ImGui::TextColored(ImColor(0.5f, 0.5f, 0.5f, 1.0f), i18n::TL("sym_0178").c_str());
    }
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0151").c_str());
    ImGui::Spacing();
    if (button("sym_0179", ImVec2(-1, 70), 15.0f, 
              ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
              ImVec4(0.0f, 0.48f, 0.98f, 1.0f))) {
    GetBase();
    }   
    ImGui::Spacing();
    if (button("sym_0180", ImVec2(-1, 70), 15.0f, 
              ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
              ImVec4(0.0f, 0.48f, 0.98f, 1.0f))) {
        NumIoSave("sym_0079");
    }
    if (button("????-??:???R", ImVec2(-1, 70), 15.0f, 
              ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
              ImVec4(0.0f, 0.48f, 0.98f, 1.0f))) {
        NumIoSave("???????:???R");
    }
    if (button("Quit", ImVec2(-1, 80), 15.0f, 
              ImVec4(0.0f, 0.0f, 0.0f, 0.0f), 
              ImVec4(1.0f, 0.2f, 0.2f, 1.0f))) {
        exit(0);
    }
    ImGui::Spacing();
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("Target FPS").c_str());
    SliderFloat ("Target FPS", &NumIo[12], 30.0f, 240.0f, "%.0f");
    ImGui::Spacing();
    if (button(i18n::TL("ScreenShot").c_str(), ImVec2(-1, 70), 15.0f,
               ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
               ImVec4(0.0f, 0.48f, 0.98f, 1.0f))) {
        time_t now = time(nullptr);
        string fn = "/sdcard/screenshot_" + to_string((long long)now) + ".png";
        string cmd = "screencap -p " + fn + " &";
        system(cmd.c_str());
    }
    // Balance PushStyleVar(ImGuiStyleVar_ItemSpacing, ...)
    ImGui::PopStyleVar();
    TouchScroll(true);
    CenteredText(i18n::TL("sym_0048 sym_0049").c_str(), 1.5f);
    // Removed loot initialization toggle
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0181").c_str());
    Toggle("  sym_0082", sym_0001[1]);
    Toggle("  sym_0182", sym_0001[45]);
    Toggle("  sym_0183", DrawIo[910]);
    ImGui::Spacing();         
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0184").c_str());
    Toggle("  sym_0185", sym_0001[18]);
    Toggle("  sym_0186", sym_0001[3]);
    Toggle("  sym_0187", sym_0001[9]);
    ImGui::Spacing();   
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0188").c_str());
    Toggle("  sym_0189", sym_0001[5]);
    Toggle("  sym_0190", sym_0001[6]);
    Toggle("  sym_0191", sym_0001[7]);
    Toggle("  sym_0192", sym_0001[8]);
    Toggle("  sym_0193", sym_0001[11]);
    ImGui::Spacing();    
    ImGui::TextColored(ImColor(150, 150, 150, 255), i18n::TL("sym_0194").c_str());
    Toggle("  sym_0195", sym_0001[4]);
    // Removed BOSS toggle
    Toggle("  sym_0196", sym_0001[60]);
    ImGui::Spacing();
}

void sym_0050() {
    TouchScroll(true);
    CenteredText(i18n::TL("sym_0048 sym_0049").c_str(), 1.5f);
    // Removed loot initialization toggle
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0181").c_str());
    Toggle("  sym_0082", sym_0001[1]);
    Toggle("  sym_0182", sym_0001[45]);
    Toggle("  sym_0183", DrawIo[910]);
    ImGui::Spacing();
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0184").c_str());
    Toggle("  sym_0185", sym_0001[18]);
    Toggle("  sym_0186", sym_0001[3]);
    Toggle("  sym_0187", sym_0001[9]);
    ImGui::Spacing();
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0188").c_str());
    Toggle("  sym_0189", sym_0001[5]);
    Toggle("  sym_0190", sym_0001[6]);
    Toggle("  sym_0191", sym_0001[7]);
    Toggle("  sym_0192", sym_0001[8]);
    Toggle("  sym_0193", sym_0001[11]);
    ImGui::Spacing();
    ImGui::TextColored(ImColor(255, 255, 255, 255), i18n::TL("sym_0194").c_str());
    Toggle("  sym_0195", sym_0001[4]);
    // Removed BOSS toggle
    Toggle("  sym_0196", sym_0001[60]);
    ImGui::Spacing();
}

void sym_0102() {
        float pxx = screen_x/2;
        string play = to_string(fanhua.PlayerCount);
        string bot = to_string(fanhua.BotCount);
        errorBgWidth += (targetBgWidth - errorBgWidth) * bgAnimationSpeed;
        errorBgHeight += (targetBgHeight - errorBgHeight) * bgAnimationSpeed;
    
        if (fanhua.libUE4) {
        if (libUE4SuccessTime == 0) {
        libUE4SuccessTime = time(nullptr);
        targetBgWidth = 200.0f;
        targetBgHeight = 40.0f;
        }
        time_t currentTime = time(nullptr);
        float elapsed = difftime(currentTime, libUE4SuccessTime);
        if (elapsed < 5.0f) {
        string successMsg = i18n::TL("sym_0197");
        auto successSize = ImGui::CalcTextSize(successMsg.c_str());
        float padding = 10.0f;
        ImVec2 bgMin(pxx - errorBgWidth/2, 150 - errorBgHeight/2);
        ImVec2 bgMax(pxx + errorBgWidth/2, 150 + errorBgHeight/2);
        ImGui::GetForegroundDrawList()->AddRectFilled(bgMin, bgMax, ImColor(0, 200, 0, 200), 20.0f);
        ImVec2 successPos(pxx - successSize.x/2, 150 - successSize.y/2);
        ImGui::GetForegroundDrawList()->AddText(successPos, ImColor(255, 255, 255, 255), successMsg.c_str());
        }
        } else {
        libUE4SuccessTime = 0;
        string errorMsg = i18n::TL("sym_0198，sym_0199");
        auto errorSize = ImGui::CalcTextSize(errorMsg.c_str());
        float textPadding = 20.0f;
        targetBgWidth = errorSize.x + textPadding * 2;
        targetBgHeight = 40.0f;
        
        ImVec2 bgMin(pxx - errorBgWidth/2, 150 - errorBgHeight/2);
        ImVec2 bgMax(pxx + errorBgWidth/2, 150 + errorBgHeight/2);
        ImGui::GetForegroundDrawList()->AddRectFilled(bgMin, bgMax, ImColor(255, 0, 0, 200), errorBgHeight/2.0f);
        
        ImVec2 errorPos(pxx - errorSize.x/2, 150 - errorSize.y/2);
        ImGui::GetForegroundDrawList()->AddText(errorPos, ImColor(255, 255, 255), errorMsg.c_str());
        }
        if (!sym_0101) {
        const float dotSize = 13.0f;
        
        float ball_radius = dotSize;
        const float balls_y = 30; 
        const float horizontal_spacing = 32; 
        const int total_balls = 3;

        float start_x = screen_x/2 - (horizontal_spacing * (total_balls-1))/2;
        
        // Removed colored circles; floating button below toggles UI
        }
    
        // Disabled legacy overlay labels to keep main menu translated and clean
    
        ImVec2 childWindowSize = ImVec2(650, 250);
        ImVec2 childWindowPos = ImVec2(screen_x/2-(325), 0);
        ImGui::SetNextWindowPos(childWindowPos);
        ImGui::SetNextWindowSize(childWindowSize);
        ImVec4 transparentColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, transparentColor);
        ImGui::PushStyleColor(ImGuiCol_Border, transparentColor);
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, transparentColor);
        // Round floating toggle button (ZK)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.12f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.18f, 0.18f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.10f, 0.10f, 0.95f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 9999.0f);
        ImGui::Begin("FloatingToggle", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        // Place near bottom-right
        ImGui::SetWindowPos(ImVec2(screen_x - 90.0f, screen_y - 90.0f), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(64, 64), ImGuiCond_Always);
        ImGui::SetCursorPos(ImVec2((64 - 56) * 0.5f, (64 - 56) * 0.5f));
        if (ImGui::Button("ZK", ImVec2(56, 56))) { window = !window; }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(6);



