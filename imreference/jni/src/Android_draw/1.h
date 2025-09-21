    // 常量定义
    const float WIDTH = 235.0f, HEIGHT = 30.0f;
    const float TEAM_W = 55.0f, DIST_W = 55.0f, PAD = 8.0f;
    const float Y_OFFSET = 40.0f, SPIKE_H = 8.0f, SPIKE_W = 16.0f;
    const float BAR_H = 4.0f;

    // 颜色定义
    static ImColor TEAM_COLORS[] = {
        ImColor(100, 230, 255, 220), ImColor(255, 160, 120, 220),
        ImColor(160, 255, 180, 220), ImColor(255, 210, 100, 220),
        ImColor(220, 160, 255, 220)
    };
    ImColor teamColor = TEAM_COLORS[TeamID % IM_ARRAYSIZE(TEAM_COLORS)];
    ImColor bgColor(0, 0, 0, 210); // 黑色背景
    ImColor barBgColor(50, 50, 50, 220); // 深灰色血条背景
    ImColor textColor(250, 250, 250, 255); // 白色文字

    // 基础位置计算
    ImVec2 basePos(x, y - Y_OFFSET);
    ImVec2 bgMin(basePos.x - WIDTH/2, basePos.y);
    ImVec2 bgMax(bgMin.x + WIDTH, bgMin.y + HEIGHT);
    
    // 绘制主背景
    ImGui::GetBackgroundDrawList()->AddRectFilled(bgMin, bgMax, bgColor, HEIGHT/2, ImDrawFlags_RoundCornersAll);

    // 队伍ID区域
    ImVec2 teamMax(bgMin.x + TEAM_W, bgMax.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(bgMin, teamMax, teamColor, HEIGHT/2, ImDrawFlags_RoundCornersAll);
    
    // 队伍ID文字
    std::string teamStr = std::to_string(TeamID);
    ImVec2 teamTextSize = ImGui::GetFont()->CalcTextSizeA(18, FLT_MAX, 0, teamStr.c_str());
    ImVec2 teamTextPos(
        bgMin.x + (TEAM_W - teamTextSize.x)/2, 
        bgMin.y + (HEIGHT - teamTextSize.y)/2
    );
    ImGui::GetBackgroundDrawList()->AddText(
        ImGui::GetFont(), 18, teamTextPos, textColor, teamStr.c_str()
    );

    // 距离区域
    ImVec2 distMin(bgMax.x - DIST_W, bgMin.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(distMin, bgMax, teamColor, HEIGHT/2, ImDrawFlags_RoundCornersAll);
    
    // 距离文字
    std::string distStr = (fanhua.Distance > 999) ? "999+M" : std::to_string((int)fanhua.Distance)+"M";
    ImVec2 distTextSize = ImGui::GetFont()->CalcTextSizeA(18, FLT_MAX, 0, distStr.c_str());
    ImVec2 distTextPos(
        distMin.x + (DIST_W - distTextSize.x)/2,
        distMin.y + (HEIGHT - distTextSize.y)/2
    );
    ImGui::GetBackgroundDrawList()->AddText(
        ImGui::GetFont(), 18, distTextPos, textColor, distStr.c_str()
    );

    // 名称文字
    std::string nameStr = name;
    float nameAvailableWidth = WIDTH - TEAM_W - DIST_W - PAD*2;
    ImVec2 nameTextSize = ImGui::GetFont()->CalcTextSizeA(22, nameAvailableWidth, 0, nameStr.c_str());
    ImVec2 nameTextPos(
        bgMin.x + TEAM_W + PAD,
        bgMin.y + (HEIGHT - nameTextSize.y)/2
    );
    ImGui::GetBackgroundDrawList()->AddText(
        ImGui::GetFont(), 22, nameTextPos, textColor, nameStr.c_str()
    );

    // 血条绘制
    ImVec2 barMin(bgMin.x, bgMax.y + 2);
    ImVec2 barMax(bgMax.x, barMin.y + BAR_H);
    ImGui::GetBackgroundDrawList()->AddRectFilled(barMin, barMax, barBgColor, BAR_H/2, ImDrawFlags_RoundCornersAll);
    
    // 动态血量颜色
    float healthPercent = std::clamp(iHealth, 0.0f, 100.0f) / 100.0f;
    ImColor healthColor;
    if (healthPercent > 0.6f) {
        healthColor = ImColor(0, 255, 0, 220); // 绿色
    } else if (healthPercent > 0.3f) {
        healthColor = ImColor(255, 255, 0, 220); // 黄色
    } else {
        healthColor = ImColor(255, 0, 0, 220); // 红色
    }
    
    // 绘制血量前景
    ImVec2 healthMax(barMin.x + WIDTH * healthPercent, barMax.y);
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        barMin, healthMax, healthColor, BAR_H/2, 
        healthPercent >= 1.0f ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft
    );

    // 绘制血条下方的三角形
    ImVec2 spike[3] = {
        {basePos.x - SPIKE_W/2, barMax.y},
        {basePos.x, barMax.y + SPIKE_H},
        {basePos.x + SPIKE_W/2, barMax.y}
    };
    ImGui::GetBackgroundDrawList()->AddConvexPolyFilled(spike, 3, teamColor);

    break;