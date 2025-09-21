#include <draw.h>
#include "include.h"
#include "内存图片/悬浮窗.h"
#include "内存图片/内存图片工具.h"
#include "内存图片/自瞄按钮1.h"
#include "内存图片/自瞄按钮2.h"
#include "封装绘图.h"
#include "结构体.h"
#include "morishima_fonts.hpp"
#include "../../include/Tools/offsets.h"
#include <fstream>
#include <sstream>
void *handle;// 动态库方案
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
ANativeWindow *native_window;
ANativeWindow *(*createNativeWindow)(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author);
EGLContext context = EGL_NO_CONTEXT;

int FPS = 100000;
timer FPS限制;
float FPF显示 = 0;
Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;

string exec(string command) {
char buffer[128];
string result = "";
// Open pipe to file
FILE* pipe = popen(command.c_str(), "r");
if (!pipe) {
return "popen failed!";
}
// read till end of process:
while (!feof(pipe)) {
// use buffer to read and add to result
if (fgets(buffer, 128, pipe) != nullptr){
result += buffer;
}
}
pclose(pipe);
return result;
}




void ReadResolutionFromFile(const std::string& filePath, int& width, int& height) {
    std::ifstream inputFile(filePath);
    if (inputFile.is_open()) {
        std::string line;
        if (std::getline(inputFile, line)) {
            std::stringstream ss(line);
            std::string token;
            if (std::getline(ss, token, ',')) {
                width = std::stoi(token);
                if (std::getline(ss, token, ',')) {
                    height = std::stoi(token);
                }
            }
        }
        inputFile.close();
    } else {
        printf("读取分辨率文件失败,请确保/data/目录下的分辨率文件格式为 EACF.txt");
        exit(0);
    }
}













int init_egl(int _screen_x, int _screen_y, bool log){
void* sy = get_createNativeWindow(); // 适配9-13安卓版本
createNativeWindow = (ANativeWindow *(*)(const char *, uint32_t, uint32_t, bool))(sy);
native_window = createNativeWindow("Ssage", _screen_x, _screen_y, false);
ANativeWindow_acquire(native_window);
display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
if (display == EGL_NO_DISPLAY) {
printf("eglGetDisplay error=%u\n", glGetError());
return -1;
}
if(log){
printf("eglGetDisplay ok\n");
}
if (eglInitialize(display, 0, 0) != EGL_TRUE) {
printf("eglInitialize error=%u\n", glGetError());
return -1;
}
if(log){
printf("eglInitialize ok\n");
}
EGLint num_config = 0;
const EGLint attribList[] = {
EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
EGL_BLUE_SIZE, 5,//-->delete
EGL_GREEN_SIZE, 6,//-->delete
EGL_RED_SIZE, 5,//-->delete
EGL_BUFFER_SIZE, 32,//-->new field
EGL_DEPTH_SIZE, 16,
EGL_STENCIL_SIZE, 8,
EGL_NONE
};
if (eglChooseConfig(display, attribList, nullptr, 0, &num_config) != EGL_TRUE) {
printf("eglChooseConfigerror=%u\n", glGetError());
return -1;
}
if(log){
printf("num_config=%d\n", num_config);
}
if (!eglChooseConfig(display, attribList, &config, 1, &num_config)) {
printf("eglChooseConfigerror=%u\n", glGetError());
return -1;
}
if(log){
printf("eglChooseConfig ok\n");
}
EGLint egl_format;
eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
if (context == EGL_NO_CONTEXT) {
printf("eglCreateContexterror = %u\n", glGetError());
return -1;
}
if(log){
printf("eglCreateContext ok\n");
}
surface = eglCreateWindowSurface(display, config, native_window, nullptr);
if (surface == EGL_NO_SURFACE) {
printf("eglCreateWindowSurfaceerror = %u\n", glGetError());
return -1;
}
if(log){
printf("eglCreateWindowSurface ok\n");
}
if (!eglMakeCurrent(display, surface, surface, context)) {
printf("eglMakeCurrenterror = %u\n", glGetError());
return -1;
}
if(log){
printf("eglMakeCurrent ok\n");
}
return 1;
}

void screen_config(){
std::string window_size = exec("wm size");
sscanf(window_size.c_str(),"Physical size: %dx%d",&screen_x, &screen_y);
full_screen.ScreenX = screen_x;
full_screen.ScreenY = screen_y;
std::thread *orithread = new std::thread([&] {
while(true){
Orientation = atoi(exec("dumpsys input | grep SurfaceOrientation | awk '{print $2}' | head -n 1").c_str());
if(Orientation == 0 || Orientation == 2){
screen_x = full_screen.ScreenX;
screen_y = full_screen.ScreenY;
}
if(Orientation == 1 || Orientation == 3){
screen_x = full_screen.ScreenY;
screen_y = full_screen.ScreenX;
}
std::this_thread::sleep_for(0.5s);
}
});
orithread->detach();
}

void NumIoSave(const char *name){
if (numSave == nullptr) {
string SaveFile = "/data";
SaveFile += "/";
SaveFile += name;
numSave = fopen(SaveFile.c_str(), "wb+");
}
fseek(numSave, 0, SEEK_SET);
fwrite(Ios, sizeof(float) * 50, 1, numSave);

fflush(numSave);
fsync(fileno(numSave));
}

void NumIoLoad(const char *name){
if (numSave == nullptr) {
string SaveFile = "/data";
SaveFile += "/";
SaveFile += name;
numSave = fopen(SaveFile.c_str(), "rb+");
}
if (numSave != nullptr) {
fseek(numSave, 0, SEEK_SET);
fread(Ios, sizeof(float) * 50, 1, numSave);
} else {
Config.自动瞄准.自瞄控件 = true;
Config.预警绘制.雷达X轴 = 300.0f;
Config.预警绘制.雷达Y轴 = 400.0f;
Config.自动瞄准.自瞄范围 = 175.0f;
Config.自动瞄准.触摸位置Y = 1500.0f;
Config.自动瞄准.触摸位置X = 650.0f;
Config.自动瞄准.触摸范围 = 135.0f;
Config.自动瞄准.锁定强度 = 10.5f; 
Config.自动瞄准.瞄准速度 = 18.2f;
Config.自动瞄准.子弹速度 = 500.0f;
Config.自动瞄准.压枪参数 = 1.6f;
Config.自动瞄准.预判参数 = 2.15f;
Config.自动瞄准.子弹下坠预判 = 0.0f;
}
}

void ImGuiMenustyle() {
Config.自动瞄准.自瞄控件 = true;
Config.预警绘制.雷达X轴 = 300.0f;
Config.预警绘制.雷达Y轴 = 400.0f;
Config.自动瞄准.自瞄范围 = 175.0f;
Config.自动瞄准.触摸位置Y = 1500.0f;
Config.自动瞄准.触摸位置X = 650.0f;
Config.自动瞄准.触摸范围 = 135.0f;
Config.自动瞄准.锁定强度 = 10.5f; 
Config.自动瞄准.瞄准速度 = 18.2f;
Config.自动瞄准.子弹速度 = 500.0f;
Config.自动瞄准.压枪参数 = 1.6f;
Config.自动瞄准.预判参数 = 2.15f;
Config.自动瞄准.子弹下坠预判 = 0.0f;
FPS = 100000;
}


void ImGui_init(){
if (g_Initialized){
return;
}
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();
io.IniFilename = NULL;
ImGui::StyleColorsDark();
onButton = ImAgeHeadFile(FunnelIcon5, sizeof(FunnelIcon5));
offButton = ImAgeHeadFile(FunnelIcon4, sizeof(FunnelIcon4));
FloatBall = ImAgeHeadFile(icon, sizeof(icon));
ImGui_ImplAndroid_Init(native_window);
ImGui_ImplOpenGL3_Init("#version 300 es");
io.Fonts->AddFontFromMemoryTTF((void*)Morishima_data, Morishima_size, 38.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
ImGui::GetStyle().ScaleAllSizes(3.0f);
ImGuiMenustyle();
ColorInitialization();
g_Initialized = true;
颜色初始化();
}

struct AimStruct {
Vector3A ObjAim;
Vector3A MyObjAim;
Vector3A AimMovement;
float ScreenDistance = 0;
float WodDistance = 0; 
char Name[32];
}Aim[100];

bool IsAimLongAim = false;
char AimName[32];

// 遍历自瞄对象
int findminat() {
float min = Config.自动瞄准.自瞄范围;
float DistanceMin = 500;
int minAt = 999;
for (int i = 0; i < MaxPlayerCount; i++) {
switch ((int)Config.自动瞄准.优先方式) {
case 0:
if (IsAimLongAim) {
if (strcmp(Aim[i].Name, AimName) == 0) {   
minAt = i;
}
} else {
if (Aim[i].ScreenDistance < min) {
if (Config.自动瞄准.持续锁定) {
strcpy(AimName, Aim[i].Name);
}
min = Aim[i].ScreenDistance;
minAt = i;
}
}
break;
case 1:
if (IsAimLongAim) {
if (strcmp(Aim[i].Name, AimName) == 0) {   
minAt = i;
}
} else {
if (Aim[i].WodDistance < DistanceMin) {
if (Config.自动瞄准.持续锁定) {
strcpy(AimName, Aim[i].Name);
}
DistanceMin = Aim[i].WodDistance;
minAt = i;
}
}
break;
}
}
if (minAt == 999) {
Gmin = -1;
IsAimLongAim = false;
return -1;
}
Gmin = minAt;   
Aim[minAt].WodDistance;
if (Config.自动瞄准.持续锁定) {
IsAimLongAim = true;
}
return minAt;
}
float GetPitch(float Fov)  {
if (Fov > 75 && Fov <= 130) { // 不开镜
return (0.7f/30)*Config.自动瞄准.压枪参数;
}else if (Fov == 70 || Fov == 75) { // 机瞄 ，侧瞄
return (1.8f/30)*Config.自动瞄准.压枪参数;
}else if (Fov == 55 || Fov == 60) { // 红点 ，全息
return (1.8f/30)*Config.自动瞄准.压枪参数;
}else if ((int)Fov == 44) {// 2倍
return (5.3f/30)*Config.自动瞄准.压枪参数;
}else if ((int)Fov == 26) {// 3倍
return (7.2f/30)*Config.自动瞄准.压枪参数;
}else if ((int)Fov == 20) {// 4倍
return (8.6f/30)*Config.自动瞄准.压枪参数;
}else if ((int)Fov == 13) {// 6倍
return (14.1f/30)*Config.自动瞄准.压枪参数;
}
return (2.5f/30)*Config.自动瞄准.压枪参数;// 8倍
}

void AimBotAuto(){
bool isDown = false;
// 是否按下触摸
double leenx = 0.0f;
// x轴速度
double leeny = 0.0f;
// y轴速度 
double de = 1.5f;
// 顶部不可触摸区域

double tx = 0, ty = 0;
// 触摸点位置

float SpeedMin = 2.0f;
// 临时触摸速度

double w = 0.0f, h = 0.0f, cmp = 0.0f;
// 宽度 高度 正切

double ScreenX = screen_x, ScreenY = screen_y;
// 分辨率(竖屏)PS:滑屏用的坐标是竖屏状态下的

double ScrXH = ScreenX / 2.0f;
// 一半屏幕X

double ScrYH = ScreenY / 2.0f;
// 一半屏幕X

static float TargetX = 0;
static float TargetY = 0;
// 触摸目标位置

float PushX;
/* 压枪X */
float PushY;
/* 压枪Y */

Vector3A obj;

float NowCoor[3];

Touch_Init(&screen_x,&screen_y);

while (1) {
usleep(1000000 / 120);
ImGuiIO& iooi = ImGui::GetIO();
if (Config.自动瞄准.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= Config.自动瞄准.触摸位置Y + Config.自动瞄准.触摸范围 && iooi.MousePos.y <= screen_y - Config.自动瞄准.触摸位置X + Config.自动瞄准.触摸范围 && iooi.MousePos.x >= Config.自动瞄准.触摸位置Y - Config.自动瞄准.触摸范围 && iooi.MousePos.y >= screen_y - Config.自动瞄准.触摸位置X - Config.自动瞄准.触摸范围) {
usleep(30000);  
if (Config.自动瞄准.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= Config.自动瞄准.触摸位置Y + Config.自动瞄准.触摸范围 && iooi.MousePos.y <= screen_y - Config.自动瞄准.触摸位置X + Config.自动瞄准.触摸范围 && iooi.MousePos.x >= Config.自动瞄准.触摸位置Y - Config.自动瞄准.触摸范围 && iooi.MousePos.y >= screen_y - Config.自动瞄准.触摸位置X - Config.自动瞄准.触摸范围) {
while (Config.自动瞄准.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= Config.自动瞄准.触摸位置Y + Config.自动瞄准.触摸范围 && iooi.MousePos.y <= screen_y - Config.自动瞄准.触摸位置X + Config.自动瞄准.触摸范围 && iooi.MousePos.x >= Config.自动瞄准.触摸位置Y - Config.自动瞄准.触摸范围 && iooi.MousePos.y >= screen_y - Config.自动瞄准.触摸位置X - Config.自动瞄准.触摸范围) {
Config.自动瞄准.触摸位置Y = iooi.MousePos.x;
Config.自动瞄准.触摸位置X = screen_y - iooi.MousePos.y;
TouchingColor = ImColor(0, 220, 0, 150);
usleep(500);   
}
TouchingColor = ImColor(255, 0, 0, 150);
}
}

if (!Config.自动瞄准.自瞄开关) {   
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量 
Touch_Up(6);
// 抬起
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}

if (!Config.自动瞄准.自瞄控件) {   
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量 
Touch_Up(6);
// 抬起
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}

findminat();
// 获取目标

if (Gmin == -1) {  
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量 
Touch_Up(6);
// 抬起
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}

float ToReticleDistance = Aim[Gmin].ScreenDistance;
float FlyTime = WorldDistance / Config.自动瞄准.子弹速度;
//float DropM = 540.0f * FlyTime * FlyTime;
float DropM = 540.0f * FlyTime * Config.自动瞄准.子弹下坠预判;

float Fov = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(自身结构体+ 0x90) + 0x404) + 0x4A4);

float 压枪 = GetPitch(Fov);

int 开火判断 = driver->read<int>(自身结构体 + 0x15a8);
int 开镜判断 = driver->read<int>(自身结构体 + 0xfe1);

NowCoor[0] = Aim[Gmin].ObjAim.X;
NowCoor[1] = Aim[Gmin].ObjAim.Y;
NowCoor[2] = Aim[Gmin].ObjAim.Z;
obj.X = NowCoor[0] + (Aim[Gmin].AimMovement.X * FlyTime);
obj.Y = NowCoor[1] + (Aim[Gmin].AimMovement.Y * FlyTime);
obj.Z = NowCoor[2] + (Aim[Gmin].AimMovement.Z * FlyTime) + DropM;

if(开火判断 == 1 || 开镜判断 == 1){
obj.Z -= Aim[Gmin].WodDistance * Config.自动瞄准.压枪参数 * GetWeaponId(MyWeapon);
}


if(开火判断 == 1 || 开镜判断 == 1){
if(AimHead.X < screen_x/2){//左
obj.X -= Aim[Gmin].WodDistance *Config.自动瞄准.预判参数;
} else if (AimHead.X > screen_x/2) {//右
obj.X += Aim[Gmin].WodDistance *Config.自动瞄准.预判参数;
}
}

float cameras = matrix[3] * obj.X + matrix[7] * obj.Y + matrix[11] * obj.Z + matrix[15];

Vector2A vpvp = WorldToScreen(obj,matrix,cameras); 

float AimDs = sqrt(pow(px - vpvp.X, 2) + pow(py - vpvp.Y, 2));
zm_y = vpvp.X; 
zm_x = ScreenX - vpvp.Y;

if(Config.自动瞄准.动态范围){
if(开镜判断 == 1 || 开火判断 == 1) {
AimbotFov = AimDs;
}else{
AimbotFov = Config.自动瞄准.自瞄范围;
}
}

if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY) {  
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量 
Touch_Up(6);
// 抬起
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}

//if (ToReticleDistance <= Config.自动瞄准.自瞄范围 || AimDs <= Config.自动瞄准.自瞄范围) {
if (ToReticleDistance <= Config.自动瞄准.自瞄范围) {

if (Config.自动瞄准.触发方式 == 自瞄触发方式::开火) {
if (开火判断 != 1) {
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量
Touch_Up(6);
isDown = false;
}  
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}
} else if (Config.自动瞄准.触发方式 == 自瞄触发方式::开镜) {
if (开镜判断 != 1) {
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量
Touch_Up(6);
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}
} else if (Config.自动瞄准.触发方式 == 自瞄触发方式::开火开镜) {
if (开镜判断 != 1) {
if (开火判断 != 1) {
IsAimLongAim = false;
if (isDown == true) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量
Touch_Up(6);
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}
}
}

float Acc = getScopeAcc((int)(90 / Fov));

if (isDown == false) {
usleep(1000);
if (Config.自动瞄准.屏幕位置 == 0)
Touch_Down(6, (int)tx, (int)ty);
else
Touch_Down(6, screen_y - (int)tx, screen_x-(int)ty);
isDown = true;
usleep(1000);
}

if (zm_x > ScrXH) {
TargetX = -(ScrXH - zm_x) / Config.自动瞄准.瞄准速度 * Acc;
if (TargetX + ScrXH > ScrXH * 2) {
TargetX = 0;
}
} else if (zm_x < ScrXH) {
TargetX = (zm_x - ScrXH) / Config.自动瞄准.瞄准速度 * Acc; 
if (TargetX + ScrXH < 0) {
TargetX = 0;
}
}
if (zm_y > ScrYH) {
TargetY = -(ScrYH - zm_y) / Config.自动瞄准.瞄准速度 * Acc;  
if (TargetY + ScrYH > ScrYH * 2) {
TargetY = 0;
}
} else if (zm_y < ScrYH) {
TargetY = (zm_y - ScrYH) / Config.自动瞄准.瞄准速度 * Acc;  
if (TargetY + ScrYH < 0) {
TargetY = 0;
}
}

if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35) {
if (isDown) {
usleep(1000);
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量
Touch_Up(6);
isDown = false;
}
usleep(Config.自动瞄准.锁定强度 * 1000);
continue;
}   

tx += TargetX;
ty += TargetY;

if (tx >= Config.自动瞄准.触摸位置X + Config.自动瞄准.触摸范围 || tx <= Config.自动瞄准.触摸位置X - Config.自动瞄准.触摸范围 || ty >= Config.自动瞄准.触摸位置Y + Config.自动瞄准.触摸范围 || ty <= Config.自动瞄准.触摸位置Y - Config.自动瞄准.触摸范围) {
usleep(1000);
// 只要滑屏达到了边界，直接还原至中心
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量
Touch_Up(6);
// 抬起  
usleep(3000);  
// 延迟
if (!Config.自动瞄准.屏幕位置)
Touch_Down(6, (int)tx, (int)ty);
else
Touch_Down(6, screen_y - (int)tx, screen_x - (int)ty);
// 按下   
isDown = true;
tx += TargetX;
ty += TargetY;
usleep(1000);
}  
if (!Config.自动瞄准.屏幕位置)
Touch_Move(6, (int)tx, (int)ty);
else
Touch_Move(6, screen_y - (int)tx, screen_x - (int)ty);
isDown = true;
usleep(Config.自动瞄准.锁定强度 * 1000);
} else {
IsAimLongAim = false;
if (isDown) {
tx = Config.自动瞄准.触摸位置X, ty = Config.自动瞄准.触摸位置Y;
// 恢复变量 
Touch_Up(6);
// 抬起
isDown = false;
// 延时
usleep(Config.自动瞄准.锁定强度 * 1000);
}
}  
}
}


int DrawInt() {
const char *pkgs[] = {
    "com.tencent.ig",
    "com.pubg.krmobile",
    "com.vng.pubgmobile",
    "com.rekoo.pubgm",
    "com.tencent.tmgp.pubgmhd"
};
进程 = 0;
for (size_t i = 0; i < sizeof(pkgs) / sizeof(pkgs[0]); ++i) {
    pid_t candidate = 获取进程((char *)pkgs[i]);
    if (candidate > 0 && candidate < 100000) {
        进程 = candidate;
        break;
    }
}
if (进程 <= 0) {
    printf("\n [>]获取游戏Pid失败！\n");
    exit(1);
    return -1;
}
driver->initialize(进程);
基址头 = driver->get_module_base((char *)"libUE4.so");
return 0;
}
void DrawPlayer(ImDrawList *draw) {
 const std::string filePath = "/data/EACF.txt";
    int xx, yy;
    ReadResolutionFromFile(filePath, xx, yy);
   // printf("%d %d\n", xx, yy);


     px = xx / 2; // 计算屏幕的水平中心
     py = yy / 2; // 计算屏幕的垂直中心
  //  printf("px: %d, py: %d\n", px, py);
// 自瞄范围
if(Config.自动瞄准.自瞄开关){
if(Config.自动瞄准.动态范围){
ImGui::GetForegroundDrawList()->AddCircle({px, py}, AimbotFov,ImColor(255,188,000,255),0,1.5f); 
} else {
ImGui::GetForegroundDrawList()->AddCircle({px, py}, Config.自动瞄准.自瞄范围,ImColor(255,188,000,255),0,1.5f); 
}
}

if (Config.自动瞄准.触摸位置) {
ImGui::GetForegroundDrawList()->AddRectFilled({0,0}, {px*2, py*2},ImColor(0,0,0,110));
std::string ssf;
ssf += "勿放控件，长按拖动";
ImGui::GetForegroundDrawList()->AddRectFilled({Config.自动瞄准.触摸位置Y - Config.自动瞄准.触摸范围/2, py*2 - Config.自动瞄准.触摸位置X + Config.自动瞄准.触摸范围/2}, {Config.自动瞄准.触摸位置Y + Config.自动瞄准.触摸范围/2, py*2 - Config.自动瞄准.触摸位置X - Config.自动瞄准.触摸范围/2},ImColor(255,255,0,100)); 
ImGui::GetForegroundDrawList()->AddText(NULL,32,{Config.自动瞄准.触摸位置Y - 12,py*2 - Config.自动瞄准.触摸位置X + 12},ImColor(255,255,255),ssf.c_str());
}

using namespace zk_offsets;
uintptr_t world_proxy = driver->read<uintptr_t>(基址头 + kUWorld);
uintptr_t 矩阵地址 = 0;
uintptr_t 世界地址 = 0;
uintptr_t 持久化世界 = 0;
if (world_proxy) {
    uintptr_t matrix_holder = driver->read<uintptr_t>(world_proxy + kWorldToMat_C0);
    if (matrix_holder) {
        矩阵地址 = matrix_holder + kMat_Proj;
    }
    世界地址 = driver->read<uintptr_t>(world_proxy + 0x30);
}
if (世界地址) {
    持久化世界 = driver->read<uintptr_t>(世界地址 + 0x20);
}
long int 对象列阵 = 持久化世界 ? driver->read<uintptr_t>(持久化世界 + 0xA0) : 0;
long int 对象数量 = 持久化世界 ? driver->read<int>(持久化世界 + 0xA8) : 0;
自身结构体 = 世界地址 ? driver->read<uintptr_t>(世界地址 + 0x450) : 0;
int 自身队伍主 = 自身结构体 ? driver->read<int>(自身结构体 + 0x938) : 0;
int 自身队伍副 = 自身结构体 ? driver->read<int>(自身结构体 + 0xAC0) : 0;
int 自身队伍 = 自身队伍主 > 0 ? 自身队伍主 : 自身队伍副;
memset(matrix, 0, sizeof(matrix));
if (矩阵地址) {
    driver->read(矩阵地址, matrix, 16 * 4);
}

玩家数量 = 0; 
人机数量 = 0;
AimCount = 0;
AimObjCount = 0;

int 最大数量 = 对象数量 > 0 ? 对象数量 : 0;
if (最大数量 > 800) {
    最大数量 = 800;
}
for (int i = 0; i < 最大数量; i++) {
    uintptr_t 对象结构体 = driver->read<uintptr_t>(对象列阵 + 0x8 * i);
    if (对象结构体 <= 0x10000000 || 对象结构体 % 4 != 0) {
        continue;
    }

    uintptr_t object = driver->read<uintptr_t>(对象结构体 + 0x1B0);
    if (object <= 0x10000000 || object % 4 != 0) {
        continue;
    }

    getUTF8(PlayerName, driver->read<uintptr_t>(对象结构体 + 0x900));

    int 阵营主 = driver->read<int>(对象结构体 + 0x938);
    int 阵营副 = driver->read<int>(对象结构体 + 0xAC0);
    int 敌人阵营 = 阵营主 > 0 ? 阵营主 : 阵营副;

    int 人机判断 = driver->read<int>(对象结构体 + 0xAB4) ? 1 : 0;
    long int renji标记 = driver->read<int>(对象结构体 + 0x9e9);
    if (renji标记 == 16842753 || renji标记 == 16843009 || renji标记 == 16843008) {
        人机判断 = 1;
    }

    int 人物状态 = driver->read<int>(对象结构体 + 0xFA0);
    float MinHealth = driver->read<float>(对象结构体 + 0xDB8);
    float MaxHealth = driver->read<float>(对象结构体 + 0xDBC);

    int MyWeapon = 0;
    if (自身结构体) {
        MyWeapon = driver->read<int>(
            driver->read<uintptr_t>(
                driver->read<uintptr_t>(
                    driver->read<uintptr_t>(自身结构体 + 0x27a8) + 0x558) +
                0x11b8) +
            0x178);
    }
    int scwq = driver->read<int>(
        driver->read<uintptr_t>(
            driver->read<uintptr_t>(
                driver->read<uintptr_t>(对象结构体 + 0x27a8) + 0x558) +
            0x11b8) +
        0x178);
    if (scwq == 0) {
        int alt = driver->read<int>(
            driver->read<uintptr_t>(
                driver->read<uintptr_t>(对象结构体 + 0x3190) + 0x678) +
            0xBD0);
        if (alt != 0) {
            scwq = alt;
        }
    }
    int sczd = driver->read<int>(
        driver->read<uintptr_t>(
            driver->read<uintptr_t>(对象结构体 + 0x2528) + 0x558) +
        0xF98);
    int zdmax = driver->read<int>(
        driver->read<uintptr_t>(
            driver->read<uintptr_t>(对象结构体 + 0x2528) + 0x558) +
        0xFB0);

    int VHPD = driver->read<int>(对象结构体 + 0x9c0);
    int VHID = VHPD;

    Vector3A Z{};
    if (自身结构体) {
        driver->read(driver->read<uintptr_t>(自身结构体 + 0x1B0) + 0x1C0, &Z, sizeof(Z));
    }

    Vector3A D{};
    driver->read(object + 0x1C0, &D, sizeof(D));

    Vector3A Movement{};
    if (driver->read<uintptr_t>(对象结构体 + 0x1c0)) {
        driver->read(driver->read<uintptr_t>(对象结构体 + 0xD98) + 0x1C0, &Movement, sizeof(Movement));
    } else {
        driver->read(object + 0x260, &Movement, sizeof(Movement));
    }

    float angle = driver->read<float>(对象结构体 + 0x1570) - 90;
    Vector2A Radar = rotateCoord(angle, (Z.X - D.X) / 200, (Z.Y - D.Y) / 200);

    if (自身队伍 == 敌人阵营 || 敌人阵营 < 1) {
        continue;
    }

    if (人物状态 == 262144 || 人物状态 == 262152 || 人物状态 == 262145 || 人物状态 == 0) {
        continue;
    }

    if (MaxHealth <= 0) {
        continue;
    }

    if (driver->read<float>(对象结构体 + 0x2A68) != 479.5f) {
        continue;
    }

    if (Config.人物绘制.忽略人机 && 人机判断 == 1) {
        continue;
    }

camera = matrix[3] * D.X + matrix[7] * D.Y + matrix[11] * D.Z + matrix[15];
r_x = px + (matrix[0] * D.X + matrix[4] * D.Y + matrix[8] * D.Z + matrix[12]) / camera * px;
r_y = py - (matrix[1] * D.X + matrix[5] * D.Y + matrix[9] * (D.Z - 5) + matrix[13]) / camera * py;
r_w = py - (matrix[1] * D.X + matrix[5] * D.Y + matrix[9] * (D.Z + 205) + matrix[13]) / camera * py;

float X = r_x - (r_y - r_w) / 4;
float Y = r_y;
float W = (r_y - r_w) / 2;

float MIDDLE = X + W / 2;
float BOTTOM = Y + W;
float TOP = Y - W;

float 距离 = sqrt(pow(D.X - Z.X, 2) + pow(D.Y - Z.Y, 2) + pow(D.Z - Z.Z, 2)) * 0.01;

float left = (X + W / 2) - W / 2.6f;
float right = X + W / 1.12f;

long int Mesh = driver->read<uintptr_t>(对象结构体 + 0x4A8);

long int human = Mesh + 0x1B0;

long int Bone = driver->read<uintptr_t>(Mesh + 0x8A8) + 0x30;

FTransform meshtrans = getBone(human);
FMatrix c2wMatrix = TransformToMatrix(meshtrans);

// 头部
FTransform headtrans = getBone(Bone + 5 * 48);
FMatrix boneMatrix = TransformToMatrix(headtrans);
Vector3A relLocation = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
relLocation.Z += 7;// 脖子长度
Head = WorldToScreen(relLocation, matrix, camera);

// 胸部
FTransform chesttrans = getBone(Bone + 4 * 48);
FMatrix boneMatrix1 = TransformToMatrix(chesttrans);
Vector3A relLocation1 = MarixToVector(MatrixMulti(boneMatrix1, c2wMatrix));
Chest = WorldToScreen(relLocation1, matrix, camera);

// 盆骨
FTransform pelvistrans = getBone(Bone + 11 * 48);
FMatrix boneMatrix2 = TransformToMatrix(pelvistrans);
Vector3A LrelLocation1 = MarixToVector(MatrixMulti(boneMatrix2, c2wMatrix));
Pelvis = WorldToScreen(LrelLocation1, matrix, camera);

// 左肩膀
FTransform lshtrans = getBone(Bone + 32 * 48);
FMatrix boneMatrix3 = TransformToMatrix(lshtrans);
 Vector3A relLocation2 = MarixToVector(MatrixMulti(boneMatrix3, c2wMatrix));
Left_Shoulder = WorldToScreen(relLocation2, matrix, camera);

// 右肩膀
FTransform rshtrans = getBone(Bone + 12 * 48);
FMatrix boneMatrix4 = TransformToMatrix(rshtrans);
Vector3A relLocation3 = MarixToVector(MatrixMulti(boneMatrix4, c2wMatrix));
Right_Shoulder = WorldToScreen(relLocation3, matrix, camera);

// 左手肘
FTransform lelbtrans = getBone(Bone + 33 * 48);
FMatrix boneMatrix5 = TransformToMatrix(lelbtrans);
Vector3A relLocation4 = MarixToVector(MatrixMulti(boneMatrix5, c2wMatrix));
Left_Elbow = WorldToScreen(relLocation4, matrix, camera);

// 右手肘
FTransform relbtrans = getBone(Bone + 63 * 48);
FMatrix boneMatrix6 = TransformToMatrix(relbtrans);
Vector3A relLocation5 = MarixToVector(MatrixMulti(boneMatrix6, c2wMatrix));
Right_Elbow = WorldToScreen(relLocation5, matrix, camera);
 
// 左手腕
FTransform lwtrans = getBone(Bone + 62 * 48);
FMatrix boneMatrix7 = TransformToMatrix(lwtrans);
Vector3A relLocation6 = MarixToVector(MatrixMulti(boneMatrix7, c2wMatrix));
Left_Wrist = WorldToScreen(relLocation6, matrix, camera);

// 右手腕
FTransform rwtrans = getBone(Bone + 1 * 48);
FMatrix boneMatrix8 = TransformToMatrix(rwtrans);
Vector3A relLocation7 = MarixToVector(MatrixMulti(boneMatrix8, c2wMatrix));
Right_Wrist = WorldToScreen(relLocation7, matrix, camera);

// 左大腿
FTransform Llshtrans = getBone(Bone + 52 * 48);
FMatrix boneMatrix9 = TransformToMatrix(Llshtrans);
Vector3A LrelLocation2 = MarixToVector(MatrixMulti(boneMatrix9, c2wMatrix));
Left_Thigh = WorldToScreen(LrelLocation2, matrix, camera);

// 右大腿
FTransform Lrshtrans = getBone(Bone + 56 * 48);
FMatrix boneMatrix10 = TransformToMatrix(Lrshtrans);
Vector3A LrelLocation3 = MarixToVector(MatrixMulti(boneMatrix10, c2wMatrix));
Right_Thigh = WorldToScreen(LrelLocation3, matrix, camera);

// 左膝盖
FTransform Llelbtrans = getBone(Bone + 53 * 48);
FMatrix boneMatrix11 = TransformToMatrix(Llelbtrans);
Vector3A LrelLocation4 = MarixToVector(MatrixMulti(boneMatrix11, c2wMatrix));
Left_Knee = WorldToScreen(LrelLocation4, matrix, camera);

// 右膝盖
FTransform Lrelbtrans = getBone(Bone + 57 * 48);
FMatrix boneMatrix12 = TransformToMatrix(Lrelbtrans);
Vector3A LrelLocation5 = MarixToVector(MatrixMulti(boneMatrix12, c2wMatrix));
Right_Knee = WorldToScreen(LrelLocation5, matrix, camera);

// 左脚腕
FTransform Llwtrans = getBone(Bone + 54 * 48);
FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
Vector3A LrelLocation6 = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
Left_Ankle = WorldToScreen(LrelLocation6, matrix, camera);

// 右脚腕
FTransform Lrwtrans = getBone(Bone + 58 * 48);
FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
Vector3A LrelLocation7 = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
Right_Ankle = WorldToScreen(LrelLocation7, matrix, camera);

// Max
FTransform Maxs = getBone(Bone + 6 * 48);
FMatrix boneMatrix15 = TransformToMatrix(Maxs);
Vector3A Max = MarixToVector(MatrixMulti(boneMatrix15, c2wMatrix));
// Min
FTransform Mins = getBone(Bone + 58 * 48);
FMatrix boneMatrix16 = TransformToMatrix(Mins);
Vector3A Min = MarixToVector(MatrixMulti(boneMatrix16, c2wMatrix));

FTransform Rootz = getBone(Bone + 0 * 48);
FMatrix boneMatrix17 = TransformToMatrix(Rootz);
Vector3A relLocation99 = MarixToVector(MatrixMulti(boneMatrix17, c2wMatrix));
auto Roots = WorldToScreen(relLocation99, matrix, camera);

// 自瞄头部
FTransform headtransA = getBone(Bone + 5 * 48);
FMatrix boneMatrixA1 = TransformToMatrix(headtransA);
Vector3A relLocationA1 = MarixToVector(MatrixMulti(boneMatrixA1, c2wMatrix));
AimHead = WorldToScreen(relLocationA1, matrix, camera);
// 自瞄胸部
FTransform chesttransA = getBone(Bone + 4 * 48);
FMatrix boneMatrixA2 = TransformToMatrix(chesttransA);
Vector3A relLocationA2 = MarixToVector(MatrixMulti(boneMatrixA2, c2wMatrix));
AimChest = WorldToScreen(relLocationA2, matrix, camera);
// 自瞄盆骨
FTransform pelvistransA = getBone(Bone + 11 * 48);
FMatrix boneMatrixA3 = TransformToMatrix(pelvistransA);
Vector3A LrelLocationA3 = MarixToVector(MatrixMulti(boneMatrixA3, c2wMatrix));
AimPelvis = WorldToScreen(LrelLocationA3, matrix, camera);
/*
if(Head.X < px){//左
// 头部
relLocationA1.X -= Config.自动瞄准.预判参数 *100;
} else if (Head.X > px) {//右
relLocationA1.X += Config.自动瞄准.预判参数 *100;
}
if(Head.X < px){//左
// 胸部
relLocationA2.X -= Config.自动瞄准.预判参数 *100;
} else if (Head.X > px) {//右
relLocationA2.X += Config.自动瞄准.预判参数 *100;
}
if(Head.X < px){//左
// 盆骨
LrelLocationA3.X -= Config.自动瞄准.预判参数 *100;
} else if (Head.X > px) {//右
LrelLocationA3.X += Config.自动瞄准.预判参数 *100;
}
*/
if (LrelLocation6.Z < LrelLocation7.Z) {
LrelLocation7.Z = LrelLocation6.Z;
}

float top1 = Pelvis.Y - Head.Y;
float top = Pelvis.Y - top1 - W / 5;

if (Left_Ankle.Y < Right_Ankle.Y) {
float bottom = Right_Ankle.Y + W / 10;
} else {
float bottom = Left_Ankle.Y+ W / 10;
}

/*
if(Head.X < px){//左
} else if (Head.X > px) {//右
}
*/

if (W > 0){
if ((Config.自动瞄准.忽略倒地 == false || (Config.自动瞄准.忽略倒地 && MinHealth > 0)) && (Config.自动瞄准.忽略人机 == false || (Config.自动瞄准.忽略人机 && 人机判断 == 0))) {
Aim[AimCount].WodDistance = 距离;
Aim[AimCount].AimMovement = Movement;
if (Config.自动瞄准.瞄准部位 == 自瞄部位::头部){
Aim[AimCount].ObjAim = relLocationA1;
Aim[AimCount].ScreenDistance = sqrt(pow(px - AimHead.X, 2) + pow(py - AimHead.Y, 2));
}else if (Config.自动瞄准.瞄准部位 == 自瞄部位::胸部){
Aim[AimCount].ObjAim = relLocationA2;
Aim[AimCount].ScreenDistance = sqrt(pow(px - AimChest.X, 2) + pow(py - AimChest.Y, 2));
}else if (Config.自动瞄准.瞄准部位 == 自瞄部位::腹部){
Aim[AimCount].ObjAim = LrelLocationA3;
Aim[AimCount].ScreenDistance = sqrt(pow(px - AimPelvis.X, 2) + pow(py - AimPelvis.Y, 2));
}
//绘制自瞄线
if(Config.自动瞄准.自瞄开关){
if(findminat()==AimCount) {
if(Config.自动瞄准.瞄准部位 == 自瞄部位::头部){
ImGui::GetForegroundDrawList()->AddLine({px , py}, {Head.X, Head.Y}, ImColor(255,255,0), {1.5});  
}else if (Config.自动瞄准.瞄准部位 == 自瞄部位::胸部){
ImGui::GetForegroundDrawList()->AddLine({px , py}, {Chest.X, Chest.Y}, ImColor(255,255,0), {1.5});
}else if (Config.自动瞄准.瞄准部位 == 自瞄部位::腹部){
ImGui::GetForegroundDrawList()->AddLine({px , py}, {Pelvis.X, Pelvis.Y}, ImColor(255,255,0), {1.5});
}
}
}
AimCount++;
}
}

// 雷达背景
if (Config.预警绘制.绘制雷达){
ImGui::GetForegroundDrawList()->AddCircleFilled({Config.预警绘制.雷达X轴,Config.预警绘制.雷达Y轴},150,ImColor(0,0,0,60));
ImGui::GetForegroundDrawList()->AddCircle({Config.预警绘制.雷达X轴,Config.预警绘制.雷达Y轴}, 150,ImColor(255, 255, 255, 255));
ImGui::GetForegroundDrawList()->AddCircle({Config.预警绘制.雷达X轴,Config.预警绘制.雷达Y轴}, 60,ImColor(255, 255, 255, 255));
ImGui::GetForegroundDrawList()->AddLine({Config.预警绘制.雷达X轴+150,Config.预警绘制.雷达Y轴}, {Config.预警绘制.雷达X轴-150,Config.预警绘制.雷达Y轴}, ImColor(255, 255, 255, 255), 2);
ImGui::GetForegroundDrawList()->AddLine({Config.预警绘制.雷达X轴,Config.预警绘制.雷达Y轴+150}, {Config.预警绘制.雷达X轴,Config.预警绘制.雷达Y轴-150}, ImColor(255, 255, 255, 255), 2);
}
if (Config.预警绘制.绘制雷达) {
if (距离 <= 300) {  
if(人机判断==1) {
ImGui::GetForegroundDrawList()->AddCircleFilled({Config.预警绘制.雷达X轴 + Radar.X,Config.预警绘制.雷达Y轴 + Radar.Y}, {4.5}, ImColor(255,255,255));
}else{
ImGui::GetForegroundDrawList()->AddCircleFilled({Config.预警绘制.雷达X轴 + Radar.X,Config.预警绘制.雷达Y轴 + Radar.Y}, {4.5}, ImColor(255,0,0));
}
}
}

if (Config.载具绘制.绘制载具) {
std::string s;
s += GetVehicleName(VHID);
s += "－－－";
s += "[";
s += std::to_string((int)距离);
s += "M]";
if (距离 > 500.0f) {
continue;
} 
绘制字体描边(22.5, X - 25, Y,ImColor(248,248,255),s.c_str());
}

if (Config.预警绘制.背敌预警) {
std::string ssd;
ssd += std::to_string((int) 距离);
ssd += "m";
auto textSize = ImGui::CalcTextSize(ssd.c_str(), nullptr, true);
if(人机判断 == 1){
if (X + W / 2 < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({0, Head.Y}, 60, ImColor(255, 255, 255, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {5, Head.Y - 20}, ImColor(000, 000, 000, 200),ssd.c_str());
} else if (W > 0 && X > px * 2) {
ImGui::GetForegroundDrawList()->AddCircleFilled({px * 2, Head.Y}, 60, ImColor(255, 255, 255, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {px * 2 - 50, Head.Y - 20},ImColor(000, 000, 000, 200),ssd.c_str());
} else if (W > 0 && Y + W < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({Head.X, 0}, 60, ImColor(255, 255, 255, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {Head.X - 30, 10}, ImColor(000, 000, 000, 200),ssd.c_str());
} else if (W < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({px * 2 - Head.X, py * 2}, 60, ImColor(255, 255, 255, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {px * 2 - Head.X - 30, py * 2 - 30},ImColor(000, 000, 000, 200), ssd.c_str());
}
}else{
if (X + W / 2 < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({0, Head.Y}, 60, ImColor(255, 000, 000, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {5, Head.Y - 20}, ImColor(255, 255, 255, 200),ssd.c_str());
} else if (W > 0 && X > px * 2) {
ImGui::GetForegroundDrawList()->AddCircleFilled({px * 2, Head.Y}, 60, ImColor(255, 000, 000, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {px * 2 - 50, Head.Y - 20},ImColor(255, 255, 255, 200),ssd.c_str());
} else if (W > 0 && Y + W < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({Head.X, 0}, 60, ImColor(255, 000, 000, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {Head.X - 30, 10}, ImColor(255, 255, 255, 200),ssd.c_str());
} else if (W < 0) {
ImGui::GetForegroundDrawList()->AddCircleFilled({px * 2 - Head.X, py * 2}, 60, ImColor(255, 000, 000, 125));
ImGui::GetForegroundDrawList()->AddText(nullptr, 35, {px * 2 - Head.X - 30, py * 2 - 30},ImColor(255, 255, 255, 200), ssd.c_str());
}
}
}

if(W > 0) {

if (Config.人物绘制.绘制方框) {
if (BOX == BOX::box1) {
if (人机判断 == 1) {
ImGui::GetForegroundDrawList()->AddRect({X, TOP}, {X + W, BOTTOM},ImColor(255, 255, 255, 255), 0, 0, 1.3f);
} else {
ImGui::GetForegroundDrawList()->AddRect({X, TOP}, {X + W, BOTTOM},ImColor(255, 000, 000, 255), {0}, 0, {1.5});
}
}
if (BOX == BOX::box2) {
float boxHeight = abs(Head.X - Roots.Y);
float boxWidth = boxHeight * 0.65f;
if (人机判断 == 1) {
Box4Line(draw, 0.5f, Head.X - (boxWidth / 2), Head.Y, boxWidth, boxHeight,  IM_COL32(255, 255, 255, 255));
} else {
Box4Line(draw, 0.5f, Head.X - (boxWidth / 2), Head.Y, boxWidth, boxHeight,  IM_COL32(255, 255, 255, 255));
}      
}
if (BOX == BOX::box3) {
}
}

if (Config.人物绘制.绘制射线) {
//射线
if (LINE == LINE::line1) {
if(人机判断 == 1){
ImGui::GetForegroundDrawList()->AddLine({px , 160.5f}, {Head.X, TOP - 50}, ImColor(255, 255, 255, 255), {1.5});  
}else{
ImGui::GetForegroundDrawList()->AddLine({px , 160.5f}, {Head.X, TOP - 50}, ImColor(000, 255, 255, 255), {1.5});
}
}
if (LINE == LINE::line2) {
if(人机判断 == 1){
ImGui::GetForegroundDrawList()->AddLine({px, py}, {Head.X, Head.Y}, ImColor(255, 255, 255, 255), {1.5});
}else{
ImGui::GetForegroundDrawList()->AddLine({px, py}, {Head.X, Head.Y}, ImColor(000, 255, 255, 255), {1.5});
}
}
if (LINE == LINE::line3) {
if(人机判断 == 1){
ImGui::GetForegroundDrawList()->AddLine({px, 1000}, {Head.X, Head.Y}, ImColor(255, 255, 255, 255), {1.5});
}else{
ImGui::GetForegroundDrawList()->AddLine({px, 1000}, {Head.X, Head.Y}, ImColor(000, 255, 255, 255), {1.5});
}
}
}

if (Config.人物绘制.绘制骨骼) {
//骨骼
if (人机判断 == 1) {
/*ImGui::GetForegroundDrawList()->AddCircle({Head.X , Head.Y}, W/5, ImColor(255, 255, 255, 255), 0);
ImGui::GetForegroundDrawList()->AddLine({Head.X, Head.Y}, {Chest.X, Chest.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Pelvis.X, Pelvis.Y},ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Left_Shoulder.X,Left_Shoulder.Y},ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Right_Shoulder.X,Right_Shoulder.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Shoulder.X, Left_Shoulder.Y}, {Left_Elbow.X,Left_Elbow.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Shoulder.X, Right_Shoulder.Y},{Right_Elbow.X, Right_Elbow.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Elbow.X, Left_Elbow.Y}, {Left_Wrist.X,Left_Wrist.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Elbow.X, Right_Elbow.Y}, {Right_Wrist.X,Right_Wrist.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Left_Thigh.X, Left_Thigh.Y},ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Right_Thigh.X,Right_Thigh.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Thigh.X, Left_Thigh.Y}, {Left_Knee.X,Left_Knee.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Thigh.X, Right_Thigh.Y}, {Right_Knee.X,Right_Knee.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Knee.X, Left_Knee.Y}, {Left_Ankle.X,Left_Ankle.Y}, ImColor(255, 255, 255, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Knee.X, Right_Knee.Y}, {Right_Ankle.X,Right_Ankle.Y}, ImColor(255, 255, 255, 255),{2});*/
}else{
ImGui::GetForegroundDrawList()->AddCircle({Head.X , Head.Y}, W/5, ImColor(255, 000, 000, 255), 0);
ImGui::GetForegroundDrawList()->AddLine({Head.X, Head.Y}, {Chest.X, Chest.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Pelvis.X, Pelvis.Y},ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Left_Shoulder.X,Left_Shoulder.Y},ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Right_Shoulder.X,Right_Shoulder.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Shoulder.X, Left_Shoulder.Y}, {Left_Elbow.X,Left_Elbow.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Shoulder.X, Right_Shoulder.Y},{Right_Elbow.X, Right_Elbow.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Elbow.X, Left_Elbow.Y}, {Left_Wrist.X,Left_Wrist.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Elbow.X, Right_Elbow.Y}, {Right_Wrist.X,Right_Wrist.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Left_Thigh.X, Left_Thigh.Y},ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Right_Thigh.X,Right_Thigh.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Thigh.X, Left_Thigh.Y}, {Left_Knee.X,Left_Knee.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Thigh.X, Right_Thigh.Y}, {Right_Knee.X,Right_Knee.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Left_Knee.X, Left_Knee.Y}, {Left_Ankle.X,Left_Ankle.Y}, ImColor(255, 000, 000, 255),{2});
ImGui::GetForegroundDrawList()->AddLine({Right_Knee.X, Right_Knee.Y}, {Right_Ankle.X,Right_Ankle.Y}, ImColor(255, 000, 000, 255),{2});  
}
}

if (Config.人物绘制.绘制信息) {
ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 95, TOP - 58}, {MIDDLE - 50,TOP - 30}, ImColor(000,000,000,120), 1, 0);
ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 50, TOP - 58}, {MIDDLE + 95,TOP - 30}, ImColor(队伍颜色(敌人阵营)), 1, 0);
ImGui::GetForegroundDrawList()->AddTriangleFilled(ImVec2(MIDDLE + 25 / 2, TOP - 25 ), ImVec2(MIDDLE - 25 / 2, TOP - 25), ImVec2(MIDDLE + 0 / 2, TOP - 10), ImColor(队伍颜色(敌人阵营)));
std::string s;
auto textSize = ImGui::CalcTextSize(s.c_str(), 0); 
if(敌人阵营 < 10 ){
s +="0";
s += std::to_string(敌人阵营);
ImGui::GetForegroundDrawList()->AddText(NULL, 25, {MIDDLE - 86 - (textSize.x / 2), TOP - 56}, ImColor(255, 255, 255, 255),s.c_str());
} else if(敌人阵营 >= 10 && 敌人阵营 < 100){
s += std::to_string(敌人阵营);
ImGui::GetForegroundDrawList()->AddText(NULL, 25, {MIDDLE - 86 - (textSize.x / 2), TOP - 56}, ImColor(255, 255, 255, 255),s.c_str());
} else if(敌人阵营 >= 100){
s += std::to_string(敌人阵营);
ImGui::GetForegroundDrawList()->AddText(NULL, 25, {MIDDLE - 88 - (textSize.x / 2), TOP - 56}, ImColor(255, 255, 255, 255),s.c_str());
}
if (人机判断 == 1) {
ImGui::GetForegroundDrawList()->AddText(NULL, 24, {MIDDLE - 48, TOP - 56}, ImColor(255, 255, 255, 255), " AI[ROBOT]");
} else { 
ImGui::GetForegroundDrawList()->AddText(NULL, 24, {MIDDLE - 48, TOP - 56}, ImColor(255, 255, 255, 255), PlayerName);
}
}

if(Config.人物绘制.绘制血量){
int CurHP = (int) std::max(0, std::min((int)MinHealth,100));
ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 95, TOP - 29}, {MIDDLE - 95 + (1.9f * CurHP), TOP - 25}, ImColor(255,255,255,100), 1);
if (MinHealth <= 0.0f && (人物状态 == 131072 || 人物状态 == 131073)) {
ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 95, TOP - 29}, {MIDDLE - 95 + (1.9f * MaxHealth), TOP - 25}, ImColor(255,000,000,255), 1);
}
}

if (Config.人物绘制.绘制距离) {
std::string s;
s += std::to_string((int)距离);
s += "M";
绘制加粗文本(18,MIDDLE - 20,BOTTOM + 10,ImColor(255,255,255,255),黑色,s.c_str());
}

if (Config.人物绘制.手持武器) {
std::string s; 
s += " ";
s += playerstatus(人物状态);
s += " [";
s += 获取枪械信息(scwq);
s += " ";
s += std::to_string((int) sczd);
s += "/";
s += std::to_string((int) zdmax);
s += "]";
绘制加粗文本(18,MIDDLE - 71,TOP - 76,白色,黑色,s.c_str());
}

}
if (人机判断 == 1) {
    人机数量++;
} else {
    玩家数量++;
}
}

MaxPlayerCount = AimCount;

if(玩家数量 <= 9){
std::string str1;
str1 += ""; 
str1 += to_string((int)玩家数量);
ImGui::GetForegroundDrawList()->AddText(NULL, 50, ImVec2(px - 10.0f, 35), ImColor(255, 000, 000, 255), str1.c_str());
} else if(玩家数量 >= 10) {
std::string str1;
str1 += ""; 
str1 += to_string((int)玩家数量);
ImGui::GetForegroundDrawList()->AddText(NULL, 50, ImVec2(px - 16.5f, 35), ImColor(255, 000, 000, 255), str1.c_str());
}
std::string str2;
str2 += "MORISHIMA"; 
ImGui::GetForegroundDrawList()->AddText(NULL, 50, ImVec2(px - 112.0f,90.0f), ImColor(255, 000, 000, 255), str2.c_str());

ImGui::GetForegroundDrawList()->AddCircle({px,160.5f}, 10, ImColor(255,255,000,255), 0, 2.5f); 
ImGui::GetForegroundDrawList()->AddCircleFilled({px,160.5f}, 5, ImColor(255,255,000,255)); 

/*
if(PlayerCount > 0 || RobotCount > 0){
char 人机数量[50];
char 玩家数量[50];
sprintf(玩家数量, "%d", PlayerCount);
sprintf(人机数量, "%d", RobotCount);
ImGui::GetForegroundDrawList()->AddRectFilled({px-220,50},{px-160,100},ImColor(45,45,45,255),4.5f);//玩家
ImGui::GetForegroundDrawList()->AddRectFilled({px-160,50},{px-60,100},ImColor(255,255,255,255),4.5f);
ImGui::GetForegroundDrawList()->AddRectFilled({px+60,50},{px+120,100},ImColor(45,45,45,255),4.5f);//人机
ImGui::GetForegroundDrawList()->AddRectFilled({px+120,50},{px+220,100},ImColor(255,255,255,255),4.5f);

ImGui::GetForegroundDrawList()->AddText(font,40,{px-202,55},ImColor(255,255,255,255),玩家数量);
ImGui::GetForegroundDrawList()->AddText(font,40,{px+78,55},ImColor(255,255,255,255),人机数量);
ImGui::GetForegroundDrawList()->AddText(font,40,{px-150,55},ImColor(000,000,000,255),"玩家");
ImGui::GetForegroundDrawList()->AddText(font,40,{px+130,55},ImColor(000,000,000,255),"人机");
}else{
std::string str;
str += "CLEAR"; 
ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(px - 65, 60), ImVec2(px + 65, 110), ImColor(2, 181, 62, 80), 1.0f);
ImGui::GetForegroundDrawList()->AddRect(ImVec2(px - 65, 60), ImVec2(px + 65, 110), ImColor(2, 181, 62, 255), 1.0f, 0, 2.5f);
ImGui::GetForegroundDrawList()->AddText(NULL, 30, ImVec2(px - 35.0,70), ImColor(255, 255, 255, 255), str.c_str());
}

std::string str2;
str2 += "MORISHIMA-AIM 3.0"; 
绘制字体描边(35.0f, (float) px - 125.0f,110.0f, ImColor(255,000,000,255), str2.c_str());

ImGui::GetForegroundDrawList()->AddCircle({px,160.5f}, 10, ImColor(255,255,000,255), 0, 2.5f); 
ImGui::GetForegroundDrawList()->AddCircleFilled({px,160.5f}, 5, ImColor(255,255,000,255)); 
*/
if (Config.其他绘制.绘制准星){
ImGui::GetForegroundDrawList()->AddLine({px+25, py}, {px-25,py}, ImColor(255, 255, 255, 255), 3);
ImGui::GetForegroundDrawList()->AddLine({px, py+25}, {px, py-25}, ImColor(255, 255, 255, 255), 3);
}

char FText1[100];
char FText2[100];
//sprintf(FText1," ");
sprintf(FText2,"电报@EACnb");
//绘制字体描边(30.0f, (float)px-1100,70.5f, ImColor(255,255,255,255), FText1);
绘制字体描边(80.0f, (float)px-1100,100.5f, ImColor(255,000,000,255), FText2);

}


void DrawLogo(ImVec2 center, float size) {
ImGui::SetCursorPos({0, 180});
ImGui::SetCursorPos({0, 180});
ImDrawList *draw_list = ImGui::GetWindowDrawList();
draw_list->AddImage(FloatBall,{center.x - size /1,center.y - size / 1},{center.x + size / 1,center.y + size / 1});
}

bool Tabb,interface3,isDown,isTe;

void tick() {
ImGuiIO& io = ImGui::GetIO();
if (display == EGL_NO_DISPLAY)
return;
static ImVec4 clear_color = ImVec4(0, 0, 0, 0);
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplAndroid_NewFrame(init_screen_x, init_screen_y);
ImGui::NewFrame();

ImGuiStyle &style = ImGui::GetStyle();

style.Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);//字体颜色
style.Colors[ImGuiCol_WindowBg] = ImColor(10, 10, 10, 225);//背景色
	
style.Colors[ImGuiCol_Button] = ImColor(147,189,64,255);//按钮颜色
style.Colors[ImGuiCol_ButtonHovered] = ImColor(147,189,64,255);//按钮点击后悬停颜色
style.Colors[ImGuiCol_ButtonActive] = ImColor(147,189,64,255);//按钮点击颜色
	
style.Colors[ImGuiCol_FrameBg] = ImColor(147,189,64,150);//控件内背景色
style.Colors[ImGuiCol_FrameBgActive] = ImColor(147,189,64,160);//控件内点击背景色
style.Colors[ImGuiCol_FrameBgHovered] = ImColor(147,189,64,160);//控件内点击后悬停背景色
/*
style.Colors[ImGuiCol_FrameBg] = ImColor(54,54,54,150);
style.Colors[ImGuiCol_FrameBgActive] = ImColor(42,42,42,150);
style.Colors[ImGuiCol_FrameBgHovered] = ImColor(100,100,100,150);
*/
style.Colors[ImGuiCol_CheckMark] = ImColor(147,189,64,255);//控件Check勾子颜色
	
style.Colors[ImGuiCol_SliderGrab] = ImColor(147,189,64,255);
style.Colors[ImGuiCol_SliderGrabActive] = ImColor(147,189,64,255);
	
style.Colors[ImGuiCol_Header] = ImColor(147,189,64,255);
style.Colors[ImGuiCol_HeaderHovered] = ImColor(147,189,64,255);
style.Colors[ImGuiCol_HeaderActive] = ImColor(147,189,64,255);

style.Colors[ImGuiCol_ScrollbarBg] = ImColor(42,42,42,225);//滑动条背景颜色
style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(147,189,64,255);//滑动条颜色
style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(147,189,64,255);//滚动条点击后悬停
style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(147,189,64,255);//滑动条点击
	
style.Colors[ImGuiCol_Separator] = ImColor(80,80,80,255);

style.GrabRounding = 5.0f;
style.GrabMinSize = 35.0f;

style.FrameRounding = 5.0f;
style.FrameBorderSize = 0.5f;
style.FramePadding = ImVec2(5, 5);

style.ScrollbarSize = 35.0f;
style.ScrollbarRounding = 5.0f;

style.WindowBorderSize = 0.0f;
style.WindowTitleAlign = ImVec2(0.5, 0.5);

if(Config.自动瞄准.自瞄控件) {
style.WindowMinSize = ImVec2(120, 120);//窗口尺寸
if (ImGui::Begin("##自瞄", &interface3,ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
auto Pos = ImGui::GetWindowPos();
if(ImGui::IsItemActive()){
Tabb++;
if(!isDown&&!isTe){
isDown = true;
}else{
isDown = false;
}
}else if(Tabb != 0 && Tabb <= 10){
if(!isDown){
isTe = false;
}else{
isTe = true;
}
}else{
Tabb = 0;
}
if(isTe){
Config.自动瞄准.自瞄开关=true;
ImGui::GetWindowDrawList()->AddImage(onButton, {Pos.x+10, Pos.y+10}, {Pos.x + 90, Pos.y + 90}); 
}else{
Config.自动瞄准.自瞄开关=false;
ImGui::GetWindowDrawList()->AddImage(offButton, {Pos.x+10, Pos.y+10}, {Pos.x + 90, Pos.y + 90});
}
}
}


static bool IsBall = true;
static float ANIM_SPEED = 0.25f;
static float Velua = IsBall ? 0.0f : 1.0f;
Velua = ImClamp(Velua + (io.DeltaTime / ANIM_SPEED) * (IsBall ? 1.0f : -1.0f), 0.0f, 1.0f);
if ((950 * Velua) <= 950 && (600 * Velua) <= 600) {
ImGui::SetWindowSize("##MORISHIMA CHEAT", {950 * Velua, 600 * Velua});
} else if ((950 * Velua) >= 100 && (600 * Velua) >= 100) {
ImGui::SetWindowSize("##MORISHIMA CHEAT", {950 * Velua, 600 * Velua});
} if ((950 * Velua) <= 100 && !IsBall) {BallSwitch = true;MemuSwitch = false;}
if (BallSwitch) {
style.WindowRounding = 80.0f;
if (ImGui::Begin("Ball", &BallSwitch,  ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
auto Pos = ImGui::GetWindowPos();
Window = ImGui::GetCurrentWindow();
DrawLogo({Pos.x + 50, Pos.y + 50}, 35);
if (ImGui::IsItemActive()) {
if (!IsDown) {
IsDown = true;
ImagePos = ImGui::GetWindowPos();
}
} else if (IsDown) {
IsDown = false;
if(ImagePos.x == ImGui::GetWindowPos().x && ImagePos.y == ImGui::GetWindowPos().y) {
IsBall = true;
MemuSwitch = true;
BallSwitch = false;
ImGui::SetWindowPos("##MORISHIMA CHEAT", Pos, ImGuiCond_Always);
}
}
}
ImGui::End();
}

if (MemuSwitch) {
style.WindowRounding = 10.0f;
if (ImGui::Begin("##MORISHIMA CHEAT", &MemuSwitch, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
auto Pos = ImGui::GetWindowPos();
Window = ImGui::GetCurrentWindow();
ImDrawList *draw_list = ImGui::GetWindowDrawList();
ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {25 , 25});
if (ImGui::BeginChild("##主菜单", ImVec2(220, -1), false, ImGuiWindowFlags_NoScrollbar)) {
DrawLogo({Pos.x + 117, Pos.y + 100}, 80);
ImGui::SetCursorPos({0, 180});
ImGui::Separator();
if (ImGui::Button("主页", ImVec2(220, 58))) {
MenuTab = 1;
}
if (ImGui::Button("绘制", ImVec2(220, 58))) {
MenuTab = 2;
}
if (ImGui::Button("自瞄", ImVec2(220, 58))) {
MenuTab = 3;
}
if (ImGui::Button("设置", ImVec2(220, 58))) {
MenuTab = 4;
}
if (ImGui::Button("缩小", ImVec2(220, 58))) {
IsBall = false;
}
}
ImGui::EndChild();
ImGui::PopStyleVar(1);
ImGui::SameLine();
ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
ImGui::SameLine();
ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {25 , 25});
ImGui::SameLine();
if (ImGui::BeginChild("##右菜单", ImVec2(-1, -1), false)) {
ImGui::TextColored(ImColor(147,189,64,225), "[^] PUBGM_kernel");
ImGui::Separator();
switch (MenuTab) {
case 1: {
ImGuiLayout1();
}
break;
case 2: {
ImGuiLayout2();
}
break;
case 3: {
ImGuiLayout3();
}
break;
case 4: {
ImGuiLayout4();
}
break;
}
}
ImGui::EndChild();
ImGui::PopStyleVar(1);
}
ImGui::End();
}
if (IsWin) {
IsWin = false;
IsLoGin = false;
BallSwitch = false;
ImGui::SetWindowSize("Ball", {120.0f, 150.0f});
}

//if (链接驱动) {
DrawPlayer(ImGui::GetForegroundDrawList());
//}
ImGui::End();
ImGui::Render();
FPS = 100000;
glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
eglSwapBuffers(display, surface);
}

void shutdown() {
if (!g_Initialized) {
return;
}
ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplAndroid_Shutdown();
ImGui::DestroyContext();
if (display != EGL_NO_DISPLAY){
eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
if (context != EGL_NO_CONTEXT){
eglDestroyContext(display, context);
}
if (surface != EGL_NO_SURFACE){
eglDestroySurface(display, surface);
}
eglTerminate(display);
}
display = EGL_NO_DISPLAY;
context = EGL_NO_CONTEXT;
surface = EGL_NO_SURFACE;
ANativeWindow_release(native_window);
}

void ImGuiLayout1() {
if (ImGui::BeginChild("主页", ImVec2(ImGui::GetWindowWidth(), -1))) {
if (链接驱动) {
ImGui::Button("链接成功", {-1,55});
} else {
if (ImGui::Button("链接数据",{-1,55})) {
DrawInt();
链接驱动 = true;				
}
}
if (ImGui::Button("退出程序",{-1,55})) {
exit(0);
}
ImGui::Separator();
ImGui::Text(" 服务器运行中 %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]); 
static float values[90]= {};
static int values_offset= 0;
static double refresh_time = 0.0;
while( refresh_time < ImGui::GetTime() ) // Create data at fixed 60 Hz rate for the demo
{
static float phase= 0.0f;
values[values_offset] = cosf( phase );
values_offset = ( values_offset + 1 ) % IM_ARRAYSIZE( values );
phase += 0.10f * values_offset;
refresh_time += 1.0f / 60.0f;
}
char overlay[32] = "";
ImGui::PlotLines("", values, IM_ARRAYSIZE( values ), values_offset, overlay, -1.0f, 1.0f, ImVec2( 0,20.0f ) );
ImGuiIO io = ImGui::GetIO();
ImGui::Text(" 设备帧率波动 %.3f ms 当前帧率: %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); 
ImGui::BulletText("电报@EACnb");
ImGui::BulletText("当前设备分辨率：%dx%d", screen_x,screen_y);
ImGui::BulletText("当前游戏Pid: %d", 进程); 
ImGui::BulletText("libUE4.so: 0x%lX", 基址头);
ImGui::Separator();
}
ImGui::EndChild();
}
void ImGuiLayout2() {
if (ImGui::BeginChild("绘制", ImVec2(ImGui::GetWindowWidth(), -1))) {
if (ImGui::Button("一键绘制",{-1,65})) {
Config.人物绘制.绘制方框 = true;
Config.人物绘制.绘制射线 = true;
Config.人物绘制.绘制骨骼 = true;
Config.人物绘制.绘制信息 = true;
Config.人物绘制.绘制血量 = true;
Config.人物绘制.绘制距离 = true;
Config.预警绘制.绘制雷达 = true;
Config.人物绘制.手持武器 = true;
//Config.其他绘制.盒子绘制 = true;
//Config.载具绘制.绘制载具 = true;
//Config.预警绘制.手雷预警 = true;
Config.预警绘制.背敌预警 = true;
}
ImGui::Checkbox("绘制方框", &Config.人物绘制.绘制方框);
ImGui::SameLine();
ImGui::Checkbox("绘制射线", &Config.人物绘制.绘制射线);
ImGui::SameLine();
ImGui::Checkbox("绘制骨骼", &Config.人物绘制.绘制骨骼);

ImGui::Checkbox("绘制信息", &Config.人物绘制.绘制信息);
ImGui::SameLine();
ImGui::Checkbox("绘制血量", &Config.人物绘制.绘制血量);
ImGui::SameLine();
ImGui::Checkbox("绘制距离", &Config.人物绘制.绘制距离);

ImGui::Checkbox("手持武器", &Config.人物绘制.手持武器);
ImGui::SameLine();
ImGui::Checkbox("绘制雷达", &Config.预警绘制.绘制雷达);
//ImGui::SameLine();
//ImGui::Checkbox("盒子绘制", &Config.其他绘制.盒子绘制);

//ImGui::Checkbox("载具绘制", &Config.载具绘制.绘制载具);
//ImGui::SameLine();
//ImGui::Checkbox("手雷预警", &Config.预警绘制.手雷预警);
ImGui::SameLine();
ImGui::Checkbox("背敌预警", &Config.预警绘制.背敌预警);

ImGui::Checkbox("忽略人机", &Config.人物绘制.忽略人机);
ImGui::SameLine();
ImGui::Checkbox("绘制准星", &Config.其他绘制.绘制准星);
ImGui::Separator();
}
ImGui::EndChild();
}
void ImGuiLayout3() {
if (ImGui::BeginChild("自瞄", ImVec2(ImGui::GetWindowWidth(), -1))) {
ImGui::Checkbox("自瞄开关", &Config.自动瞄准.自瞄开关);
ImGui::SameLine();
ImGui::Checkbox("动态范围", &Config.自动瞄准.动态范围);
ImGui::SameLine();
ImGui::Checkbox("调整触摸位置", &Config.自动瞄准.触摸位置);

if (ImGui::SliderFloat("自瞄范围", &Config.自动瞄准.自瞄范围, 10.0f, 300.0f, "%0.1f"));

ImGui::Checkbox("持续锁定", &Config.自动瞄准.持续锁定);
ImGui::SameLine();
ImGui::Checkbox("忽略倒地", &Config.自动瞄准.忽略倒地);
ImGui::SameLine();
ImGui::Checkbox("忽略人机", &Config.自动瞄准.忽略人机);

static const char *triggers[] = {"开火","开镜","开火/开镜"};
ImGui::Combo("触发方式", (int *) &Config.自动瞄准.触发方式, triggers, 3, -1);

static const char *targets[] = {"头部","胸部","腹部"};
ImGui::Combo("瞄准部位", (int *) &Config.自动瞄准.瞄准部位, targets, 3, -1);

ImGui::RadioButton("准心优先", &Config.自动瞄准.优先方式, 0.0f);
ImGui::SameLine();
ImGui::RadioButton("距离优先", &Config.自动瞄准.优先方式, 1.0f);

ImGui::RadioButton("触摸右屏", &Config.自动瞄准.屏幕位置, 0.0f);      
ImGui::SameLine();
ImGui::RadioButton("触摸左屏", &Config.自动瞄准.屏幕位置, 1.0f);

if (ImGui::CollapsingHeader("基础设置")) {
if (ImGui::SliderFloat("锁定强度", &Config.自动瞄准.锁定强度, 5.0f, 25.0f, "%0.1f"));
if (ImGui::SliderFloat("瞄准速度", &Config.自动瞄准.瞄准速度, 5.0f, 25.0f, "%0.1f"));
if (ImGui::SliderFloat("触摸范围", &Config.自动瞄准.触摸范围, 50.01f, 200.05f, "%0.2f"));
if (ImGui::SliderFloat("子弹速度", &Config.自动瞄准.子弹速度, 500.0f, 1280.0f, "%0.1f"));
if (ImGui::SliderFloat("压枪参数", &Config.自动瞄准.压枪参数, 0.2f, 5.0f, "%0.1f"));
if (ImGui::SliderFloat("预判参数", &Config.自动瞄准.预判参数, 0.00f, 5.0f, "%0.2f"));
if (ImGui::SliderFloat("子弹下坠预判", &Config.自动瞄准.子弹下坠预判, -5.0f, 5.0f, "%0.2f"));
}
ImGui::Separator();
}
ImGui::EndChild();
}
void ImGuiLayout4() {
if (ImGui::BeginChild("设置", ImVec2(ImGui::GetWindowWidth(), -1))) {
if (ImGui::SliderFloat("雷达X轴调整", &Config.预警绘制.雷达X轴, 10.0f, 1400.0f, "%0.1f")) {
}
if (ImGui::SliderFloat("雷达Y轴调整", &Config.预警绘制.雷达Y轴, 10.0f, 1400.0f, "%0.1f")) {
}
ImGui::Text("射线位置:");
ImGui::SameLine();
static const char *ABC1[] = {"屏幕上方","屏幕中间","屏幕下方"};
ImGui::Combo("##LINE", (int *) &LINE, ABC1, 3, -1);
ImGui::Text("方框类型:");
ImGui::SameLine();
static const char *ABC2[] = {"平面方框","四角方框","3D方框"};
ImGui::Combo("##BOX", (int *) &BOX, ABC2, 3, -1);
if (ImGui::Button("保存配置",{-1,70})) {
NumIoSave("SaveNum");
}
if (ImGui::Button("重置配置",{-1,70})) {
ImGuiMenustyle();
}
if (ImGui::Button("退出程序",{-1,70})) {
exit(0);
}
ImGui::Separator();
}
ImGui::EndChild();
}
