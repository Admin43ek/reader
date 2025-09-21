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
    {"Main",           {"Main",            "Главная"}},
    {"Settings",       {"Settings",        "Настройки"}},
    {"Language",       {"Language",        "Язык"}},
    {"English",        {"English",         "Английский"}},
    {"Russian",        {"Russian",         "Русский"}},
    {"Theme",          {"Theme",           "Тема"}},
    {"Nightly",        {"Nightly",         "Ночной"}},
    {"Daily",          {"Daily",           "Дневной"}},
    {"Target FPS",     {"Target FPS",      "Целевая частота кадров"}},
    {"Connect Data",   {"Connect Data",    "Подключить данные"}},
    {"Matrix Acquired", {"Matrix Acquired", "Матрица получена"}},
    {"Enter key",      {"Enter key",       "Ввод ключа"}},
    {"Key",            {"Key",             "Ключ"}},
    {"Invalid key",    {"Invalid key",     "Неверный ключ"}},
    {"Try again",      {"Try again",       "Попробуйте снова"}},
    {"Driver selection", {"Driver selection","Выбор драйвера"}},
    {"ImGui Settings", {"ImGui Settings",  "Настройки ImGui"}},
    {"Transparency",   {"Transparency",    "Прозрачность"}},
    {"ESP",            {"ESP",             "ESP"}},
    {"Aimbot",         {"Aimbot",          "Аимбот"}},
    {"Items",          {"Items",           "Предметы"}},
    {"自 瞄",          {"Aimbot",          "Аимбот"}},
    {"绘 图",          {"ESP",             "ESP"}},
    {"物 资",          {"Items",           "Предметы"}},
    {"关于雷达",        {"Radar",           "Радар"}},
    {"雷达X",          {"Radar X",         "Радар X"}},
    {"雷达Y",          {"Radar Y",         "Радар Y"}},
    {"雷达大小",        {"Radar Size",      "Размер радара"}},
    {"雷达缩放",        {"Radar Scale",     "Масштаб радара"}},
    {"范围",            {"Radius",          "Радиус"}},
    {"平滑",            {"Smooth",          "Плавность"}},
    {"速度",            {"Speed",           "Скорость"}},
    {"触摸",            {"Touch",           "Тач"}},
    {"压枪",            {"Recoil",          "Отдача"}},
    {"预判",            {"Prediction",      "Предикт"}},
    {"空余位",          {"Padding",         "Отступ"}},
    {"  方框",          {"  Boxes",         "  Коробки"}},
    {"  骨骼",          {"  Skeleton",      "  Скелет"}},
    {"  射线",          {"  Rays",          "  Лучи"}},
    {"  距离",          {"  Distance",      "  Дистанция"}},
    {"  背敌",          {"  Off-screen",    "  Вне экрана"}},
    {"  忽地",          {"  Ignore downed", "  Игнорировать ноков"}},
    {"关于雷达",        {"Radar",           "Радар"}},
    {"雷达X",          {"Radar X",         "Радар X"}},
    {"雷达Y",          {"Radar Y",         "Радар Y"}},
    {"雷达大小",        {"Radar Size",      "Размер радара"}},
    {"雷达缩放",        {"Radar Scale",     "Масштаб радара"}},
    {"空余位",          {"Padding",         "Отступ"}},
    {"  距离",          {"  Distance",      "  Дистанция"}},
    {"  雷达",          {"  Radar",         "  Радар"}},
    {"  背敌",          {"  Off-screen",    "  Вне экрана"}},
    {"  人机",          {"  Bots",          "  Боты"}},
    {"  忽地",          {"  Ignore downed", "  Игнорировать ноков"}},
    {"  手持",          {"  Weapon icon",   "  Иконка оружия"}},
    {"  手雷",          {"  Grenades",      "  Гранаты"}},
    {"  血量",          {"  HP",            "  Здоровье"}},
    {"  FPS",           {"  FPS",           "  FPS"}},
    {"  背瞄<未完工>",   {"  Back-aim<WIP>", "  Задний прицел<WIP>"}},
    {"  仅真人(自瞄)",    {"  Only players",  "  Только игроки"}},
    {"  瞄准时不绘制",    {"  Hide while aim", "  Скрыть при прицеливании"}},
    {"关于调节",        {"Tuning",          "Настройка"}},
    {"  自瞄总开关",     {"  Aimbot",        "  Аимбот"}},
    {"  显示自瞄圈",     {"  Show FOV",      "  Показать FOV"}},
    {"  触摸位置",       {"  Touch pos",     "  Позиция тача"}},
    {"  动态范围",       {"  Dynamic FOV",   "  Динамический FOV"}},
    {"  预瞄位置",       {"  Pred. pos",     "  Точка предикта"}},
    {"Trigger",        {"Trigger",         "Триггер"}},
    {"Firing",         {"Firing",          "Стрельба"}},
    {"Aiming",         {"Aiming",          "Прицеливание"}},
    {"Either",         {"Either",          "Любое"}},
    {"Always",         {"Always",          "Всегда"}},
    {"勿放控件，长按拖动", {"CatchMe",         "Перетащи"}},
    {"Target bone",    {"Target bone",     "Целевая кость"}},
    {"Head",           {"Head",            "Голова"}},
    {"Chest",          {"Chest",           "Грудь"}},
    {"Pelvis",         {"Pelvis",          "Таз"}},
    {"Left shoulder",  {"Left shoulder",   "Левое плечо"}},
    {"Right shoulder", {"Right shoulder",  "Правое плечо"}},
    {"Left elbow",     {"Left elbow",      "Левый локоть"}},
    {"Right elbow",    {"Right elbow",     "Правый локоть"}},
    {"Left wrist",     {"Left wrist",      "Левое запястье"}},
    {"Right wrist",    {"Right wrist",     "Правое запястье"}},
    {"Left thigh",     {"Left thigh",      "Левое бедро"}},
    {"Right thigh",    {"Right thigh",     "Правое бедро"}},
    {"Left knee",      {"Left knee",       "Левое колено"}},
    {"Right knee",     {"Right knee",      "Правое колено"}},
    {"Left ankle",     {"Left ankle",      "Левая лодыжка"}},
    {"Right ankle",    {"Right ankle",     "Правая лодыжка"}},
    {"载具类",          {"Vehicles",        "Транспорт"}},
    {"武器类",          {"Weapons",         "Оружие"}},
    {"药品类",          {"Medical",         "Медикаменты"}},
    {"特殊类",          {"Special",         "Особые"}},
    {"  Ammo",         {"  Ammo",          "  Патроны"}},
    {"  Magazine",     {"  Magazine",      "  Магазины"}},
    {"  Airdrop",      {"  Airdrop",       "  Аирдроп"}},
    {"  物资初始化",     {"  Items init",    "  Инициация лута"}},
    {"  载具",          {"  Vehicles",      "  Транспорт"}},
    {"  盒子",          {"  Crates",        "  Ящики"}},
    {"  宝箱状态",       {"  Chest state",   "  Статус сундука"}},
    {"  宝箱",          {"  Airdrop",       "  Аирдроп"}},
    {"  信号枪",        {"  Flaregun",      "  Сигнальный пистолет"}},
    {"  召回信号枪",     {"  Recall flare",  "  Факел возврата"}},
    {"  止痛药",        {"  Painkiller",    "  Обезбол"}},
    {"  饮料",          {"  Energy",        "  Энергетик"}},
    {"  肾上腺素",       {"  Adrenaline",    "  Адреналин"}},
    {"  自救器",        {"  Self revive",   "  Саморевив"}},
    {"  药品",          {"  Meds",          "  Аптечки"}},
    {"  金插",          {"  Gold plug",     "  Золотой мод"}},
    {"  BOOS",          {"  Boss",          "  Босс"}},
    {"  超级物资",       {"  Super loot",    "  Супер лут"}},
    {"ScreenShot",     {"ScreenShot",      "Снимок экрана"}},
    {"Save config",    {"Save config",     "Сохранить конфиг"}},
    {"Quit",           {"Quit",            "Выход"}},
    {"Duration",       {"Duration",        "Длительность"}},
    {"Start recording",{"Start recording", "Начать запись"}},
    {"Stop recording", {"Stop recording",  "Остановить запись"}},
    {"Kill",            {"Kill",            "Убить процесс"}},
    {"Performance",    {"Performance",     "Производительность"}},
    {"UI",             {"UI",              "Интерфейс"}},
    {"Cars",           {"Cars",            "Машины"}},
    {"Bots",           {"Bots",            "Боты"}},
    {"On",             {"On",              "Вкл"}},
    {"Off",            {"Off",             "Выкл"}},
    {"Developer: show raw item names", {"Developer: show raw item names", "Режим разработчика: показывать исходные названия предметов"}},
    {"Load items", {"Load items", "Загрузить предметы"}},
    {"Use top-center buttons to add/load items", {"Use top-center buttons to add/load items", "Используйте кнопки сверху по центру, чтобы добавить/загрузить предметы"}},
    {"Boot animation placeholder", {"Boot animation placeholder", "Заглушка анимации загрузки"}},
    {"Bot",            {"Bot",             "Бот"}},
    {"Circle",         {"Circle",          "Круг"}},
    {"Arrow",          {"Arrow",           "Стрелка"}},
    {"Grid",           {"Grid",            "Сетка"}},
    {"Rectangle",      {"Rectangle",       "Прямоугольник"}},
    {"Classic",        {"Classic",         "Классика"}},
    {"Banner",         {"Banner",          "Баннер"}},
    {"iOS Card",       {"iOS Card",        "iOS карта"}},
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
    if (has("Buggy")) return pick("Buggy", "Багги");
    if (has("UAZ")) return pick("UAZ", "УАЗ");
    if (has("MotorcycleC")||has("Motorcycle")) return pick("Motorcycle", "Мотоцикл");
    if (has("Dacia")||has("VH_4SportCar")||has("StationWagon")) return pick("Car", "Машина");
    if (has("Mirado")) return pick("Mirado", "Мирадo");
    if (has("PG117")||has("AquaRail")) return pick("Boat", "Лодка");
    if (has("Scooter")) return pick("Scooter", "Скутер");
    if (has("Snowbike")) return pick("Snowbike", "Сноубайк");
    if (has("Snowmobile")) return pick("Snowmobile", "Снегоход");
    if (has("Tuk")) return pick("Tuk", "Тук-тук");
    if (has("BRDM")) return pick("BRDM", "БРДМ");
    if (has("ATV")) return pick("ATV", "Квадроцикл");
    if (has("Motorglider")||has("Glider")) return pick("Glider", "Планёр");
    if (has("Horse")) return pick("Horse", "Лошадь");
    if (has("TrackVehicle")||has("DumpTruck")||has("Excavator")) return pick("Truck", "Грузовик");
    // Common items / crates / signals
    if (has("CharacterDeadInventoryBox")||has("DeadList")) return pick("Death box", "Ящик смерти");
    if (has("EscapeBoxHight_SupplyBox")||has("SupplyBox")||has("Airdrop")) return pick("Airdrop", "Аирдроп");
    if (has("Flaregun")||has("Flare")) return pick("Flaregun", "Сигнальный пистолет");
    if (has("_revivalAED")||has("RevivalCard")) return pick("Revival", "Ревив");
    if (has("Grenade_Shoulei")||has("ProjGrenade")) return pick("Frag", "Осколочная");
    if (has("Grenade_Smoke")||has("Smoke")) return pick("Smoke", "Дымовая");
    if (has("Grenade_Burn")||has("ProjBurn")) return pick("Molotov", "Коктейль");
    if (has("Drink_Pickup")||has("ink_Pickup")) return pick("Energy", "Энергетик");
    if (has("jection_Pickup")||has("Adrenaline")) return pick("Adrenaline", "Адреналин");
    if (has("lls_Pickup")||has("Painkiller")) return pick("Painkiller", "Обезбол");
    if (has("MedKit")||has("FirstAid")) return pick("Medkit", "Аптечка");
    // Ammo types
    if (has("Ammo_762")||has("7_62")||has("762mm")) return pick("7.62mm", "7.62мм");
    if (has("Ammo_556")||has("5_56")||has("556mm")) return pick("5.56mm", "5.56мм");
    if (has("Ammo_9mm")||has("9mm")) return pick("9mm", "9мм");
    if (has("Ammo_45ACP")||has("45ACP")||has(".45")) return pick(".45 ACP", ".45 ACP");
    if (has("Ammo_12")||has("12Gauge")||has("12ga")) return pick("12 Gauge", "12 калибр");
    if (has("300Magnum")||has("300")||has("300mag")) return pick(".300 Magnum", ".300 Магнум");
    return {};
}

} // namespace i18n

#endif // I18N_H
