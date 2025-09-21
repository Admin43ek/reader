#ifndef SHOU_H
#define SHOU_H
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <thread>
#include <random>
#include <linux/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fstream>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unordered_map>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <thread>
#include <array>
#include <android/native_window_jni.h>
#include "imgui.h"
#include "imgui_image.h"
#include "imgui_internal.h"
#include "纹理.h"
#include "tupian.h"

ImTextureID FloatBallwd;//悬浮球
ImVec2 ImagePos = {0, 0};
ImGuiWindow *Window = nullptr;

void 帅哥(){
	FloatBallwd = ImAgeHeadFile(png, sizeof(png));
}

//图标
void DrawLogo(ImTextureID ID,ImVec2 center, float size)
{
	ImGui::SetCursorPos({180, 180});
	ImDrawList *draw_list = ImGui::GetWindowDrawList();
	draw_list->AddImage(ID,{center.x - size / 1, center.y - size / 1},{center.x + size / 1, center.y + size / 1});
}




void 温迪(){
		ImGuiStyle * style = &ImGui::GetStyle();
       style->Colors[ImGuiCol_Text] = ImColor(0, 0, 0, 255);//文本颜色
    style->Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255, 255);//窗口背景颜色
    style->Colors[ImGuiCol_PopupBg] = ImVec4(118, 238, 198,255);//弹出窗口背景颜色
    style->Colors[ImGuiCol_Border] = ImColor(127, 255, 212,255); // 整体边框颜色
    //style->Colors[ImGuiCol_BorderShadow] = ImColor(118, 238, 198,255);//边框阴影颜色(内部控件边框颜色)
    style->Colors[ImGuiCol_FrameBg] = ImColor(255, 255, 255, 255);//框架背景颜色(按钮没动的颜色)
     style->Colors[ImGuiCol_FrameBgActive] = ImColor(255, 255, 255, 255);//激活的框架背景颜色
     style->Colors[ImGuiCol_FrameBgHovered] = ImColor(255, 255, 255, 255);//鼠标悬停的框架背景颜色
     style->Colors[ImGuiCol_ScrollbarBg] = ImColor(255, 255, 255, 255);//滚动区域背景颜色
     style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(118, 238, 198,255);//滚动条抓取颜色
     style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(118, 238, 198,255);//鼠标悬停时的滚动条抓取颜色
     style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(118, 238, 198,255);//激活时的滚动条抓取颜色
     style->Colors[ImGuiCol_CheckMark] = ImColor(118, 238, 198,255);//选中标记颜色
     style->Colors[ImGuiCol_SliderGrab] = ImColor(16, 255, 225,255);//滑块抓取颜色
     style->Colors[ImGuiCol_SliderGrabActive] = ImColor(118, 238, 198,255);//激活时的滑块抓取颜色
     style->Colors[ImGuiCol_Separator] = ImColor(118, 238, 198,255);//分隔线颜色
     style->Colors[ImGuiCol_SeparatorActive] = ImColor(118, 238, 198,255);//激活时的分隔线颜色//深灰色
     style->Colors[ImGuiCol_SeparatorHovered] = ImColor(118, 238, 198,255);//鼠标悬停时的分隔线颜色
     style->Colors[ImGuiCol_Button] = ImColor(118, 238, 198,255);//正常按钮颜色//酷安绿
     style->Colors[ImGuiCol_ButtonActive] = ImColor(118, 238, 198,255);//激活的按钮颜色//微信绿
     style->Colors[ImGuiCol_ButtonHovered] = ImColor(118, 238, 198,255);//鼠标悬停时的按钮颜色
     style->Colors[ImGuiCol_HeaderActive] = ImColor(118, 238, 198,255);//激活的标题颜色
     style->Colors[ImGuiCol_HeaderHovered] = ImColor(118, 238, 198,255);//鼠标悬停时的标题颜色
	 style->Colors[ImGuiCol_TextSelectedBg] = ImColor(87, 59, 128,255);//选中文本背景颜色
	 style->Colors[ImGuiCol_Header] = ImColor(118, 238, 198,255);//设置下展标题颜色
}

#endif
