// ===============================
//  Требуемые стандартные заголовки для математических операций и строк
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
// ===============================
//  Рендеринг ESP/сбор данных АИМа
//  Этот модуль отвечает за:
//   - чтение мира/объектов
//   - классификацию целей (бот/игрок)
//   - ограничение дистанции отрисовки
//   - учёт счётчиков игроков/ботов
//   - подготовку точек для аимбота
//   - отрисовку боксов/скелета/подписей
//
//  ВАЖНО:
//   - Скелет рисуем ТОЛЬКО для игроков (ботам достаточно минимальных костей
//     для корректного бокса).
//   - Боксы/подписи отображаются и у ботов, и у игроков.
//   - Счётчик сверху: зелёный, если нет игроков; красный, если есть хотя бы 1.
//   - Ограничение дальности отрисовки вынесено рано — до инкремента счётчиков.
//
//  Индексы переключателей (кратко):
//    DrawIo[1]  — Бокс
//    DrawIo[2]  — Луч (Ray)
//    DrawIo[3]  — Скелет (Только игроки)
//    DrawIo[5]  — Дистанция (отдельный текст для стиля 0)
//    DrawIo[6]  — Полоска HP/инфо над игроком
//    DrawIo[7]  — Радар
//    DrawIo[10] — Иконка оружия
//    DrawIo[14] — Гранаты
//    DrawIo[17] — Скрыть ботов (оригинальная логика)
//    DrawIo[31] — Только игроки для аимбота
//    DrawIo[32] — Игнорировать поверженных
//    DrawIo[40] — FPS-оверлей
//    DrawIo[45] — Отрисовка круга FOV
//    DrawIo[46] — Прятать ESP при прицеливании
//    DrawIo[49] — Точка предикта (аим)
//
//    NumIo[3]   — Радиус FOV (пиксели)
//    NumIo[8]   — Целевая кость (1..15)
//    NumIo[16]  — Масштаб радара
//    NumIo[21]  — Критерий выбора цели (0 — экранная дистанция, 1 — мировая)
//
// ===============================
// Developer item mode integration (externs from main.cpp)
extern bool g_items_dev_mode;
extern char g_dev_watch_name[128];
extern float g_dev_watch_best;
extern bool g_items_draw_overrides;
bool FindItemOverrideC(const char* name, ImU32* out_col, char* out_label, int label_cap);

// Счётчики и подготовка
AimCount = 0;
fanhua.PlayerCount = 0;
fanhua.BotCount = 0;
constexpr float kMaxDrawDistance = 434.0f; // максимальная дистанция отрисовки ESP
float cornerRadius = 7.0f;
bool radarArrowDrawn = false;
// Базовые указатели UE (не трогаем здесь оффсеты подключения матрицы)
fanhua.Gname = driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xe47e8f0) + 0x110);
fanhua.Uworld = driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xea54a70) + 0x30);
fanhua.Uleve = driver->read<uintptr_t>(fanhua.Uworld + 0x20);
fanhua.Arrayaddr = driver->read<uintptr_t>(fanhua.Uleve + 0xA0);
fanhua.Count = driver->read<int>(fanhua.Uleve + 0xA8);
fanhua.Oneself = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.libUE4 + 0xea54a70) + 0x30) + 0x450);
fanhua.MyTeam = driver->read<int>(fanhua.Oneself + 0x938);
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
	if (DrawIo[1000]) {
	float 观透Team = NumIo[49];
	fanhua.TeamID = (team_main > 0) ? team_main : team_alt;
if (观透Team == fanhua.TeamID){
continue;
}
}


fanhua.Health = (driver->read<float>(fanhua.Objaddr + 0xDB8) / driver->read<float>(fanhua.Objaddr + 0xDBC)) * 100;
if (fanhua.Health > 100)
continue;
// Позиция/дистанция/проекция
driver->read(object + 0x1c0, &fanhua.Pos, 12);
GetDistance(fanhua.Pos, fanhua.MyPos, &fanhua.Distance);
WorldToScreen(&fanhua.ScreenPos, &fanhua.camera, &fanhua.w, fanhua.Pos);
// Ограничение дальности: отбрасываем далеко стоящие объекты сразу,
// чтобы счётчик и цвет сверху были корректными.
if (fanhua.Distance > kMaxDrawDistance) {
    continue;
}
// Классификация «бот/игрок» по двум признакам, затем инкремент счётчиков
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
driver->read(driver->read<uintptr_t>(fanhua.Objaddr + 0xd98) + 0x1C0,  &fanhua.Predict, sizeof(fanhua.Predict)); // 载具向量
} else {
driver->read(object + 0x260, &fanhua.Predict, sizeof(fanhua.Predict)); 
}
long int rw = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(fanhua.Objaddr + 0x348) + 0x30) + 0x388);
fanhua.drb = driver->read<int>(rw + 0x1C4);
fanhua.drt = driver->read<int>(rw + 0x1FC);
fanhua.drj = driver->read<int>(rw + 0x234);
fanhua.敌方动作 = driver->read<int>(driver->read<uint64_t>(fanhua.Objaddr + 0xfa0));
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
// Минимальный набор костей для всех: голова, грудь, таз
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
    // Полный скелет — только для игроков
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
    // Для ботов — только лодыжки (корректный низ бокса)
    FTransform Llwtrans = getBone(fanhua.Bone + g * 48);
    FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
    fanhua.Left_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
    fanhua.Left_Ankle.ScreenPos = WorldToScreen(fanhua.Left_Ankle.Pos, fanhua.matrix, fanhua.camera);
    FTransform Lrwtrans = getBone(fanhua.Bone + h * 48);
    FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
    fanhua.Right_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
    fanhua.Right_Ankle.ScreenPos = WorldToScreen(fanhua.Right_Ankle.Pos, fanhua.matrix, fanhua.camera);
}
if (DrawIo[20] && ((DrawIo[31] && !fanhua.IsBot) || !DrawIo[31]) && ((DrawIo[32] && fanhua.Health > 0) || !DrawIo[32])) {
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
    Draw->AddRect({left, top}, {right, bottom}, boxColor, 0, 0, 绘制粗细[1]);
}
if (DrawIo[2]) {
    static const float rayStartY = (draw_style[0] == 0 || draw_style[0] == 1) ? 73 : 
                                 (draw_style[0] == 2) ? screen_y*0.135f : 0;
    const ImVec2 endPos = {fanhua.Head.ScreenPos.x, fanhua.Head.ScreenPos.y - 70};
    const bool isOffScreen = (fanhua.Head.ScreenPos.x < 0 || fanhua.Head.ScreenPos.x > screen_x ||
                             fanhua.Head.ScreenPos.y < 0 || fanhua.Head.ScreenPos.y > screen_y ||
                             fanhua.camera <= 0.0f);
    ImColor rayColor = fanhua.IsBot ? ImColor(0, 255, 0) : ImColor(173, 216, 230);
    const float lineWidth = fanhua.IsBot ? 绘制粗细[4] : 绘制粗细[3];
    if (!isOffScreen) {
        Draw->AddLine({screen_x / 2, rayStartY}, endPos, rayColor, lineWidth);
    }
}
// Скелет — только для игроков, а не для ботов
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
DrawHealth({fanhua.Head.ScreenPos.x, top }, fanhua.w * 2, fanhua.Health, fanhua.TeamID, "人机");
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
}
if (wz.w > 0){
物资函数(载具, 0, 400);
物资函数(宝箱, 1, 150);
物资函数(物资箱, 2, 150);
物资函数(武器箱, 3, 150);
物资函数(药品箱, 4, 150);
物资函数(信函箱, 5, 150);
物资函数(姥爷物资, 6, 150);
物资函数(狗子, 7, 150);
物资函数(金插, 8, 120);
物资函数(信号枪_, 9, 200);
物资函数(药品, 11, 150);
物资函数(手枪, 12, 150);
物资函数(步枪, 13, 150);
物资函数(冲锋枪, 14, 150);
物资函数(狙击枪, 15, 150);
物资函数(霰弹枪, 16, 150);
物资函数(子弹, 17, 150);
物资函数(弹夹, 18, 150);
物资函数(枪口, 19, 150);
物资函数(瞄准镜, 20, 150);
if (物资[9]) {
string name;
name += "召回信号枪[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "RevivalCard_PickUp_C") || strstr(wz.Name, "BP_Ammo_Flare_Pickup_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[8]) {
string name;
name += "自救器[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "_revivalAED_Pickup_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[7]) {
string name;
name += "肾上腺素[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "jection_Pickup_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[6]) {
string name;
name += "饮料[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "ink_Pickup_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[5]) {
string name;
name += "止痛药[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "lls_Pickup_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[4]) {
string name;
name += "金插[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "perPeopleSkill")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[3]) {
string name;
name += "信号枪[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "BP_Pistol_Flaregun_Wrapper_C") || strstr(wz.Name, "Flare")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 255), name.c_str());
}
}
if (物资[18]) {
string name;
name += "宝箱[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "EscapeBoxHight_SupplyBox_Lv5_C") || strstr(wz.Name, "Escape_SupplyBax_Lv5_C") || strstr(wz.Name, "EscapeBoxHight_SupplyBox_Lv4_C") || strstr(wz.Name, "Escape_SupplyBax_Lv4_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 0,0, 255), name.c_str());
}
}
if (物资[45]) {
string name;
name += "盒子[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "CharacterDeadInventoryBox_") || strstr(wz.Name, "BP_PlayerDeadListWrapper_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 0,0, 255), name.c_str());
}
}
if (物资[42]) { 
string name;
name += CasName;
if(wz.Distance<400){
name += "[";
name += to_string((int)wz.Distance);
name += "米]";
auto textSize = ImGui::CalcTextSize(name.c_str(),0, 30);
Draw->AddText(NULL, 20,{wz.ScreenPos.x-(textSize.x / 2), wz.ScreenPos.y}, ImColor(255,255,0,255), name.c_str());  
}
}
if (物资[60]) {
string name;
name += "超级物资箱！[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "MilitarySupplyBoxBase_Baltic_Classic_C") || strstr(wz.Name, "EscapeBox_SpeEffect_C")) {
    Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 0,0, 255), name.c_str());
}
}
// Removed BOSS feature per request
if (物资[1]) {
    // Localized vehicle/item label override
    std::string locLabel = i18n::FromClassName(wz.Name);
    if (!locLabel.empty()) {
        std::string text;
        text += locLabel;
        text += "[";
        text += std::to_string((int)wz.Distance);
        text += (i18n::GetLanguage() == i18n::Lang::RU ? "м]" : "m]");
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,255,255,255), text.c_str());
    } else {
    string name;
    string name1;
    string name2;
    string name3;
    string name4;
    string name5;
    string name6;
    string name7;
    string name8;
    string name9;
    string name10;
    string name11;
    string name12;
    string name13;
    string name14;
    string name15;
    string name16;
    string name17;
    string name18;
    string name19;
    string name20;
    string name21;
    string name22;
    string name23;
    string name24;
    string name25;
    string name26;
    string name27;
    string name28;
    string name29;
    string name30;
    string name31;
    string name32;
    string name33;
    string name34;
    string name35;
    string name36;
    name += "蹦蹦[" + to_string((int)wz.Distance) + "米]\n";
    name1 += "吉普[" + to_string((int)wz.Distance) + "米]\n";
    name2 += "摩托车[" + to_string((int)wz.Distance) + "米]\n";
    name3 += "三轮摩托车[" + to_string((int)wz.Distance) + "米]\n";
    name4 += "轿车[" + to_string((int)wz.Distance) + "米]\n";
    name5 += "冲锋艇[" + to_string((int)wz.Distance) + "米]\n";
    name6 += "快艇[" + to_string((int)wz.Distance) + "米]\n";
    name7 += "巴士[" + to_string((int)wz.Distance) + "米]\n";
    name8 += "双人跑车[" + to_string((int)wz.Distance) + "米]\n";
    name9 += "小绵羊[" + to_string((int)wz.Distance) + "米]\n";
    name10 += "尼罗皮卡[" + to_string((int)wz.Distance) + "米]\n";
    name11 += "轻型雪地车[" + to_string((int)wz.Distance) + "米]\n";
    name12 += "重型雪地车[" + to_string((int)wz.Distance) + "米]\n";
    name13 += "三蹦子[" + to_string((int)wz.Distance) + "米]\n";
    name14 += "装甲车[" + to_string((int)wz.Distance) + "米]\n";
    name15 += "雪地越野[" + to_string((int)wz.Distance) + "米]\n";
    name16 += "大脚车[" + to_string((int)wz.Distance) + "米]\n";
    name17 += "滑翔机[" + to_string((int)wz.Distance) + "米]\n";
    name18 += "肌肉跑车[" + to_string((int)wz.Distance) + "米]\n";
    name19 += "山地摩托[" + to_string((int)wz.Distance) + "米]\n";
    name20 += "电动车[" + to_string((int)wz.Distance) + "米]\n";
    name21 += "敞篷跑车[" + to_string((int)wz.Distance) + "米]\n";
    name22 += "马[" + to_string((int)wz.Distance) + "米]\n";
    name23 += "马[" + to_string((int)wz.Distance) + "米]\n";
    name24 += "马[" + to_string((int)wz.Distance) + "米]\n";
    name25 += "马[" + to_string((int)wz.Distance) + "米]\n";
    name26 += "拉力赛车[" + to_string((int)wz.Distance) + "米]\n";
    name27 += "沙漠大脚车[" + to_string((int)wz.Distance) + "米]\n";
    name28 += "敞篷肌肉跑车[" + to_string((int)wz.Distance) + "米]\n";
    name29 += "旅行车[" + to_string((int)wz.Distance) + "米]\n";
    name30 += "宝宝巴士[" + to_string((int)wz.Distance) + "米]\n";
    name31 += "布朗suv[" + to_string((int)wz.Distance) + "米]\n";
    name32 += "地铁矿车[" + to_string((int)wz.Distance) + "米]\n";
    name33 += "蹦子炮车[" + to_string((int)wz.Distance) + "米]\n";
    name34 += "渣土车[" + to_string((int)wz.Distance) + "米]\n";
    name35 += "挖机[" + to_string((int)wz.Distance) + "米]\n";
    name36 += "自行车[" + to_string((int)wz.Distance) + "米]\n";
    const ImColor outlineColor = ImColor(0, 0, 0, 255);
    const float outlineThickness = 1.0f;
    auto DrawTextWithOutline = [&](const string& text, float x, float y, ImColor textColor) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i != 0 || j != 0) {
                    Draw->AddText(NULL, 25, {x + i*outlineThickness, y + j*outlineThickness}, 
                                outlineColor, text.c_str());
                }
            }
        }
        Draw->AddText(NULL, 25, {x, y}, textColor, text.c_str());
    };
    if (strstr(wz.Name, "Buggy")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name.c_str());
    } else if (strstr(wz.Name, "UAZ")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name1.c_str());
    } else if (strstr(wz.Name, "Motorcycle")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name2.c_str());
    } else if (strstr(wz.Name, "MotorcycleC")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name3.c_str());
    } else if (strstr(wz.Name, "Dacia")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name4.c_str());
    } else if (strstr(wz.Name, "AquaRail")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name5.c_str());
    } else if (strstr(wz.Name, "PG117")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name6.c_str());
    } else if (strstr(wz.Name, "MiniBus")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name7.c_str());
    } else if (strstr(wz.Name, "VH_CoupeRB_1_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name8.c_str());
    } else if (strstr(wz.Name, "Scooter")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name9.c_str());
    } else if (strstr(wz.Name, "Rony")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name10.c_str());
    } else if (strstr(wz.Name, "Snowbike")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name11.c_str());
    } else if (strstr(wz.Name, "Snowmobile")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name12.c_str());
    } else if (strstr(wz.Name, "Tuk")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name13.c_str());
    } else if (strstr(wz.Name, "BRDM")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name14.c_str());
    } else if (strstr(wz.Name, "LadaNiva")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name15.c_str());
    } else if (strstr(wz.Name, "Bigfoot")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name16.c_str());
    } else if (strstr(wz.Name, "Motorglider")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name17.c_str());
    } else if (strstr(wz.Name, "rado_close_1_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name18.c_str());
    } else if (strstr(wz.Name, "ATV")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name19.c_str());
    } else if (strstr(wz.Name, "Scooter")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name20.c_str());
    } else if (strstr(wz.Name, "VH_4SportCar_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name21.c_str());
    } else if (strstr(wz.Name, "VH_Horse_1_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name22.c_str());
    } else if (strstr(wz.Name, "VH_Horse_4_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name23.c_str());
    } else if (strstr(wz.Name, "VH_Horse_3_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name24.c_str());
    } else if (strstr(wz.Name, "VH_Horse_2_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name25.c_str());
    } else if (strstr(wz.Name, "VH_Drift_001_New_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name26.c_str());
    } else if (strstr(wz.Name, "VH_DesertCar_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name27.c_str());
    } else if (strstr(wz.Name, "Mirado")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name28.c_str());
    } else if (strstr(wz.Name, "StationWagon")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name29.c_str());
    } else if (strstr(wz.Name, "VH_Picobus_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name30.c_str());
    } else if (strstr(wz.Name, "VH_Blanc_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name31.c_str());
    } else if (strstr(wz.Name, "TrackVehicle_BP_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name32.c_str());
    } else if (strstr(wz.Name, "VH_LostMobile_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name33.c_str());
    } else if (strstr(wz.Name, "VH_DumpTruck_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name34.c_str());
    } else if (strstr(wz.Name, "MH_Excavator_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name35.c_str());
    } else if (strstr(wz.Name, "VH_Mountainbike_Training_C")) {
        Draw->AddText(NULL, 25, { wz.ScreenPos.x - 10, wz.ScreenPos.y - 10 }, ImColor(255,0,0,255), name36.c_str());
    } 
    }
}
if (DrawIo[910] && 
    (strstr(wz.Name, "EscapeBoxHight_SupplyBox_") != nullptr ||
     strstr(wz.Name, "EscapeBox_SupplyBox_") != nullptr))
{
    if (const int 宝箱状态 = driver->read<int>(fanhua.Objaddr + 0x270);
        宝箱状态 == 0 && wz.Distance >= 0.5f) 
    {
        std::string label = i18n::FromClassName(wz.Name);
        if (label.empty()) label = (i18n::GetLanguage()==i18n::Lang::RU? "Аирдроп" : "Airdrop");
        std::string state = (i18n::GetLanguage()==i18n::Lang::RU? "[Не открыт]" : "[Closed]");
        std::string goods = state + label + "[" + std::to_string(static_cast<int>(wz.Distance)) + (i18n::GetLanguage()==i18n::Lang::RU? "м]" : "m]");
        const ImVec2 textSize = ImGui::CalcTextSize(goods.c_str());
        const ImVec2 basePos = { wz.ScreenPos.x - (textSize.x * 0.5f), wz.ScreenPos.y };
        Draw->AddRectFilled(
            { basePos.x - 5, basePos.y - 3 },
            { basePos.x + textSize.x + 5, basePos.y + textSize.y + 3 },
            ImColor(30, 30, 30, 220),
            cornerRadius
        );
        Draw->AddText(
            nullptr,
            25.0f,
            basePos,
            (宝箱状态 == 0) ? ImColor(255, 215, 0) : ImColor(128, 128, 128),
            goods.c_str()
        );
    }
}
if (DrawIo[46]) {
    bool isSniperRifle = (fanhua.MyWeapon == 103011 ||
                          fanhua.MyWeapon == 103001 ||
                          fanhua.MyWeapon == 103003 ||
                          fanhua.MyWeapon == 103015 ||
                          fanhua.MyWeapon == 103012 ||
                          fanhua.MyWeapon == 103002);
    if (fanhua.IsAiming != 0) {
        continue;
    }
}
if (DrawIo[14]) {
// Prefer class-based localization; draw one line if we can map
std::string lbl = i18n::FromClassName(wz.Name);
if (!lbl.empty()){
    std::string out = lbl + "[" + std::to_string((int)wz.Distance) + (i18n::GetLanguage()==i18n::Lang::RU? "м]":"m]");
    Draw->AddText(NULL, 20, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 0, 0), out.c_str());
} else {
string name;
string name1;
string name2;
name += "手榴弹[" + to_string((int)wz.Distance) + "米]\n";
name1 += "燃烧瓶[" + to_string((int)wz.Distance) + "米]\n";
name2 += "烟雾弹[" + to_string((int)wz.Distance) + "米]\n";
if (strstr(wz.Name, "ProjGrenade_BP_C") || strstr(wz.Name, "BP_Grenade_Shoulei_C")) {
Draw->AddText(NULL, 20, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 0, 0), name.c_str());
Draw->AddCircle({ wz.ScreenPos.x, wz.ScreenPos.y }, 45.0f, ImColor(255, 0, 0), 12);
} else if (strstr(wz.Name, "BP_Grenade_Burn_C") || strstr(wz.Name, "ProjBurn_BP_C")) {
Draw->AddText(NULL, 20, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(255, 255, 0), name1.c_str());
Draw->AddCircle({ wz.ScreenPos.x, wz.ScreenPos.y }, 45.0f, ImColor(255, 0, 0), 12);
} else if (strstr(wz.Name, "BP_Grenade_Smoke_C")) {
Draw->AddText(NULL, 25, { wz.ScreenPos.x - 25, wz.ScreenPos.y - 10 }, ImColor(0, 255, 0), name2.c_str());
Draw->AddCircle({ wz.ScreenPos.x, wz.ScreenPos.y }, 45.0f, ImColor(0, 255, 0), 12);
}
}


if (strstr(wz.Name, "BP_Grenade_Shoulei_C") || strstr(wz.Name, "BP_Grenade_Burn_C")) {
if (wz.Distance < 100) {
// 在屏幕中间绘制一个附近有手雷的文本
string guistr = "附近有投掷物";
Draw->AddText(NULL, 70, {screen_x/2 -160, screen_y  -900}, ImColor(255, 0, 0), guistr.c_str());
}
}
}
}
}
}
if (DrawIo[7] && !radarArrowDrawn) {
    DrawRadarArrow();
}
MaxPlayerCount = AimCount;
fanhua.TotalUp = fanhua.PlayerCount + fanhua.BotCount;
