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
    
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.86f, 0.86f, 0.86f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.86f, 0.86f, 0.86f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.86f, 0.86f, 0.86f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  ImVec4(0.86f, 0.86f, 0.86f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0.86f, 0.86f, 0.86f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));
    
    ImGui::BeginChild(names[0].c_str(), 窗口, true);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0));
    
    if (ImGui::Button(i18n::TL(names[0]).c_str(), ImVec2(按钮.x, 按钮.y))) {
    name.Tab = 0;
    参数 = 0;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    min = ImVec2(ImGui::GetItemRectMin().x + 差距.x, ImGui::GetItemRectMin().y + 差距.y); 
    max = ImVec2(ImGui::GetItemRectMax().x - 差距.x, ImGui::GetItemRectMax().y - 差距.y); 
    Lerp(name.beg, name.end, 0.06f);
    
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(min.x + name.beg, min.y),
        ImVec2(max.x + name.beg, max.y),
        ImColor(255, 255, 255, 255),
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

        ImGui::GetWindowDrawList()->AddText(NULL, 35, textPos, ImColor(0, 0, 0, 255), bynh.c_str());
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
