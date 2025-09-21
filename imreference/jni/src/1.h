
            if (Tab == 0) {
        white = 45;
        wh["win_x_0"].end = 0;
        wh["win_x_1"].end = 0;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 750;
        wh["win_y_2"].end = 750;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 1) {
        white = 45;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 0;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 45;
        wh["win_y_2"].end = 750;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 2) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 0;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 45;
        wh["win_y_3"].end = 750;
        wh["win_y_4"].end = 750;
    } else if (Tab == 4) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 50;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 15;
        wh["win_y_3"].end = 450;
        wh["win_y_4"].end = 45;
    } else if (Tab == 6) {
        white = 750;
        wh["win_x_0"].end = 50;
        wh["win_x_1"].end = 50;
        wh["win_x_2"].end = 50;
        wh["win_x_3"].end = 50;
        wh["win_x_4"].end = 0;
        wh["win_y_1"].end = 15;
        wh["win_y_2"].end = 15;
        wh["win_y_3"].end = 15;
        wh["win_y_4"].end = 45;
    }

        ImGui::SetNextWindowPos({窗口居中X, 窗口居中Y}, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(animatedWindowWidth - 设置.beg + 调色板.beg, animatedWindowHeight), ImGuiCond_Always);
        if (window) {
            zhu_set();
            ImGui::Begin("繁华奚落", &window, 
                ImGuiWindowFlags_NoDecoration | 
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_AlwaysAutoResize);
            
        ImGui::PopStyleVar(); 
        ImGui::PopStyleColor();
        aevrage_all = window_x / 6.302521f; 
        aevrage_now = ImGui::GetWindowSize().x / 6.302521f; 
        tab0 = window_y * 0;
        tab1 = window_y * 1;
        tab2 = window_y * 2;
        tab3 = window_y * 3;
        tab4 = window_y * 4;
        tab5 = window_y * 5;
        tab6 = window_y * 6;

        ImGui::SetCursorPos(ImVec2(0, 20));
        ImGui::BeginChild("左侧标题", ImVec2(aevrage_now*1.5, 670), true);
        ImDrawFlags roundingFlags = ImDrawFlags_RoundCornersLeft;
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetWindowPos(), 
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            ImColor(230, 230, 230, 255),
            20.0f,
            roundingFlags
        );
        
        ImVec2 windowPos = ImGui::GetWindowPos();
        float circleRadius = 10;
        float circleSpacing = 30;
        float startX = windowPos.x + 20;
        float startY = windowPos.y + 20;   
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddCircleFilled(ImVec2(startX, startY), circleRadius, IM_COL32(255, 0, 0, 255));  
        draw_list->AddCircleFilled(ImVec2(startX + circleSpacing, startY), circleRadius, IM_COL32(255, 255, 0, 255));
        draw_list->AddCircleFilled(ImVec2(startX + 2 * circleSpacing, startY), circleRadius, IM_COL32(0, 255, 0, 255));
        
        DrawLogo(logo_png, {ImGui::GetWindowPos().x + aevrage_now/1.944444f + 50, ImGui::GetWindowPos().y + 120}, 110);
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::Separator();
        ImGui::PopStyleColor();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); 
        
        if (button("主 页", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 0;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("绘 图", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 1;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("瞄 准", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 2;        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("物 资", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 4;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        if (button("设 置", ImVec2(ImGui::GetContentRegionAvail().x, 60))) Tab = 6;
        ImGui::EndChild();

        const float spacing = -2.0f;
        const float rightContentWidth = aevrage_now*4 - spacing;
        
        ImGui::SetCursorPos(ImVec2(aevrage_now*1.5 + spacing, 20));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 20.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        
       ImGui::BeginChild("右侧内容", ImVec2(rightContentWidth, 670), true, 
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
        
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetWindowPos(), 
            ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
            ImColor(0, 0, 0, 255),
            20.0f,
            ImDrawFlags_RoundCornersRight
        );

        static float win_y = ImGui::GetCursorPosY();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_0"].beg/2, win_y + 15)); 
        ImGui::BeginChild("主页内容", ImVec2(rightContentWidth - wh["win_x_0"].beg, 720), true);
        if (Tab == 0) 主页_1(); 
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_1"].beg/2, win_y + wh["win_y_1"].beg));
        ImGui::BeginChild("绘制内容", ImVec2(rightContentWidth - wh["win_x_1"].beg, 690), true);
        if (Tab == 1) 绘制_1();
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_2"].beg/2, win_y + wh["win_y_2"].beg));
        ImGui::BeginChild("自瞄内容", ImVec2(rightContentWidth - wh["win_x_2"].beg, 690), true);
        if (Tab == 2) 自瞄_1();
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_3"].beg/2, win_y + wh["win_y_3"].beg));
        ImGui::BeginChild("物资内容", ImVec2(rightContentWidth - wh["win_x_3"].beg, 690), true);
        if (Tab == 4) 物资_1();
        ImGui::EndChild();
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + wh["win_x_4"].beg/2, win_y + wh["win_y_4"].beg));
        ImGui::BeginChild("设置内容", ImVec2(rightContentWidth - wh["win_x_4"].beg, 690), true);
        if (Tab == 6) 设置_1();
        ImGui::EndChild();
        
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::End();       
}
if (!开关 && animationScale <= 0.0f) {
windows = 0;
}
}
}
