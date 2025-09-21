#ifndef STRUCT_H
#define STRUCT_H
#include <Function.h>
#include <sys/types.h>
#include <cstdint>

struct Data {
pid_t pid = -1;
int Count,PlayerCount,BotCount,TotalUp;
uintptr_t libUE4;
uintptr_t Gname,Matrixs,Uworld,Uleve,Arrayaddr,Oneself;
uintptr_t Objaddr,Object,Mesh,Human,Bone;
float matrix[16];
float Fov;
int IsFiring;
int IsAiming;
int MyWeapon;
int MyTeam;
int MyPartyID;
uint32_t MyEntityMask;
int PartyID;
uint32_t EntityMask;
float w;
Vec3 Pos;
Vec3 MyPos;
Vec2 ScreenPos;
Vec3 Predict;
float camera;
int 敌方动作;
int TeamID;
int scwq;
int weapon;
int dqzd;
int zdmax;
int drt;
int drj;
int ActorState;
int drb;
bool IsBot;
float Health;
bool IsAlive;
float Distance;
char PlayerName[32];
float angle;
BoneStruct Head;   
BoneStruct Chest;
BoneStruct Pelvis;
BoneStruct Left_Shoulder;
BoneStruct Right_Shoulder;
BoneStruct Left_Elbow;
BoneStruct Right_Elbow;
BoneStruct Left_Wrist;
BoneStruct Right_Wrist;
BoneStruct Left_Thigh;
BoneStruct Right_Thigh;
BoneStruct Left_Knee;
BoneStruct Right_Knee;
BoneStruct Left_Ankle;
BoneStruct Right_Ankle;
	};
struct WuziData{
int Objaddr,Object;
int Count = 0;
float w;
Vec3 Pos;
Vec2 ScreenPos;
float camera;
float Distance;
char Name[128];
};

extern Data fanhua;
extern WuziData wz;

  
#endif
