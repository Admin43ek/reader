#ifndef I18N_H
#define I18N_H

#include <string>
#include <unordered_map>

namespace i18n {

enum class Lang { EN, RU };

inline Lang g_lang = Lang::RU; // default Russian per request
inline void SetLanguage(Lang l) { g_lang = l; }
inline Lang GetLanguage() { return g_lang; }

struct TRPair { std::string en; std::string ru; };

inline std::unordered_map<std::string, TRPair> g_tr = {
    {"Main",           {"Main",            "�������"}},
    {"Settings",       {"Settings",        "���������"}},
    {"Language",       {"Language",        "����"}},
    {"English",        {"English",         "����������"}},
    {"Russian",        {"Russian",         "�������"}},
    {"Theme",          {"Theme",           "����"}},
    {"Nightly",        {"Nightly",         "������"}},
    {"Daily",          {"Daily",           "�������"}},
    {"Target FPS",     {"Target FPS",      "������� ������� ������"}},
    {"Connect Data",   {"Connect Data",    "���������� ������"}},
    {"Matrix Acquired", {"Matrix Acquired", "������� ��������"}},
    {"Enter key",      {"Enter key",       "���� �����"}},
    {"Key",            {"Key",             "����"}},
    {"Invalid key",    {"Invalid key",     "�������� ����"}},
    {"Try again",      {"Try again",       "���������� �����"}},
    {"Driver selection", {"Driver selection","����� ��������"}},
    {"ImGui Settings", {"ImGui Settings",  "��������� ImGui"}},
    {"Transparency",   {"Transparency",    "������������"}},
    {"ESP",            {"ESP",             "ESP"}},
    {"Aimbot",         {"Aimbot",          "������"}},
    {"Items",          {"Items",           "��������"}},
    {"? ?",          {"Aimbot",          "������"}},
    {"? ?",          {"ESP",             "ESP"}},
    {"? ?",          {"Items",           "��������"}},
    {"????",        {"Radar",           "�����"}},
    {"??X",          {"Radar X",         "����� X"}},
    {"??Y",          {"Radar Y",         "����� Y"}},
    {"????",        {"Radar Size",      "������ ������"}},
    {"????",        {"Radar Scale",     "������� ������"}},
    {"??",            {"Radius",          "������"}},
    {"??",            {"Smooth",          "���������"}},
    {"??",            {"Speed",           "��������"}},
    {"??",            {"Touch",           "���"}},
    {"??",            {"Recoil",          "������"}},
    {"??",            {"Prediction",      "�������"}},
    {"???",          {"Padding",         "������"}},
    {"  ??",          {"  Boxes",         "  �������"}},
    {"  ??",          {"  Skeleton",      "  ������"}},
    {"  ??",          {"  Rays",          "  ����"}},
    {"  ??",          {"  Distance",      "  ���������"}},
    {"  ??",          {"  Off-screen",    "  ��� ������"}},
    {"  ??",          {"  Ignore downed", "  ������������ �����"}},
    {"????",        {"Radar",           "�����"}},
    {"??X",          {"Radar X",         "����� X"}},
    {"??Y",          {"Radar Y",         "����� Y"}},
    {"????",        {"Radar Size",      "������ ������"}},
    {"????",        {"Radar Scale",     "������� ������"}},
    {"???",          {"Padding",         "������"}},
    {"  ??",          {"  Distance",      "  ���������"}},
    {"  ??",          {"  Radar",         "  �����"}},
    {"  ??",          {"  Off-screen",    "  ��� ������"}},
    {"  ??",          {"  Bots",          "  ����"}},
    {"  ??",          {"  Ignore downed", "  ������������ �����"}},
    {"  ??",          {"  Weapon ID",   "  ID ������"}},
    {"  ??",          {"  Grenades",      "  �������"}},
    {"  ??",          {"  HP",            "  ��������"}},
    {"  FPS",           {"  FPS",           "  FPS"}},
    {"  ??<???>",   {"  Back-aim<WIP>", "  ������ ������<WIP>"}},
    {"  ???(??)",    {"  Only players",  "  ������ ������"}},
    {"  ??????",    {"  Hide while aim", "  ������ ��� ������������"}},
    {"????",        {"Tuning",          "���������"}},
    {"  ?????",     {"  Aimbot",        "  ������"}},
    {"  ?????",     {"  Show FOV",      "  �������� FOV"}},
    {"  ????",       {"  Touch pos",     "  ������� ����"}},
    {"  ????",       {"  Dynamic FOV",   "  ������������ FOV"}},
    {"  ????",       {"  Pred. pos",     "  ����� ��������"}},
    {"Trigger",        {"Trigger",         "�������"}},
    {"Firing",         {"Firing",          "��������"}},
    {"Aiming",         {"Aiming",          "������������"}},
    {"Either",         {"Either",          "�����"}},
    {"Always",         {"Always",          "������"}},
    {"????,????", {"CatchMe",         "��������"}},
    {"Target bone",    {"Target bone",     "������� �����"}},
    {"Head",           {"Head",            "������"}},
    {"Chest",          {"Chest",           "�����"}},
    {"Pelvis",         {"Pelvis",          "���"}},
    {"Left shoulder",  {"Left shoulder",   "����� �����"}},
    {"Right shoulder", {"Right shoulder",  "������ �����"}},
    {"Left elbow",     {"Left elbow",      "����� ������"}},
    {"Right elbow",    {"Right elbow",     "������ ������"}},
    {"Left wrist",     {"Left wrist",      "����� ��������"}},
    {"Right wrist",    {"Right wrist",     "������ ��������"}},
    {"Left thigh",     {"Left thigh",      "����� �����"}},
    {"Right thigh",    {"Right thigh",     "������ �����"}},
    {"Left knee",      {"Left knee",       "����� ������"}},
    {"Right knee",     {"Right knee",      "������ ������"}},
    {"Left ankle",     {"Left ankle",      "����� �������"}},
    {"Right ankle",    {"Right ankle",     "������ �������"}},
    {"???",          {"Vehicles",        "���������"}},
    {"???",          {"Weapons",         "������"}},
    {"???",          {"Medical",         "�����������"}},
    {"???",          {"Special",         "������"}},
    {"  Ammo",         {"  Ammo",          "  �������"}},
    {"  Magazine",     {"  Magazine",      "  ��������"}},
    {"  Airdrop",      {"  Airdrop",       "  �������"}},
    {"  ?????",     {"  Items init",    "  ��������� ����"}},
    {"  ??",          {"  Vehicles",      "  ���������"}},
    {"  ??",          {"  Crates",        "  �����"}},
    {"  ????",       {"  Chest state",   "  ������ �������"}},
    {"  ??",          {"  Airdrop",       "  �������"}},
    {"  ???",        {"  Flaregun",      "  ���������� ��������"}},
    {"  ?????",     {"  Recall flare",  "  ����� ��������"}},
    {"  ???",        {"  Painkiller",    "  �������"}},
    {"  ??",          {"  Energy",        "  ���������"}},
    {"  ????",       {"  Adrenaline",    "  ���������"}},
    {"  ???",        {"  Self revive",   "  ���������"}},
    {"  ??",          {"  Meds",          "  �������"}},
    {"  ??",          {"  Gold plug",     "  ������� ���"}},
    {"  BOOS",          {"  Boss",          "  ����"}},
    {"  ????",       {"  Super loot",    "  ����� ���"}},
    {"ScreenShot",     {"ScreenShot",      "������ ������"}},
    {"Save config",    {"Save config",     "��������� ������"}},
    {"Quit",           {"Quit",            "�����"}},
    {"Duration",       {"Duration",        "������������"}},
    {"Start recording",{"Start recording", "������ ������"}},
    {"Stop recording", {"Stop recording",  "���������� ������"}},
    {"Kill",            {"Kill",            "����� �������"}},
    {"Performance",    {"Performance",     "������������������"}},
    {"UI",             {"UI",              "���������"}},
    {"Cars",           {"Cars",            "������"}},
    {"Bots",           {"Bots",            "����"}},
    {"On",             {"On",              "���"}},
    {"Off",            {"Off",             "����"}},
    {"Developer: show raw item names", {"Developer: show raw item names", "����� ������������: ���������� �������� �������� ���������"}},
    {"Load items", {"Load items", "��������� ��������"}},
    {"Use top-center buttons to add/load items", {"Use top-center buttons to add/load items", "����������� ������ ������ �� ������, ����� ��������/��������� ��������"}},
    {"Boot animation placeholder", {"Boot animation placeholder", "�������� �������� ��������"}},
    {"Bot",            {"Bot",             "���"}},
    {"Circle",         {"Circle",          "����"}},
    {"Arrow",          {"Arrow",           "�������"}},
    {"Grid",           {"Grid",            "�����"}},
    {"Rectangle",      {"Rectangle",       "�������������"}},
    {"Classic",        {"Classic",         "��������"}},
    {"Banner",         {"Banner",          "������"}},
    {"iOS Card",       {"iOS Card",        "iOS �����"}},
    {"  Only players (aimbot)", {"  Only players (aimbot)", "  Only players (aimbot)"}},
    {"Frag grenade", {"Frag grenade", "Frag grenade"}},
    {"Molotov cocktail", {"Molotov cocktail", "Molotov cocktail"}},
    {"Smoke grenade", {"Smoke grenade", "Smoke grenade"}},
    {"Incoming grenade!", {"Incoming grenade!", "Incoming grenade!"}},
};

inline std::string TL(const std::string& key) {
    auto it = g_tr.find(key);
    if (it != g_tr.end()) {
        return (g_lang == Lang::RU) ? it->second.ru : it->second.en;
    }
    return key;
}

// Unit replacement helper (no-op placeholder)
inline std::string ReplaceUnits(const std::string& s) {
    return s;
}

// Try to localize UE class names and common raw identifiers
inline std::string FromClassName(const char* n) {
    if (!n) return {};
    auto has = [&](const char* s){ return strstr(n, s) != nullptr; };
    auto pick = [&](const char* en, const char* ru){ return (g_lang==Lang::RU) ? std::string(ru) : std::string(en); };
    // Vehicles
    if (has("Buggy")) return pick("Buggy", "�����");
    if (has("UAZ")) return pick("UAZ", "���");
    if (has("MotorcycleC")||has("Motorcycle")) return pick("Motorcycle", "��������");
    if (has("Dacia")||has("VH_4SportCar")||has("StationWagon")) return pick("Car", "������");
    if (has("Mirado")) return pick("Mirado", "�����o");
    if (has("PG117")||has("AquaRail")) return pick("Boat", "�����");
    if (has("Scooter")) return pick("Scooter", "������");
    if (has("Snowbike")) return pick("Snowbike", "��������");
    if (has("Snowmobile")) return pick("Snowmobile", "��������");
    if (has("Tuk")) return pick("Tuk", "���-���");
    if (has("BRDM")) return pick("BRDM", "����");
    if (has("ATV")) return pick("ATV", "����������");
    if (has("Motorglider")||has("Glider")) return pick("Glider", "�����");
    if (has("Horse")) return pick("Horse", "������");
    if (has("TrackVehicle")||has("DumpTruck")||has("Excavator")) return pick("Truck", "��������");
    // Common items / crates / signals
    if (has("CharacterDeadInventoryBox")||has("DeadList")) return pick("Death box", "���� ������");
    if (has("EscapeBoxHight_SupplyBox")||has("SupplyBox")||has("Airdrop")) return pick("Airdrop", "�������");
    if (has("Flaregun")||has("Flare")) return pick("Flaregun", "���������� ��������");
    if (has("_revivalAED")||has("RevivalCard")) return pick("Revival", "�����");
    if (has("Grenade_Shoulei")||has("ProjGrenade")) return pick("Frag", "����������");
    if (has("Grenade_Smoke")||has("Smoke")) return pick("Smoke", "�������");
    if (has("Grenade_Burn")||has("ProjBurn")) return pick("Molotov", "��������");
    if (has("Drink_Pickup")||has("ink_Pickup")) return pick("Energy", "���������");
    if (has("jection_Pickup")||has("Adrenaline")) return pick("Adrenaline", "���������");
    if (has("lls_Pickup")||has("Painkiller")) return pick("Painkiller", "�������");
    if (has("MedKit")||has("FirstAid")) return pick("Medkit", "�������");
    // Ammo types
    if (has("Ammo_762")||has("7_62")||has("762mm")) return pick("7.62mm", "7.62��");
    if (has("Ammo_556")||has("5_56")||has("556mm")) return pick("5.56mm", "5.56��");
    if (has("Ammo_9mm")||has("9mm")) return pick("9mm", "9��");
    if (has("Ammo_45ACP")||has("45ACP")||has(".45")) return pick(".45 ACP", ".45 ACP");
    if (has("Ammo_12")||has("12Gauge")||has("12ga")) return pick("12 Gauge", "12 ������");
    if (has("300Magnum")||has("300")||has("300mag")) return pick(".300 Magnum", ".300 ������");
    return {};
}

} // namespace i18n

#endif // I18N_H
