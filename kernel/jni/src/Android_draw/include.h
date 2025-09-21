#include "内核读写/函数.h"
#include <touch.h>
int Aimbotposition1;
int Aimbotposition2;
int aimCount = 0;
static float Ios[50];
string SaveDir;
FILE *numSave = nullptr;
float inttouchx = 300;
float inttouchy = 500;

typedef unsigned long S;

ImColor TouchingColor = ImColor(255, 0, 0, 150);

float DropM;
char PlayerName[100]; 
int 玩家数量 = 0;
int 人机数量 = 0;
/* 自瞄定义开始 */
float zm_x,zm_y;
void log(int num);
int AimObjCount = 0;
int WorldDistance;
int AimCount = 0;
float getScopeAcc(int Scope);
float diff[3];

int MaxPlayerCount = 0;
/* 人物数量 */
int ToReticleDistance;
/* 到准星距离 */
int Gmin=-1;
#define TIME 0.1

Vector2A AimHead;
Vector2A AimChest;
Vector2A AimPelvis;

Vector2A Head; Vector2A Chest; Vector2A Pelvis; Vector2A Left_Shoulder; Vector2A Right_Shoulder; Vector2A Left_Elbow; Vector2A Right_Elbow;  Vector2A Left_Wrist; Vector2A Right_Wrist; Vector2A Left_Thigh; Vector2A Right_Thigh;Vector2A Left_Knee;Vector2A Right_Knee;Vector2A Left_Ankle;Vector2A Right_Ankle;
ImColor whiteColor = ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 1.0f));

ImColor RandomColor() {
int R, G, B, A = 140;
R = (random() % 255);
G = (random() % 255);
B = (random() % 255);
return ImColor(R, G, B, A);
}

ImColor ColorArr[100];
void ColorInitialization() {
for(int i = 0; i < 100; i++) {
ColorArr[i] = RandomColor();
}
}

float GetWeaponId(int WeaponId) {
switch (WeaponId){
//突击步枪
case 101008:
return 1.2; // M762
break;
case 101001:
return 1.1; // AKM
break;
case 101004:
return 0.8; // M416
break;
case 101003:
return 0.8; // SCAR-L
break;
case 101002:
return 0.85; // M16A4
break;
case 101009:
return 1.05; // Mk47
break;
case 101006:
return 0.75; // AUG
break;
case 101005:
return 1.05; // Groza
break;
case 101010:
return 1.15; // G36C
break;
case 101007:
return 0.95; // QBZ
break;
case 101011:
return 1.0; // AC-VAL
break;
case 101012:
return 0.90; // 蜜獾突击步枪
break;
//机关枪
case 105001:
return 1.3; // M249
break;
case 105002:
return 1.3; // DP-28
case 105010:
return 1.05; // MG3
break;
//冲锋枪
case 102001:
return 0.6; // UZI
break;
case 102003:
return 0.6; // Vector
break;
case 100103:
return 0.65; // PP-19
break;
case 102007:
return 0.6; // MP5K
break;
case 102002:
return 0.6; // UMP 5
break;
case 102004:
return 0.6; // 汤姆逊
break;
case 102105:
return 0.5; // P90
break;
case 102005:
return 0.6; // 野牛
break;
default:
return 0.95; // 未收录
break;
}
return 1.0; // 未获取
}

float getScopeAcc(int Scope){
switch (Scope) {
case 0:
return 1.0f;
break;
case 1:
return .98f;
break;
case 2:
return .95f;
break;
case 3:
return .94f;
break;
case 4:
return .92f;
break;
case 5:
return .9f;
break;
case 6:
return .88f;
break;
case 7:
return .86f;
break;
case 8:
return .85f;
break;
default:
return .9f;
break;
}
}

const char *GetVehicleName(uint8_t type) {
switch (type) {
case 1:
return "Motorcycle";
case 2:
return "Sidecar Motorcycle";
case 3:
return "Dacia";
case 4:
return "Mini Bus";
case 5:
return "Pickup (Open Top)";
case 6:
return "Pickup (Closed Top)";
case 7:
return "Buggy";
case 8:
return "UAZ";
case 9:
return "UAZ (Closed Top)";
case 10:
return "UAZ (Open Top)";
case 11:
return "PG-117";
case 12:
return "Jet Ski";
case 14:
return "Mirado (Closed Top)";
case 15:
return "Mirado (Open Top)";
case 16:
return "Rony";
case 17:
return "Scooter";
case 18:
return "Snowmobile";
case 19:
return "Tukshai";
case 20:
return "Snowbike";
case 23:
return "Helicopter UH-60";
case 33:
return "Mirado (Gold)";
case 35:
return "Armed Dacia";
case 36:
return "Armed Pickup";
case 37:
return "Armed Buggy";
case 38:
return "Armed UAZ";
case 39:
return "Armed Helicopter UH-60";
case 60:
return "Motor Glider";
case 24:
return "BRDM";
case 53:
return "Monster Truck";
case 61:
return "Coupe RB";
case 51:
return "Zima UAZ";
case 65:
return "Mountain Bike";
}
return "Vehicle";
}
