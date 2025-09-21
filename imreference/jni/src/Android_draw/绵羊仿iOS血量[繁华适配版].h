float borderPadding = 2.0f;
float borderThickness = 1.5f;
float topY = y - 30.0f;
if (DrawIo[6] && draw_style[1] == 0) {
    const float TOTAL_WIDTH = 235.0f, BASE_HEIGHT = 30.0f, BLOOD_HEIGHT = 4.0f;
    const float TEAM_WIDTH = 55.0f, DISTANCE_WIDTH = 55.0f, PADDING = 8.0f;
    const float Y_OFFSET = 40.0f, SPIKE_HEIGHT = 8.0f, SPIKE_WIDTH = 16.0f;

    static ImColor TEAM_COLORS[] = {
        ImColor(100, 230, 255, 220), ImColor(255, 160, 120, 220),
        ImColor(160, 255, 180, 220), ImColor(255, 210, 100, 220),
        ImColor(220, 160, 255, 220)
    };

    const int team_index = TeamID % (sizeof(TEAM_COLORS)/sizeof(TEAM_COLORS[0]));
    const ImColor TEAM_COLOR = TEAM_COLORS[team_index];
    const ImColor MAIN_BG_COLOR(245, 245, 245, 210), BLOOD_BG_COLOR(210, 210, 210, 220);
    const ImColor TEXT_SHADOW_COLOR(0, 0, 0, 80), BLOOD_FG_COLOR = TEAM_COLOR;

    ImVec2 base_pos(box.x, box.y - Y_OFFSET);
    ImVec2 blood_bg_min(base_pos.x - TOTAL_WIDTH/2, base_pos.y + BASE_HEIGHT + 2);
    ImVec2 blood_bg_max(blood_bg_min.x + TOTAL_WIDTH, blood_bg_min.y + BLOOD_HEIGHT);
    ImGui::GetBackgroundDrawList()->AddRectFilled(blood_bg_min, blood_bg_max, BLOOD_BG_COLOR, BLOOD_HEIGHT/2, ImDrawFlags_RoundCornersAll);

    float health_percent = std::clamp(iHealth, 0.0f, 100.0f) / 100.0f;
    ImVec2 blood_max(blood_bg_min.x + TOTAL_WIDTH * health_percent, blood_bg_max.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(blood_bg_min, blood_max, BLOOD_FG_COLOR, BLOOD_HEIGHT/2, 
        (health_percent >= 1.0f) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);

    ImVec2 spike_points[3] = {
        {base_pos.x - SPIKE_WIDTH/2, blood_bg_max.y},
        {base_pos.x, blood_bg_max.y + SPIKE_HEIGHT},
        {base_pos.x + SPIKE_WIDTH/2, blood_bg_max.y}
    };
    ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(spike_points, 3, TEAM_COLOR);

    ImVec2 main_bg_min(base_pos.x - TOTAL_WIDTH/2, base_pos.y);
    ImVec2 main_bg_max(main_bg_min.x + TOTAL_WIDTH, main_bg_min.y + BASE_HEIGHT);
    ImGui::GetBackgroundDrawList()->AddRectFilled(main_bg_min, main_bg_max, MAIN_BG_COLOR, BASE_HEIGHT/2, ImDrawFlags_RoundCornersAll);

    ImVec2 team_min = main_bg_min, team_max(team_min.x + TEAM_WIDTH, main_bg_max.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(team_min, team_max, TEAM_COLOR, BASE_HEIGHT/2, ImDrawFlags_RoundCornersAll);

    std::string teamText = std::to_string(TeamID);
    ImFont* font = ImGui::GetFont();
    ImVec2 textSize = font->CalcTextSizeA(18, FLT_MAX, 0, teamText.c_str());
    ImVec2 textPos(team_min.x + (TEAM_WIDTH - textSize.x)/2, team_min.y + (BASE_HEIGHT - textSize.y)/2 - 1);
    ImGui::GetBackgroundDrawList()->AddText(font, 18, {textPos.x+1, textPos.y+1}, TEXT_SHADOW_COLOR, teamText.c_str());
    ImGui::GetBackgroundDrawList()->AddText(font, 18, textPos, ImColor(250, 250, 250, 240), teamText.c_str());

if (DrawIo[5]) {
    ImVec2 dist_bg_min(main_bg_max.x - DISTANCE_WIDTH, main_bg_min.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(dist_bg_min, main_bg_max, TEAM_COLOR, BASE_HEIGHT/2, ImDrawFlags_RoundCornersAll);

    std::string distText = (qiyuan.Distance > 999) ? "999+M" : std::to_string((int)qiyuan.Distance)+"M";
    ImFont* font = ImGui::GetFont();
    ImVec2 textSize = font->CalcTextSizeA(18, FLT_MAX, 0, distText.c_str());
    ImVec2 textPos(dist_bg_min.x + (DISTANCE_WIDTH - textSize.x)/2, dist_bg_min.y + (BASE_HEIGHT - textSize.y)/2 - 1);
    ImGui::GetBackgroundDrawList()->AddText(font, 18, {textPos.x+1, textPos.y+1}, TEXT_SHADOW_COLOR, distText.c_str());
    ImGui::GetBackgroundDrawList()->AddText(font, 18, textPos, ImColor(250, 250, 250, 240), distText.c_str());
}

    std::string nameText = name;
    float availableWidth = TOTAL_WIDTH - TEAM_WIDTH - DISTANCE_WIDTH - PADDING*2;
    ImVec2 nameTextSize = font->CalcTextSizeA(22, availableWidth, 0, nameText.c_str());
    ImVec2 nameTextPos(main_bg_min.x + TEAM_WIDTH + PADDING, main_bg_min.y + (BASE_HEIGHT - nameTextSize.y)/2);
    ImGui::GetBackgroundDrawList()->AddText(font, 22, {nameTextPos.x+1, nameTextPos.y+1}, TEXT_SHADOW_COLOR, nameText.c_str());
    ImGui::GetBackgroundDrawList()->AddText(font, 22, nameTextPos, ImColor(60, 60, 60, 255), nameText.c_str());
}
