#include <stb_image.h>
#include "内存图片/头文件.h"

ImFont* font = nullptr;

void 绘制字体描边(float size,int x, int y, ImVec4 color, const char* str) {
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x + 1.0, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y + 1.0), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y - 1.0), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size,ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
}
void 绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char* str){
ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x-0.8, y-0.8), color1, str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x+0.8, y+0.8), color1, str);
ImGui::GetBackgroundDrawList()->AddText(NULL, size, ImVec2(x, y), color, str);
}

void Box4Line(ImDrawList *draw, float thicc, int x, int y, int w, int h, int color) {
int iw = w / 4;
int ih = h / 4;
// top
draw->AddRect(ImVec2(x, y),ImVec2(x + iw, y), color, thicc);
draw->AddRect(ImVec2(x + w - iw, y),ImVec2(x + w, y), color, thicc);
draw->AddRect(ImVec2(x, y),ImVec2(x, y + ih), color, thicc);
draw->AddRect(ImVec2(x + w - 1, y),ImVec2(x + w - 1, y + ih), color, thicc);;
// bottom
draw->AddRect(ImVec2(x, y + h),ImVec2(x + iw, y + h), color, thicc);
draw->AddRect(ImVec2(x + w - iw, y + h),ImVec2(x + w, y + h), color, thicc);
draw->AddRect(ImVec2(x, y + h - ih), ImVec2(x, y + h), color, thicc);
draw->AddRect(ImVec2(x + w - 1, y + h - ih), ImVec2(x + w - 1, y + h), color, thicc);
}

// 杂项
ImColor 浅蓝 = ImColor(ImVec4(36/255.f, 249/255.f, 217/255.f, 255/255.f));
ImColor 蓝色 = ImColor(ImVec4(170/255.f, 203/255.f, 244/255.f, 0.95f));
ImColor 白色 = ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 0.95f));
ImColor 浅粉 = ImColor(ImVec4(255/255.f, 200/255.f, 250/255.f, 0.95f));
ImColor 黑色 = ImColor(ImVec4(0/255.f, 0/255.f, 0/255.f, 0.7f));
ImColor 半黑 = ImColor(ImVec4(0/255.f, 0/255.f, 0/255.f, 0.18f));
ImColor 血色 = ImColor(ImVec4(0/255.f, 249/255.f, 0/255.f, 0.35f));
ImColor 红色 = ImColor(ImVec4(233/255.f, 55/255.f, 51/255.f, 0.95f));
ImColor 绿色 = ImColor(ImVec4(50/255.f, 222/215.f, 50/255.f, 0.95f));
ImColor 黄色 = ImColor(ImVec4(255/255.f, 255/255.f, 0/255.f, 0.95f));
ImColor 橘黄 = ImColor(ImVec4(255/255.f, 150/255.f, 30/255.f, 0.95f));
ImColor 粉红 = ImColor(ImVec4(220/255.f, 108/255.f, 1202/255.f, 0.95f));
ImColor 紫色 = ImColor(ImVec4(169/255.f, 120/255.f, 223/255.f, 0.95f));
ImColor 空白 = ImColor(ImVec4(1.0/255.f, 1.0/255.f, 1.0/255.f, 0.0f));

void DrawStrokeText(int x, int y, ImVec4 color, const char *str) {
ImGui::GetForegroundDrawList()->AddText(ImVec2((float) x + 1, (float) y),
ImGui::ColorConvertFloat4ToU32(
ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetForegroundDrawList()->AddText(ImVec2((float) x - 0.1f, (float) y),
ImGui::ColorConvertFloat4ToU32(
ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetForegroundDrawList()->AddText(ImVec2((float) x, (float) y + 1),
ImGui::ColorConvertFloat4ToU32(
ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetForegroundDrawList()->AddText(ImVec2((float) x, (float) y - 1),
ImGui::ColorConvertFloat4ToU32(
ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
ImGui::GetForegroundDrawList()->AddText(ImVec2((float) x, (float) y),
ImGui::ColorConvertFloat4ToU32(color),str);
}

char *获取枪械信息(int BoxId) {
switch (BoxId) {
//突击步枪
case 101008:
return "M762";
break;
case 101001:
return "AKM";
break;
case 101004:
return "M416";
break;
case 101003:
return "SCAR-L";
break;
case 101002:
return "M16A4";
break;
case 101009:
return "Mk47";
break;
case 101006:
return "AUG";
break;
case 101005:
return "Groza";
break;
case 101010:
return "G36C";
break;
case 101007:
return "QBZ";
break;
case 101011:
return "AC-VAL";
break;
case 101012:
return "蜜獾突击步枪";
break;
//连狙
case 103009:
return "SLR";
break;
case 103005:
return "VSS";
break;
case 103006:
return "Mini14";
break;
case 103010:
return "QBU";
break;
case 103004:
return "SKS";
break;
case 103007:
return "MK14";
break;
case 103014:
return "MK20-H";
break;
case 103013:
return "M417";
break;
//连狙
case 103012:
return "AMR";
break;
case 103003:
return "AWM";
break;
case 103002:
return "M24";
break;
case 103011:
return "莫甘娜辛";
break;
case 103001:
return "Kar98K";
break;
case 103008:
return "Win94";
break;
//机关枪
case 105001:
return "M249";
break;
case 105002:
return "DP-28";
case 105010:
return "MG3";
break;
case 107001:
return "十字弩";
break;
case 107007:
return "爆炸猎弓";
break;
//冲锋枪
case 102001:
return "UZI";
break;
case 102003:
return "Vector";
break;
case 100103:
return "PP-19";
break;
case 102007:
return "MP5K";
break;
case 102002:
return "UMP-45";
break;
case 102004:
return "汤姆逊";
break;
case 102105:
return "P90";
break;
case 102005:
return "野牛";
break;
//霰弹枪
case 104001:
return "S686";
break;
case 104002:
return "S1897";
break;
case 104003:
return "S12K";
break;
case 104004:
return "DBS";
break;
case 104100:
return "SPAS-12";
break;
//投掷爆炸物
case 602004:
return "手榴弹";
break;
case 602003:
return "燃烧瓶";
break;
case 602002:
return "烟雾弹";
break;
case 602001:
return "震撼弹";
break;
//近战武器
case 108003:
return "镰刀";
break;
case 108002:
return "撬棍";
break;
case 108001:
return "大砍刀";
break;
case 108004:
return "平底锅";
break;
case 0:
return "拳头";
break;
default:
return "未收录";
break;
}
return nullptr;
}  
