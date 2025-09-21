// ===============================
//         
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
// ===============================
//   ESP/  
//     :
//   -  /
//   -   (/)
//   -   
//   -   /
//   -    
//   -  //
//
//  :
//   -      (   
//       ).
//   - /    ,   .
//   -  : ,   ; ,     1.
//   -         .
//
//    ():
//    DrawIo[1]   
//    DrawIo[2]    (Ray)
//    DrawIo[3]    ( )
//    DrawIo[5]    (    0)
//    DrawIo[6]    HP/  
//    DrawIo[7]   
//    DrawIo[10]   
//    DrawIo[14]  
//    DrawIo[17]    ( )
//    DrawIo[31]     
//    DrawIo[32]   
//    DrawIo[40]  FPS-
//    DrawIo[45]    FOV
//    DrawIo[46]   ESP  
//    DrawIo[49]    ()
//
//    NumIo[3]     FOV ()
//    NumIo[8]      (1..15)
//    NumIo[16]    
//    NumIo[21]      (0   , 1  )
//
// ===============================
// Developer item mode integration (externs from main.cpp)
extern bool g_items_dev_mode;
extern char g_dev_watch_name[128];
extern float g_dev_watch_best;
extern bool g_items_draw_overrides;
bool FindItemOverrideC(const char* name, ImU32* out_col, char* out_label, int label_cap);

//   
AimCount = 0;
fanhua.PlayerCount = 0;
fanhua.BotCount = 0;
constexpr float kMaxDrawDistance = 434.0f; //    ESP
float cornerRadius = 7.0f;
bool radarArrowDrawn = false;
//   UE (     )
fanhua.Gname = driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xe47e8f0) + 0x110);
fanhua.Uworld = driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xea54a70) + 0x30);
fanhua.Uleve = driver->read<uintptr_t>(fanhua.Uworld + 0x20);
fanhua.Arrayaddr = driver->read<uintptr_t>(fanhua.Uleve + 0xA0);
fanhua.Count = driver->read<int>(fanhua.Uleve + 0xA8);
fanhua.Oneself = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xea54a70) + 0x30) + 0x450);
static const uintptr_t kTeamOffsets[] = {0x938, 0x8F0};
static const uintptr_t kTeamVehicleOffsets[] = {0xAC0, 0xA08};
static const uintptr_t kPartyOffsets[] = {0x9C0, 0x9B8, 0x940};
static const uintptr_t kEntityMaskOffsets[] = {0x9F8, 0x9FC, 0xA00};

auto readIdWithFallback = [&](uintptr_t base, const uintptr_t* offsets, int count) -> int {
    for (int idx = 0; idx < count; ++idx) {
        int value = driver->read<int>(base + offsets[idx]);
        if (value > 0 && value < 4096) {
            return value;
        }
    }
    return 0;
};

auto resolveTeamId = [&](uintptr_t actor) -> int {
    int team = readIdWithFallback(actor, kTeamOffsets, sizeof(kTeamOffsets) / sizeof(kTeamOffsets[0]));
    if (team <= 0) {
        team = readIdWithFallback(actor, kTeamVehicleOffsets, sizeof(kTeamVehicleOffsets) / sizeof(kTeamVehicleOffsets[0]));
    }
    return team;
};

auto resolvePartyId = [&](uintptr_t actor) -> int {
    return readIdWithFallback(actor, kPartyOffsets, sizeof(kPartyOffsets) / sizeof(kPartyOffsets[0]));
};

auto resolveEntityMask = [&](uintptr_t actor) -> uint32_t {
    for (int idx = 0; idx < static_cast<int>(sizeof(kEntityMaskOffsets) / sizeof(kEntityMaskOffsets[0])); ++idx) {
        uint32_t mask = driver->read<uint32_t>(actor + kEntityMaskOffsets[idx]);
        if (mask != 0 && mask != 0xFFFFFFFFu) {
            return mask;
        }
    }
    return 0;
};

static char g_localName[32] = {0};

fanhua.MyTeam = resolveTeamId(fanhua.Oneself);
fanhua.MyPartyID = resolvePartyId(fanhua.Oneself);
fanhua.MyEntityMask = resolveEntityMask(fanhua.Oneself);
getUTF8(g_localName, driver->read<uintptr_t>(fanhua.Oneself + 0x900));
driver->read(driver->read<uintptr_t>(fanhua.Oneself + 0x1b0) + 0x1c0, &fanhua.MyPos, 12);
driver->read(fanhua.Matrixs, &fanhua.matrix, 16 * 4);
const float radarScale = ImMax(NumIo[16], 1.0f);
const float radarRange = radarScale * 2.0f;
const ImVec2 radarCenter(NumIo[1], NumIo[2]);
const float radarBound = ImClamp(NumIo[19], 40.0f, 400.0f);
float radarForwardLen = sqrtf(fanhua.matrix[3] * fanhua.matrix[3] + fanhua.matrix[7] * fanhua.matrix[7]);
float radarCosForward = 1.0f;
float radarSinForward = 0.0f;
if (radarForwardLen > 0.0001f) {
    radarCosForward = fanhua.matrix[3] / radarForwardLen;
    radarSinForward = fanhua.matrix[7] / radarForwardLen;
}
auto ToRadar = [&](float scaledRight, float scaledForward) {
    float pxRight = ImClamp(scaledRight * radarBound, -radarBound, radarBound);
    float pxForward = ImClamp(scaledForward * radarBound, -radarBound, radarBound);
    return ImVec2(radarCenter.x + pxRight, radarCenter.y - pxForward);
};
auto DrawRadarArrow = [&]() {
    if (radarArrowDrawn) return;
    const float arrowTip = 0.82f;
    const float arrowTail = -0.45f;
    const float arrowWidth = 0.28f;
    ImVec2 tip   = ToRadar(0.0f, arrowTip);
    ImVec2 left  = ToRadar(-arrowWidth, arrowTail);
    ImVec2 right = ToRadar(arrowWidth, arrowTail);
    ImDrawList* radarDraw = ImGui::GetForegroundDrawList();
    radarDraw->AddTriangleFilled(left, tip, right, ImColor(60, 170, 255, 220));
    radarDraw->AddTriangle(left, tip, right, ImColor(10, 25, 50, 200), 1.5f);
    radarArrowDrawn = true;
};
fanhua.Fov = driver->read<float>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Oneself + 0x49e0) + 0x4f0) + 0x4e0);
fanhua.IsFiring = driver->read<int>(fanhua.Oneself + 0x1708);
fanhua.IsAiming = driver->read<int>(fanhua.Oneself + 0x1071);
fanhua.angle = driver->read<float>(fanhua.Oneself + 0x1a8);
char *CasName;
for (int i = 0; i < fanhua.Count; i ++) {
fanhua.Objaddr = driver->read<uintptr_t>(fanhua.Arrayaddr + 8 * i);
if (fanhua.Oneself == fanhua.Objaddr || fanhua.Objaddr <= 0x10000000 || fanhua.Objaddr % 4 != 0 || fanhua.Objaddr >= 0x10000000000)
continue;
uintptr_t object = driver->read<uintptr_t>(fanhua.Objaddr + 0x1b0);
if (object <= 0xffff || object == 0 || object <= 0x10000000 || object % 4 != 0 || object >= 0x10000000000)
continue;
if (driver->read<float>(fanhua.Objaddr + 0x2a68) != 479.5) {
//GetClassName(wz.Name, fanhua.Gname,driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xD66B2C0) + 0x110),driver->read<int>(fanhua.Objaddr + 0x18));


GetClassName(wz.Name, fanhua.Gname, driver->read<int>(fanhua.Objaddr + 0x18));



driver->read(object + 0x1c0, &wz.Pos, 12);
GetDistance(wz.Pos, fanhua.MyPos, &wz.Distance);
WorldToScreen(&wz.ScreenPos, &wz.camera, &wz.w, wz.Pos);
// Developer: draw raw name and track closest
if (g_items_dev_mode) {
    // Preferred draw: override color+label from file if present
    ImU32 ocol = 0; char olabel[128] = {0};
    bool hasOverride = FindItemOverrideC(wz.Name, &ocol, olabel, 128);
    if (hasOverride) {
        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {wz.ScreenPos.x - 10.0f, wz.ScreenPos.y}, ocol, olabel);
    } else {
        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {wz.ScreenPos.x - 10.0f, wz.ScreenPos.y}, ImColor(255,255,255,255), wz.Name);
    }
    float dx = wz.ScreenPos.x - (screen_x * 0.5f);
    float dy = wz.ScreenPos.y - (screen_y * 0.5f);
    float dd = dx*dx + dy*dy;
    if (wz.camera > 0.0f && dd < g_dev_watch_best) {
        g_dev_watch_best = dd;
        strncpy(g_dev_watch_name, wz.Name, sizeof(g_dev_watch_name)-1);
        g_dev_watch_name[sizeof(g_dev_watch_name)-1] = '\0';
    }
}
} else {
int State = driver->read<int>(fanhua.Objaddr + 0xfa0);
if (State == 262144 || State == 262152 || State == 262145 || State == 0)
continue;
// Robust team filtering: try multiple offsets (vehicle vs on-foot)
int team_main = driver->read<int>(fanhua.Objaddr + 0x938);
int team_alt  = driver->read<int>(fanhua.Objaddr + 0xac0);
fanhua.TeamID = (team_main > 0) ? team_main : team_alt;
if (fanhua.TeamID == fanhua.MyTeam || fanhua.TeamID < 1)
    continue;
}


fanhua.Health = (driver->read<float>(fanhua.Objaddr + 0xDB8) / driver->read<float>(fanhua.Objaddr + 0xDBC)) * 100;
if (fanhua.Health > 100)
continue;
// //
driver->read(object + 0x1c0, &fanhua.Pos, 12);
GetDistance(fanhua.Pos, fanhua.MyPos, &fanhua.Distance);
WorldToScreen(&fanhua.ScreenPos, &fanhua.camera, &fanhua.w, fanhua.Pos);
//  :     ,
//       .
if (fanhua.Distance > kMaxDrawDistance) {
    continue;
}
//  /   ,   
int isBotFlag = driver->read<int>(fanhua.Objaddr +  0xab4) ? 1 : 0;
long int renji = driver->read<int>(fanhua.Objaddr + 0x9e9);
int renjiBot = (renji == 16842753 || renji == 16843009 || renji == 16843008) ? 1 : 0;
fanhua.IsBot = (isBotFlag || renjiBot) ? 1 : 0;
if (fanhua.IsBot) fanhua.BotCount++; else fanhua.PlayerCount++;

if(DrawIo[17])
{
if (fanhua.IsBot == 1)
{
 continue;
}
}
if (driver->read<uintptr_t>(fanhua.Objaddr + 0x1c0)) {
driver->read(driver->read<uintptr_t>(fanhua.Objaddr + 0xd98) + 0x1C0,  &fanhua.Predict, sizeof(fanhua.Predict)); // 
} else {
driver->read(object + 0x260, &fanhua.Predict, sizeof(fanhua.Predict)); 
}
long int rw = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x348) + 0x30) + 0x388);
fanhua.drb = driver->read<int>(rw + 0x1C4);
fanhua.drt = driver->read<int>(rw + 0x1FC);
fanhua.drj = driver->read<int>(rw + 0x234);
fanhua.ActorState = driver->read<int>(driver->read<uint64_t>(fanhua.Objaddr + 0xfa0));
fanhua.dqzd = driver->read<int>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x2528)+ 0x558)+0xf98);
fanhua.zdmax = driver->read<int>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x2528)+ 0x558)+0xfb0);
// Read current weapon id (scwq) with fallback chains for different versions
fanhua.scwq = driver->read<int>(driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x27a8) + 0x558) + 0x11b8) + 0x178);
if (fanhua.scwq == 0) {
    // Legacy chain fallback
    int alt = driver->read<int>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x3190) + 0x678) + 0xbd0);
    if (alt != 0) fanhua.scwq = alt;
}
fanhua.Mesh = driver->read<uintptr_t>(fanhua.Objaddr + 0x4a8);
fanhua.Human = fanhua.Mesh + 0x1B0;
fanhua.Bone = driver->read<uintptr_t>(fanhua.Mesh + 0x8a8)+0x30;
int BoneCount = driver->read<uint64_t>(fanhua.Mesh + 0x7f8+8);
getUTF8(fanhua.PlayerName, driver->read<uintptr_t>(fanhua.Objaddr + 0x900));
int p = (BoneCount == 68) ? 33 : 32;
int o = (BoneCount == 68) ? 34 : 33;
int a = (BoneCount == 68) ? 13 : 63;
int b = (BoneCount == 68) ? 35 : 62;
int c = (BoneCount == 68) ? 55 : 52;
int d = (BoneCount == 68) ? 59 : 56;
int e = (BoneCount == 68) ? 56 : 53;
int f = (BoneCount == 68) ? 60 : 57;
int g = (BoneCount == 68) ? 57 : 54;
int h = (BoneCount == 68) ? 61 : 58;
FTransform meshtrans = getBone(fanhua.Human);
FMatrix c2wMatrix = TransformToMatrix(meshtrans);
//     : , , 
{
    FTransform headtrans = getBone(fanhua.Bone + 5 * 48);
    FMatrix boneMatrix = TransformToMatrix(headtrans);
    fanhua.Head.Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
    fanhua.Head.Pos.z += 7; 
    fanhua.Head.ScreenPos = WorldToScreen(fanhua.Head.Pos, fanhua.matrix, fanhua.camera);
    FTransform chesttrans = getBone(fanhua.Bone + 4 * 48);
    FMatrix boneMatrix1 = TransformToMatrix(chesttrans);
    fanhua.Chest.Pos = MarixToVector(MatrixMulti(boneMatrix1, c2wMatrix));
    fanhua.Chest.ScreenPos = WorldToScreen(fanhua.Chest.Pos, fanhua.matrix, fanhua.camera);
    FTransform pelvistrans = getBone(fanhua.Bone + 0 * 48);
    FMatrix boneMatrix2 = TransformToMatrix(pelvistrans);
    fanhua.Pelvis.Pos = MarixToVector(MatrixMulti(boneMatrix2, c2wMatrix));
    fanhua.Pelvis.ScreenPos = WorldToScreen(fanhua.Pelvis.Pos, fanhua.matrix, fanhua.camera);
}
if (!fanhua.IsBot) {
    //      
    FTransform lshtrans = getBone(fanhua.Bone + 11 * 48);
    FMatrix boneMatrix3 = TransformToMatrix(lshtrans);
    fanhua.Left_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix3, c2wMatrix));
    fanhua.Left_Shoulder.ScreenPos = WorldToScreen(fanhua.Left_Shoulder.Pos, fanhua.matrix, fanhua.camera);
    FTransform rshtrans = getBone(fanhua.Bone + p * 48);
    FMatrix boneMatrix4 = TransformToMatrix(rshtrans);
    fanhua.Right_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix4, c2wMatrix));
    fanhua.Right_Shoulder.ScreenPos = WorldToScreen(fanhua.Right_Shoulder.Pos, fanhua.matrix, fanhua.camera);
    FTransform lelbtrans = getBone(fanhua.Bone + 12 * 48);
    FMatrix boneMatrix5 = TransformToMatrix(lelbtrans);
    fanhua.Left_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix5, c2wMatrix));
    fanhua.Left_Elbow.ScreenPos = WorldToScreen(fanhua.Left_Elbow.Pos, fanhua.matrix, fanhua.camera);
    FTransform relbtrans = getBone(fanhua.Bone + o * 48);
    FMatrix boneMatrix6 = TransformToMatrix(relbtrans);
    fanhua.Right_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix6, c2wMatrix));
    fanhua.Right_Elbow.ScreenPos = WorldToScreen(fanhua.Right_Elbow.Pos, fanhua.matrix, fanhua.camera);
    FTransform lwtrans = getBone(fanhua.Bone + a * 48);
    FMatrix boneMatrix7 = TransformToMatrix(lwtrans);
    fanhua.Left_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix7, c2wMatrix));
    fanhua.Left_Wrist.ScreenPos = WorldToScreen(fanhua.Left_Wrist.Pos, fanhua.matrix, fanhua.camera);
    FTransform rwtrans = getBone(fanhua.Bone + b * 48);
    FMatrix boneMatrix8 = TransformToMatrix(rwtrans);
    fanhua.Right_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix8, c2wMatrix));
    fanhua.Right_Wrist.ScreenPos = WorldToScreen(fanhua.Right_Wrist.Pos, fanhua.matrix, fanhua.camera);
    FTransform Llshtrans = getBone(fanhua.Bone + c * 48);
    FMatrix boneMatrix9 = TransformToMatrix(Llshtrans);
    fanhua.Left_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix9, c2wMatrix));
    fanhua.Left_Thigh.ScreenPos = WorldToScreen(fanhua.Left_Thigh.Pos, fanhua.matrix, fanhua.camera);
    FTransform Lrshtrans = getBone(fanhua.Bone + d * 48);
    FMatrix boneMatrix10 = TransformToMatrix(Lrshtrans);
    fanhua.Right_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix10, c2wMatrix));
    fanhua.Right_Thigh.ScreenPos = WorldToScreen(fanhua.Right_Thigh.Pos, fanhua.matrix, fanhua.camera);
    FTransform Llelbtrans = getBone(fanhua.Bone + e * 48);
    FMatrix boneMatrix11 = TransformToMatrix(Llelbtrans);
    fanhua.Left_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix11, c2wMatrix));
    fanhua.Left_Knee.ScreenPos = WorldToScreen(fanhua.Left_Knee.Pos, fanhua.matrix, fanhua.camera);
    FTransform Lrelbtrans = getBone(fanhua.Bone + f * 48);
    FMatrix boneMatrix12 = TransformToMatrix(Lrelbtrans);
    fanhua.Right_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix12, c2wMatrix));
    fanhua.Right_Knee.ScreenPos = WorldToScreen(fanhua.Right_Knee.Pos, fanhua.matrix, fanhua.camera);
    FTransform Llwtrans = getBone(fanhua.Bone + g * 48);
    FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
    fanhua.Left_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
    fanhua.Left_Ankle.ScreenPos = WorldToScreen(fanhua.Left_Ankle.Pos, fanhua.matrix, fanhua.camera);
    FTransform Lrwtrans = getBone(fanhua.Bone + h * 48);
    FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
    fanhua.Right_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
    fanhua.Right_Ankle.ScreenPos = WorldToScreen(fanhua.Right_Ankle.Pos, fanhua.matrix, fanhua.camera);
} else {
    //      (  )
    FTransform Llwtrans = getBone(fanhua.Bone + g * 48);
    FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
    fanhua.Left_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
    fanhua.Left_Ankle.ScreenPos = WorldToScreen(fanhua.Left_Ankle.Pos, fanhua.matrix, fanhua.camera);
    FTransform Lrwtrans = getBone(fanhua.Bone + h * 48);
    FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
    fanhua.Right_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
    fanhua.Right_Ankle.ScreenPos = WorldToScreen(fanhua.Right_Ankle.Pos, fanhua.matrix, fanhua.camera);
}
if (DrawIo[20] && !fanhua.IsBot && fanhua.IsAlive) {
strcpy(Aim[AimCount].Name, fanhua.PlayerName);
Aim[AimCount].WodDistance = fanhua.Distance;
Aim[AimCount].AimMovement = fanhua.Predict;
int targetIdx = (int)NumIo[8];
auto setTarget = [&](const Vec3& pos, const Vec2& sp){
    Aim[AimCount].ObjAim = pos;
    Aim[AimCount].ScreenDistance = sqrt(pow(screen_x / 2 - sp.x, 2) + pow(screen_y / 2 - sp.y, 2));
};
switch (targetIdx) {
    default:
    case 1: setTarget(fanhua.Head.Pos, fanhua.Head.ScreenPos); break;
    case 2: setTarget(fanhua.Chest.Pos, fanhua.Chest.ScreenPos); break;
    case 3: setTarget(fanhua.Pelvis.Pos, fanhua.Pelvis.ScreenPos); break;
    case 4: setTarget(fanhua.Left_Shoulder.Pos, fanhua.Left_Shoulder.ScreenPos); break;
    case 5: setTarget(fanhua.Right_Shoulder.Pos, fanhua.Right_Shoulder.ScreenPos); break;
    case 6: setTarget(fanhua.Left_Elbow.Pos, fanhua.Left_Elbow.ScreenPos); break;
    case 7: setTarget(fanhua.Right_Elbow.Pos, fanhua.Right_Elbow.ScreenPos); break;
    case 8: setTarget(fanhua.Left_Wrist.Pos, fanhua.Left_Wrist.ScreenPos); break;
    case 9: setTarget(fanhua.Right_Wrist.Pos, fanhua.Right_Wrist.ScreenPos); break;
    case 10: setTarget(fanhua.Left_Thigh.Pos, fanhua.Left_Thigh.ScreenPos); break;
    case 11: setTarget(fanhua.Right_Thigh.Pos, fanhua.Right_Thigh.ScreenPos); break;
    case 12: setTarget(fanhua.Left_Knee.Pos, fanhua.Left_Knee.ScreenPos); break;
    case 13: setTarget(fanhua.Right_Knee.Pos, fanhua.Right_Knee.ScreenPos); break;
    case 14: setTarget(fanhua.Left_Ankle.Pos, fanhua.Left_Ankle.ScreenPos); break;
    case 15: setTarget(fanhua.Right_Ankle.Pos, fanhua.Right_Ankle.ScreenPos); break;
}
AimCount++;
}
    if (DrawIo[49] && Gmin != -1)
    {
        // Prediction point: aquamarine and 3x smaller
        const float r = 3.0f; // was 8
        ImU32 aqua = ImColor(127, 255, 212, 255);
        Draw->AddCircleFilled({vpvp.x, vpvp.y}, r, aqua);
        // subtle outline for readability
        Draw->AddCircle({vpvp.x, vpvp.y}, r, ImColor(0,0,0,180), 0, 1.0f);
    }
float top, right, left, bottom, top1; 
if (DrawIo[8]) {
    const bool isStyle0 = (draw_style[2] == 0);
    const bool isStyle1 = (draw_style[2] == 1);
    static char distanceStr[32];
    const int distance = static_cast<int>(fanhua.Distance);
    const int length = snprintf(distanceStr, sizeof(distanceStr), "%dM", distance);
    const ImVec2 textSize = ImGui::CalcTextSize(distanceStr);
    const ImColor enemyColor = fanhua.IsBot ? ImColor(0, 255, 0, 255) : ImColor(255, 0, 0, 255);
    ImVec2 headPos = {fanhua.Head.ScreenPos.x, fanhua.Head.ScreenPos.y};
    if (isStyle0 && fanhua.Health > 0) {
    const bool isLeft = (headPos.x < 0);
    const bool isRight = (headPos.x > screen_x);
    const bool isTop = (headPos.y < 0);
    const bool isBottom = (headPos.y > screen_y);
        if (isLeft || isRight || isTop || isBottom) {
            ImVec2 circlePos = {
                isLeft ? 0 : (isRight ? screen_x : headPos.x),
                isTop ? 0 : (isBottom ? screen_y : headPos.y)
            };
            ImVec2 textPos;
            if (isLeft) {
                textPos = {5, circlePos.y - 20};
            } else if (isRight) {
                textPos = {screen_x - textSize.x - 5, circlePos.y - 20};
            } else if (isTop) {
                textPos = {circlePos.x - textSize.x/2, 10};
            } else {
                textPos = {circlePos.x - textSize.x/2, screen_y - 30};
            }
            Draw->AddCircleFilled(circlePos, 70, enemyColor);
            Draw->AddText(NULL, 30, textPos, ImColor(255,255,255), distanceStr);
        }
    }   
    if (isStyle1) {
        const float tm = 120.f/255.f;
        const float cameras = fanhua.matrix[3] * fanhua.Pos.x + 
                             fanhua.matrix[7] * fanhua.Pos.y + 
                             fanhua.matrix[11] * fanhua.Pos.z + 
                             fanhua.matrix[15];
        if (!fanhua.IsBot) {
            OffScreen(fanhua.ScreenPos, cameras, ImColor(255, 0, 0, 255), 
                    NumIo[3] + 20 + fanhua.Distance * 0.3);
            ImVec2 textPos = {
                fanhua.ScreenPos.x - textSize.x / 2,
                fanhua.ScreenPos.y - 20
            };
            Draw->AddText(NULL, 12, textPos, ImColor(255, 255, 255), distanceStr);
        } else {
            OffScreen(fanhua.ScreenPos, cameras, ImColor(0, 255, 0, 255), 
                    NumIo[3] + 20 + fanhua.Distance * 0.3);
        }
    }
}
left  = fanhua.Head.ScreenPos.x - fanhua.w * 0.6;
right = fanhua.Head.ScreenPos.x + fanhua.w * 0.6;
if (!fanhua.Head.Pos.x) {
top1 = fanhua.Pelvis.ScreenPos.y - fanhua.Chest.ScreenPos.y;
} else {
top1 = fanhua.Pelvis.ScreenPos.y - fanhua.Head.ScreenPos.y;
}
top  = fanhua.Pelvis.ScreenPos.y - top1 - fanhua.w / 5;
if (fanhua.Left_Ankle.ScreenPos.y < fanhua.Right_Ankle.ScreenPos.y) {
bottom = fanhua.Right_Ankle.ScreenPos.y + fanhua.w / 10;
} else {
bottom = fanhua.Left_Ankle.ScreenPos.y  + fanhua.w / 10;
}
if (DrawIo[1]) {
    ImColor boxColor;
    if (fanhua.IsBot == 1) {
        boxColor = ImColor(0, 255, 0);
    } else if (fanhua.Health <= 0) {
        boxColor = ImColor(255, 0, 0);
    } else {
        boxColor = ImColor(173, 216, 230);
    }
    Draw->AddRect({left, top}, {right, bottom}, boxColor, 0.0f, 0, 2.0f);
}
if (DrawIo[2]) {
    static const float rayStartY = (draw_style[0] == 0 || draw_style[0] == 1) ? 73 : 
                                 (draw_style[0] == 2) ? screen_y*0.135f : 0;
    const ImVec2 endPos = {fanhua.Head.ScreenPos.x, fanhua.Head.ScreenPos.y - 70};
    const bool isOffScreen = (fanhua.Head.ScreenPos.x < 0 || fanhua.Head.ScreenPos.x > screen_x ||
                             fanhua.Head.ScreenPos.y < 0 || fanhua.Head.ScreenPos.y > screen_y ||
                             fanhua.camera <= 0.0f);
    ImColor rayColor = fanhua.IsBot ? ImColor(0, 255, 0) : ImColor(173, 216, 230);
    const float lineWidth = fanhua.IsBot ? 2.0f : 1.5f;
    if (!isOffScreen) {
        Draw->AddLine({screen_x / 2, rayStartY}, endPos, rayColor, lineWidth);
    }
}
//     ,    
if (DrawIo[3] && !fanhua.IsBot) {
    bool allBonesValid = true;
    const Vec2* bonePoints[] = {
        &fanhua.Chest.ScreenPos, &fanhua.Pelvis.ScreenPos,
        &fanhua.Left_Shoulder.ScreenPos, &fanhua.Right_Shoulder.ScreenPos,
        &fanhua.Left_Elbow.ScreenPos, &fanhua.Right_Elbow.ScreenPos,
        &fanhua.Left_Wrist.ScreenPos, &fanhua.Right_Wrist.ScreenPos,
        &fanhua.Left_Thigh.ScreenPos, &fanhua.Right_Thigh.ScreenPos,
        &fanhua.Left_Knee.ScreenPos, &fanhua.Right_Knee.ScreenPos,
        &fanhua.Left_Ankle.ScreenPos, &fanhua.Right_Ankle.ScreenPos
    };
    /* keep skeleton visible even if some bones are off-screen */
    {
        ImColor boneColor;
        if (fanhua.IsBot) {
            boneColor = ImColor(0, 255, 0);
        } else if (fanhua.Health <= 0) {
            boneColor = ImColor(255, 0, 0);
        } else {
            boneColor = ImColor(173, 216, 230);
        }
        float maxDist = sqrtf((float)screen_x*(float)screen_x + (float)screen_y*(float)screen_y) * 0.8f;
        auto valid = [&](const Vec2& p){
            if (isnan(p.x) || isnan(p.y)) return false;
            if (p.x <= -screen_x || p.x >= screen_x*2 || p.y <= -screen_y || p.y >= screen_y*2) return false;
            return true;
        };
        auto DrawBoneLine = [&](const Vec2& from, const Vec2& to) {
            if (!valid(from) || !valid(to)) return;
            float dx = from.x - to.x, dy = from.y - to.y;
            if (sqrtf(dx*dx + dy*dy) > maxDist) return; // skip stretched/misaligned bones
            Draw->AddLine({from.x, from.y}, {to.x, to.y}, boneColor, 1.5f);
        };
        DrawBoneLine(fanhua.Chest.ScreenPos, fanhua.Pelvis.ScreenPos);
        DrawBoneLine(fanhua.Chest.ScreenPos, fanhua.Left_Shoulder.ScreenPos);
        DrawBoneLine(fanhua.Chest.ScreenPos, fanhua.Right_Shoulder.ScreenPos);
        DrawBoneLine(fanhua.Left_Shoulder.ScreenPos, fanhua.Left_Elbow.ScreenPos);
        DrawBoneLine(fanhua.Right_Shoulder.ScreenPos, fanhua.Right_Elbow.ScreenPos);
        DrawBoneLine(fanhua.Left_Elbow.ScreenPos, fanhua.Left_Wrist.ScreenPos);
        DrawBoneLine(fanhua.Right_Elbow.ScreenPos, fanhua.Right_Wrist.ScreenPos);
        DrawBoneLine(fanhua.Pelvis.ScreenPos, fanhua.Left_Thigh.ScreenPos);
        DrawBoneLine(fanhua.Pelvis.ScreenPos, fanhua.Right_Thigh.ScreenPos);
        DrawBoneLine(fanhua.Left_Thigh.ScreenPos, fanhua.Left_Knee.ScreenPos);
        DrawBoneLine(fanhua.Right_Thigh.ScreenPos, fanhua.Right_Knee.ScreenPos);
        DrawBoneLine(fanhua.Left_Knee.ScreenPos, fanhua.Left_Ankle.ScreenPos);
        DrawBoneLine(fanhua.Right_Knee.ScreenPos, fanhua.Right_Ankle.ScreenPos);
    }
}
// Show standalone distance only for default style (0). Styles 1/2 have their own distance.
if (DrawIo[5]) {
    int styleIndex = static_cast<int>(draw_style[1]);
    if (styleIndex == 0) {
        if (fanhua.Pelvis.ScreenPos.x > 0 && fanhua.Pelvis.ScreenPos.y > 0 &&
            fanhua.Pelvis.ScreenPos.x < screen_x && fanhua.Pelvis.ScreenPos.y < screen_y) {
            std::string distanceText = std::to_string((int)fanhua.Distance) + " m";
            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(25, FLT_MAX, -1, distanceText.c_str());
            float xPos = std::max(0.0f, std::min(screen_x - textSize.x,
                                    fanhua.Pelvis.ScreenPos.x - textSize.x * 0.5f));
            float yPos = std::max(0.0f, std::min(screen_y - 25.0f,
                                    bottom + 15.0f));
            Draw->AddText(ImGui::GetFont(), 25, ImVec2(xPos, yPos), ImColor(255, 255, 255, 255), distanceText.c_str());
        }
    } else {
        if (fanhua.Pelvis.ScreenPos.x > 0 && fanhua.Pelvis.ScreenPos.y > 0 &&
            fanhua.Pelvis.ScreenPos.x < screen_x && fanhua.Pelvis.ScreenPos.y < screen_y) {
            std::string distanceText = std::to_string((int)fanhua.Distance) + " m";
            ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(22, FLT_MAX, -1, distanceText.c_str());
            float xPos = std::max(0.0f, std::min(screen_x - textSize.x,
                                    fanhua.Pelvis.ScreenPos.x - textSize.x * 0.5f));
            float yPos = std::max(0.0f, std::min(screen_y - 22.0f,
                                    bottom + 20.0f));
            Draw->AddText(ImGui::GetFont(), 22, ImVec2(xPos, yPos), ImColor(230, 230, 230, 255), distanceText.c_str());
        }
    }
}
if (DrawIo[6]) {
if (!fanhua.IsBot) {
DrawHealth({fanhua.Head.ScreenPos.x, top }, fanhua.w * 2, fanhua.Health, fanhua.TeamID, fanhua.PlayerName);
} else {
std::string botLabel = i18n::TL("Bot");
DrawHealth({fanhua.Head.ScreenPos.x, top }, fanhua.w * 2, fanhua.Health, fanhua.TeamID, const_cast<char*>(botLabel.c_str()));
}
}
float aa;
float bb;
ImVec2 radarCenter(1500.0f, 300.0f);
float rotationAngle = 0.0f;
Vec2 Radar = {(fanhua.MyPos.x - fanhua.Pos.x) / NumIo[16], (fanhua.MyPos.y - fanhua.Pos.y) / NumIo[16]};
if (DrawIo[7]) {
    if (fanhua.Distance < NumIo[16] * 2) {
        Vec2 radarLocal = {
            (fanhua.MyPos.x - fanhua.Pos.x) / NumIo[16],
            (fanhua.MyPos.y - fanhua.Pos.y) / NumIo[16]
        };
        float length = sqrt(radarLocal.x * radarLocal.x + radarLocal.y * radarLocal.y);
        if (length > 0.0001f) {
            float sina = -radarLocal.y / length;
            float cosa = -radarLocal.x / length;
            float xiebian = sqrt(fanhua.matrix[7] * fanhua.matrix[7] + fanhua.matrix[3] * fanhua.matrix[3]);
            if (xiebian > 0.0001f) {
                float sinb = fanhua.matrix[7] / xiebian;
                float cosb = fanhua.matrix[3] / xiebian;
                float sinab = sina * cosb - sinb * cosa;
                float cosab = cosa * cosb + sina * sinb;
                radarLocal.x = length * cosab;
                radarLocal.y = length * sinab;
                bool withinBounds =
                    NumIo[1] + radarLocal.x >= NumIo[1] - 100 && NumIo[1] + radarLocal.x <= NumIo[1] + 100 &&
                    NumIo[2] + radarLocal.y >= NumIo[2] - 100 && NumIo[2] + radarLocal.y <= NumIo[2] + 100;
                if (withinBounds) {
                    std::string displayText;
                    ImColor bgColor, textColor;
                    if (fanhua.IsBot) {
                        bgColor = ImColor(255, 255, 255);
                        displayText = std::to_string(static_cast<int>(fanhua.Distance));
                    } else {
                        bgColor = ImColor(0, 255, 0);
                        switch (static_cast<int>(draw_style[2])) {
                            case 0: displayText = std::to_string(static_cast<int>(fanhua.Distance)); break;
                            case 1: displayText = std::to_string(static_cast<int>(fanhua.Health)); break;
                            case 2: displayText = std::to_string(static_cast<int>(fanhua.TeamID)); break;
                            default: displayText = std::to_string(static_cast<int>(fanhua.Distance));
                        }
                    }
                    textColor = ImColor(0, 0, 0);
                    ImVec2 centerPos = ImVec2(NumIo[1] + radarLocal.x, NumIo[2] + radarLocal.y);
                    Draw->AddCircleFilled(centerPos, 20, bgColor);
                    if (!displayText.empty()) {
                        auto textSize = ImGui::CalcTextSize(displayText.c_str());
                        ImVec2 textPos = ImVec2(
                            centerPos.x - textSize.x / 3.4f,
                            centerPos.y - textSize.y / 3.5f
                        );
                        Draw->AddText(NULL, 21, textPos, textColor, displayText.c_str());
                    }
                }
            }
        }
    }

    DrawRadarArrow();
    if (fanhua.Distance < radarRange) {
        float dx = fanhua.Pos.x - fanhua.MyPos.x;
        float dy = fanhua.Pos.y - fanhua.MyPos.y;
        float localForward = dx * radarCosForward + dy * radarSinForward;
        float localRight = dx * (-radarSinForward) + dy * radarCosForward;
        float scaledForward = localForward / radarScale;
        float scaledRight = localRight / radarScale;
        ImVec2 centerPos = ToRadar(scaledRight, scaledForward);
        bool insideRadar =
            centerPos.x >= radarCenter.x - radarBound && centerPos.x <= radarCenter.x + radarBound &&
            centerPos.y >= radarCenter.y - radarBound && centerPos.y <= radarCenter.y + radarBound;
        if (insideRadar) {
            std::string displayText;
            ImColor bgColor, textColor;
            if (fanhua.IsBot) {
                bgColor = ImColor(255, 255, 255);
                displayText = std::to_string(static_cast<int>(fanhua.Distance));
            } else {
                bgColor = ImColor(0, 255, 0);
                switch (static_cast<int>(draw_style[2])) {
                    case 0: displayText = std::to_string(static_cast<int>(fanhua.Distance)); break;
                    case 1: displayText = std::to_string(static_cast<int>(fanhua.Health)); break;
                    case 2: displayText = std::to_string(static_cast<int>(fanhua.TeamID)); break;
                    default: displayText = std::to_string(static_cast<int>(fanhua.Distance));
                }
            }
            textColor = ImColor(0, 0, 0);
            ImDrawList* radarDraw = ImGui::GetForegroundDrawList();
            radarDraw->AddCircleFilled(centerPos, 20.0f, bgColor);
            if (!displayText.empty()) {
                auto textSize = ImGui::CalcTextSize(displayText.c_str());
                ImVec2 textPos = ImVec2(
                    centerPos.x - textSize.x * 0.5f,
                    centerPos.y - textSize.y * 0.5f
                );
                radarDraw->AddText(NULL, 21, textPos, textColor, displayText.c_str());
            }
        }
    }
if (wz.w > 0) {
    // Item drawing disabled due to pending ASCII renaming.
}
// Removed BOSS feature per request
    // Item/vehicle labeling removed pending ASCII refactor.
if (DrawIo[7] && !radarArrowDrawn) {
    DrawRadarArrow();
}
MaxPlayerCount = AimCount;
fanhua.TotalUp = fanhua.PlayerCount + fanhua.BotCount;
