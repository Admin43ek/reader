
// ===============================
//  Визуализация через Vulkan + ImGui
//
//  Назначение:
//   - Инициализация Vulkan графики и ImGui контекста
//   - Единый цикл Begin/End кадра
//   - Загрузка композитного шрифта (латиница/кириллица/китайский)
//
//  Примечание:
//   - Глобальный FPS контролируется таймером в main.cpp (RenderingFPS)
//   - Здесь нет “sleep” — только отрисовка
// ===============================
#include <draw.h>
#include <array>
#include <cmath>
#include "font2.h"
#include "font.h"
// Cyrillic-capable font bytes embedded via header at workspace root
// The embedded font header lives at the repository root.
// Use a relative path that resolves inside the project tree instead of
// accidentally escaping to the workspace parent directory.
#include "../../../bankgothicmdbt_medium_otf.h"
VulkanGraphics VK;
ANativeWindow *native_window;
Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo;

string exec(string command) {
char buffer[128];
string result = "";
FILE* pipe = popen(command.c_str(), "r");
if (!pipe) {
return "popen failed!";
}
while (!feof(pipe)) {
if (fgets(buffer, 128, pipe) != nullptr)
result += buffer;
}
pclose(pipe);
return result;
}


int init_vulkan(int _screen_x, int _screen_y, bool flp) {
native_window = android::ANativeWindowCreator::Create("Ssage", _screen_x, _screen_y, flp);;
ANativeWindow_acquire(native_window);
VK.Create(native_window, _screen_x, _screen_y);
return 1;
}


void screen_config() {
displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
screen_x = displayInfo.width;
screen_y = displayInfo.height;
Orientation = displayInfo.orientation;
}



void ImGui_init() {
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();
io.IniFilename = NULL;
 
ImGui_ImplAndroid_Init(native_window);
VK.Setup();  //初始化vk数据
// Load legacy fonts
io.Fonts->AddFontFromMemoryTTF((void *) font_data, font_size, 35.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
io.Fonts->AddFontFromMemoryTTF((void *)font2_data, font2_size, 35.0f, NULL, NULL);
// Build a composite default font covering Latin + Cyrillic + Chinese
ImFont* defaultFont = nullptr;
{
    // Base: Bank Gothic (adds Latin + Cyrillic)
    ImFontConfig base_cfg; base_cfg.OversampleH = 2; base_cfg.OversampleV = 2; base_cfg.PixelSnapH = true;
    defaultFont = io.Fonts->AddFontFromMemoryTTF((void*)bankgothicmdbt_medium_otf, (int)sizeof(bankgothicmdbt_medium_otf), 35.0f, &base_cfg, io.Fonts->GetGlyphRangesCyrillic());
    // Merge: Chinese glyphs into the same font
    ImFontConfig merge_cfg; merge_cfg.MergeMode = true; merge_cfg.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryTTF((void*)font_data, font_size, 35.0f, &merge_cfg, io.Fonts->GetGlyphRangesChineseFull());
    // Use it as default so RU doesn't render as ???
    if (defaultFont) io.FontDefault = defaultFont;
}
// Set overall UI font scale
io.FontGlobalScale = 1.15f;
}


void VK_Begin() {
screen_config();
ImGui_ImplVulkan_NewFrame(); //开始新的vk帧
ImGui_ImplAndroid_NewFrame(screen_x, screen_y);
ImGui::NewFrame();
}

void VK_End() {
ImGui::Render();
VK.Render(ImGui::GetDrawData());  //使用vk进行渲染
}


void shutdown(){
ImGui_ImplVulkan_Shutdown();
ImGui_ImplAndroid_Shutdown(); 
ImGui::DestroyContext();
VK.PrepareShutdown();
VK.Cleanup();
ANativeWindow_release(native_window);
android::ANativeWindowCreator::Destroy(native_window);
}

void OffScreen(Vec2 Obj, float camear, ImColor color, float Radius) {
    ImRect screen_rect = {0.0f, 0.0f, screen_x, screen_y};
    const ImVec2 screen_center = screen_rect.GetCenter();
    float angle = atan2f(screen_center.y - Obj.y, screen_center.x - Obj.x);
    angle += camear > 0 ? static_cast<float>(M_PI) : 0.0f;

    const float cosAngle = cosf(angle);
    const float sinAngle = sinf(angle);
    const float radiusScaledCos = Radius * cosAngle;
    const float radiusScaledSin = Radius * sinAngle;

    Vec2 arrow_center {
        screen_center.x + radiusScaledCos,
        screen_center.y + radiusScaledSin
    };

    constexpr float kArrowScale = 1.155f;
    std::array<ImVec2, 4> points {
        ImVec2(-22.0f, -8.6f),
        ImVec2(0.0f, 0.0f),
        ImVec2(-22.0f, 8.6f),
        ImVec2(-18.0f, 0.0f)
    };

    for (ImVec2 &point : points) {
        const float scaledX = point.x * kArrowScale;
        const float scaledY = point.y * kArrowScale;
        point.x = arrow_center.x + scaledX * cosAngle - scaledY * sinAngle;
        point.y = arrow_center.y + scaledX * sinAngle + scaledY * cosAngle;
    }

    float alpha = 1.0f;
    if (camear > 0) {
        constexpr float nearThreshold = 200.0f * 200.0f;
        const float diffX = (Obj.x < 0.0f)
                                ? -Obj.x
                                : (Obj.x > screen_rect.Max.x ? Obj.x - screen_rect.Max.x : 0.0f);
        const float diffY = (Obj.y < 0.0f)
                                ? -Obj.y
                                : (Obj.y > screen_rect.Max.y ? Obj.y - screen_rect.Max.y : 0.0f);
        const float distanceSquared = diffX * diffX + diffY * diffY;
        alpha = (camear < 0) ? 1.0f : distanceSquared / nearThreshold;
        if (alpha > 1.0f) {
            alpha = 1.0f;
        } else if (alpha < 0.0f) {
            alpha = 0.0f;
        }
    }

    ImColor arrowColor = color;
    arrowColor.Value.w = alpha;
    ImDrawList *drawList = ImGui::GetBackgroundDrawList();
    drawList->AddTriangleFilled(points[0], points[1], points[3], arrowColor);
    drawList->AddTriangleFilled(points[2], points[1], points[3], arrowColor);
    drawList->AddQuad(points[0], points[1], points[2], points[3], ImColor(0.0f, 0.0f, 0.0f, alpha), 1.335f);
}
