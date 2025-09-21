#ifndef VECTOOL_H
#define VECTOOL_H

struct Vec2 {
    float x;
    float y;
    Vec2() {
        this->x = 0;
        this->y = 0;
    }
    Vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }
    Vec2 operator+(float v) const {
        return Vec2(x + v, y + v);
    }
    Vec2 operator-(float v) const {
        return Vec2(x - v, y - v);
    }
    Vec2 operator*(float v) const {
        return Vec2(x * v, y * v);
    }
    Vec2 operator/(float v) const {
        return Vec2(x / v, y / v);
    }
    Vec2& operator+=(float v) {
        x += v; y += v; return *this;
    }
    Vec2& operator-=(float v) {
        x -= v; y -= v; return *this;
    }
    Vec2& operator*=(float v) {
        x *= v; y *= v; return *this;
    }
    Vec2& operator/=(float v) {
        x /= v; y /= v; return *this;
    }
    Vec2 operator+(const Vec2& v) const {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator-(const Vec2& v) const {
        return Vec2(x - v.x, y - v.y);
    }
    Vec2 operator*(const Vec2& v) const {
        return Vec2(x * v.x, y * v.y);
    }
    Vec2 operator/(const Vec2& v) const {
        return Vec2(x / v.x, y / v.y);
    }
    Vec2& operator+=(const Vec2& v) {
        x += v.x; y += v.y; return *this;
    }
    Vec2& operator-=(const Vec2& v) {
        x -= v.x; y -= v.y; return *this;
    }
    Vec2& operator*=(const Vec2& v) {
        x *= v.x; y *= v.y; return *this;
    }
    Vec2& operator/=(const Vec2& v) {
        x /= v.x; y /= v.y; return *this;
    }
};

struct Vec3 {
    float x;
    float y;
    float z;
    Vec3() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    Vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    Vec3 operator+(float v) const {
        return Vec3(x + v, y + v, z + v);
    }
    Vec3 operator-(float v) const {
        return Vec3(x - v, y - v, z - v);
    }
    Vec3 operator*(float v) const {
        return Vec3(x * v, y * v, z * v);
    }
    Vec3 operator/(float v) const {
        return Vec3(x / v, y / v, z / v);
    }
    Vec3& operator+=(float v) {
        x += v; y += v; z += v; return *this;
    }
    Vec3& operator-=(float v) {
        x -= v; y -= v; z -= v; return *this;
    }
    Vec3& operator*=(float v) {
        x *= v; y *= v; z *= v; return *this;
    }
    Vec3& operator/=(float v) {
        x /= v; y /= v; z /= v; return *this;
    }
    Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    Vec3 operator*(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    Vec3 operator/(const Vec3& v) const {
        return Vec3(x / v.x, y / v.y, z / v.z);
    }
    Vec3& operator+=(const Vec3& v) {
        x += v.x; y += v.y; z += v.z; return *this;
    }
    Vec3& operator-=(const Vec3& v) {
        x -= v.x; y -= v.y; z -= v.z; return *this;
    }
    Vec3& operator*=(const Vec3& v) {
        x *= v.x; y *= v.y; z *= v.z; return *this;
    }
    Vec3& operator/=(const Vec3& v) {
        x /= v.x; y /= v.y; z /= v.z; return *this;
    }
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Vec4 {
    float x;
    float y;
    float z;
    float w;
    Vec4() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 0;
    }
    Vec4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
};

struct FMatrix
{
    float M[4][4];
};

struct Quat
{
    float x;
    float y;
    float z;
    float w;
};

struct FTransform
{
    Quat Rotation;
    Vec3 Translation;
    float chunk;
    Vec3 Scale3D;
};

struct BoneStruct {    
    Vec3 Pos; // 世界坐标 
	Vec2 ScreenPos; // 屏幕坐标
    bool CanSee; // 可见判断
};

struct TempData{
	int Width = 0; // 屏幕宽
    int Height = 0; // 屏幕高
    
	float Fov; // 自身fov
	float matrix[16]; // 游戏矩阵
    int IsFiring; // 开火判断
    int IsAiming; // 开镜判断
	int MyWeapon; // 自身手持
	float angle;
	Vec3 MyPos;
	
    struct PlayerArray{
    	int Count = 0;    
		struct Player{
        	float w; // 人物宽度
        	Vec3 Pos; // 世界坐标
            Vec2 ScreenPos; // 屏幕坐标
        	float camera; // 人物相机
            int TeamID; // 队标
            int Dzid; //动作
            int scwq; //手持
            int dqzd;//子弹
	        int zdmax;//最大子弹
	        int drb; //敌人包
	        int drt; //敌人头
	        int drj; //敌人甲
            bool IsBot; // 人机
            float Health; // 血量百分比
            float Distance; // 距离
            char PlayerName[32]; // 玩家名称
            Vec3 Predict; // 移动向量
        	
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
		} mPlayer[100];
	} mPlayerArray;
	
	struct wuziArray{
    	int Count = 0;    
		struct wuzi{
        	float w; // 人物宽度
        	Vec3 Pos; // 世界坐标
            Vec2 ScreenPos; // 屏幕坐标
        	float camera; // 人物相机
            float Distance; // 距离
            char wuziName[128]; // 玩家名称
		} mwuzi[100000];
	} mwuziArray;
};
#endif
