#include <string>
#include <iostream>
#include <string.h>
#include <cstdio>
#include <malloc.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <sys/system_properties.h>

void ImGuiLayout1();
void ImGuiLayout2();
void ImGuiLayout3();
void ImGuiLayout4();
int MenuTab = 1;

bool MemuSwitch = true;
bool BallSwitch = true;
ImGuiWindow *Window = nullptr;
void DrawLogo(ImVec2 center, float size);
bool IsLoGin = true, IsDown = false, IsWin = true;
ImVec2 ImagePos = {0, 0};

ImVec2 aimsuspensionPos;// NOLINT

bool isSetAim = false;
bool isAimDown = false;

ImTextureID FloatBall;
ImTextureID offButton;
ImTextureID onButton;

static bool 链接驱动;

int MyWeapon;//自身武器

//射线
enum LINE {
line1 = 0,
line2 = 1,
line3 = 2
};
static LINE LINE;
//方框
enum BOX {
box1 = 0,
box2 = 1,
box3 = 2
};
static BOX BOX;
//自瞄部位
enum 自瞄部位 {
头部 = 0,
胸部 = 1,
腹部 = 2
};
//自瞄触发方式
enum 自瞄触发方式 {
开火 = 0,
开镜 = 1,
开火开镜 = 2
};
struct sConfig {
//人物绘制
struct 人物绘制 {
bool 绘制方框;
bool 绘制射线;
bool 绘制骨骼;
bool 绘制信息;
bool 绘制血量;
bool 绘制距离;
bool 手持武器;
bool 忽略人机;
};
人物绘制 人物绘制{0};
//绘制载具
struct 载具绘制 {
bool 绘制载具;
bool 载具耐久;
bool 载具燃料;
};
载具绘制 载具绘制{0};
//绘制预警
struct 预警绘制 {
bool 绘制雷达;
bool 手雷预警;
bool 背敌预警;
float 雷达X轴; 
float 雷达Y轴; 
};
预警绘制 预警绘制{0};
//绘制其他
struct 其他绘制 {
bool 盒子绘制;
bool 绘制准星;
};
其他绘制 其他绘制{0};
//自瞄
struct 自动瞄准 {
bool 自瞄控件;
bool 自瞄开关;
自瞄部位 瞄准部位;
自瞄触发方式 触发方式;
bool 持续锁定;
bool 动态范围;
bool 瞄准射线;
float 优先方式;
float 屏幕位置;
bool 触摸位置;
float 自瞄范围; 
bool 忽略倒地;
bool 忽略人机;
float 锁定强度;
float 瞄准速度;
float 触摸范围;
float 触摸位置X;
float 触摸位置Y;
float 子弹速度;
float 压枪参数;
float 预判参数;
float 子弹下坠预判;
};
自动瞄准 自动瞄准{0};
};
sConfig Config{0};

static float AimbotFov = Config.自动瞄准.自瞄范围;

struct 颜色数据 {
ImVec4 设计颜色;
};
颜色数据 颜色[100];
ImVec4 设计颜色[100];
int 拼接(int a,int b) {
int t=9;
while(t<b)t=t*10+9;
t=(t+1)*a+b;
return t;
}
ImVec4 随机(){
int 红;
int 绿;
int 蓝;
红=(rand()%255+0);
绿=(rand()%255+0);
蓝=(rand()%255+0);
if(sizeof(红)==1||sizeof(红)==0)
红=拼接(红,红);
if(sizeof(绿)==1||sizeof(绿)==0)
绿=拼接(绿,绿);
if(sizeof(蓝)==1||sizeof(蓝)==0)
蓝=拼接(蓝,蓝);
return {(float)红/255.0f,(float)绿/255.0f,(float)蓝/255.0f,174/255.0f};
}
void 颜色初始化(){
srand((unsigned)time(NULL)); /*随机数*/
for(int 数量=0;数量<100;数量++){
设计颜色[数量]=随机();
}
}
ImVec4 获取颜色(int 序号){
return 设计颜色[序号-1];
}
ImVec4 队伍颜色(int 队伍){
if(队伍<=100 && 队伍>0)
return 获取颜色(队伍);
else
return {0/255.f,255/255.f,0/255.f,50/255.f};
}

std::string playerstatus(int GetEnemyState) {
std::string StateName = "未知状态";
if (GetEnemyState == 0) {
 StateName = "静止状态"; 
}
if (GetEnemyState == 1) {
 StateName = "移动中"; 
}
if (GetEnemyState == 8) {
 StateName = "站立"; 
}
if (GetEnemyState == 9) {
 StateName = "行走"; 
}
if (GetEnemyState == 11 || GetEnemyState == 10) {
 StateName = "奔跑"; 
}
if (GetEnemyState == 16) {
 StateName = "蹲下"; 
}
if (GetEnemyState >= 17 && GetEnemyState <= 19) {
 StateName = "蹲走"; 
}
if (GetEnemyState >= 33 && GetEnemyState <= 35) {
 StateName = "爬行"; 
}
if (GetEnemyState == 32) {
 StateName = "趴下"; 
}
if (GetEnemyState >= 60 && GetEnemyState <= 100) {
 StateName = "跳跃"; 
}
if (GetEnemyState >= 260 && GetEnemyState <= 290) {
 StateName = "换弹"; 
}
if (GetEnemyState >= 120 && GetEnemyState <= 170 || GetEnemyState >= 630 && GetEnemyState <= 680) {
 StateName = "射击"; 
}
if (GetEnemyState >= 500 && GetEnemyState <= 550 || GetEnemyState >= 1500 && GetEnemyState <= 1550) {
 StateName = "瞄准"; 
}
if (GetEnemyState >= 1150 && GetEnemyState <= 1170 || GetEnemyState >= 1670 && GetEnemyState <= 1700) {
 StateName = "射击"; 
}
if (GetEnemyState >= 1020 && GetEnemyState <= 1060 || GetEnemyState >= 1530 && GetEnemyState <= 1560) {
 StateName = "探头"; 
}
if (GetEnemyState >= 2040 && GetEnemyState <= 2090 || GetEnemyState >= 3080 && GetEnemyState <= 3090) {
 StateName = "切枪"; 
}
if (GetEnemyState >= 8200 && GetEnemyState <= 8270) {
 StateName = "挥拳"; 
}
if (GetEnemyState == 131072 || GetEnemyState == 131073) {
 StateName = "当场暴毙"; //倒地
}
if (GetEnemyState == 262144) {
 StateName = "当场暴毙"; //淘汰 
}
if (GetEnemyState == 524296) {
 StateName = "射击"; 
}
if (GetEnemyState >= 65540 && GetEnemyState <= 65580) {
 StateName = "治疗中"; 
}
if (GetEnemyState >= 16390 && GetEnemyState <= 16420 || GetEnemyState >= 17410 && GetEnemyState <= 17430) {
 StateName = "投掷"; 
}
if (GetEnemyState >= 3140000 && GetEnemyState <= 3170000) {
 StateName = "探头"; 
}
if (GetEnemyState >= 3146240 && GetEnemyState <= 3146250) {
 StateName = "瞄准敌人"; 
}
if (GetEnemyState >= 4194300 && GetEnemyState <= 4194310) {
 StateName = "游泳"; 
}
if (GetEnemyState == 524288) {
 StateName = "驾驶船"; 
}
if (GetEnemyState == 524296) {
 StateName = "驾驶车辆"; 
}
if (GetEnemyState == 1048584) {
 StateName = "乘坐车辆"; 
}
if (GetEnemyState == 1050632) {
 StateName = "收枪"; 
}
if (GetEnemyState == 1048576 || GetEnemyState == 1050624) {
 StateName = "乘坐船"; 
}
if (GetEnemyState >= 16000000 && GetEnemyState <= 17000000) {
 StateName = "攀爬"; 
}
if (GetEnemyState == 268435464) {
 StateName = "鼓掌"; 
}
if (GetEnemyState >= -2147483640 && GetEnemyState <= -2000000000) {
 StateName = "拍打坤坤"; 
}
return StateName;
}
