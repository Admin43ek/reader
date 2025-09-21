case 1:
{
    float healthBarLength = 180.0f;
    float healthBarHeight = 5.0f;
    float idBoxWidth = 30.0f;
    float padding = 5.0f;
    float totalWidth = healthBarLength + idBoxWidth + padding;
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - totalWidth/2, y - 42), ImVec2(x + totalWidth/2, y - 11), ImColor(0, 0, 0, 140), 4, 0);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - totalWidth/2, y - 42), ImVec2(x - totalWidth/2 + idBoxWidth, y - 11), ImColor(128, 0, 128, 255), 4, 0);
    char idText[10];
    sprintf(idText, "%d", TeamID);
    auto idSize = ImGui::GetFont()->CalcTextSizeA(20, FLT_MAX, -1, idText);
    DrawTf.绘制字体描边(20, x - totalWidth/2 + idBoxWidth/2 - idSize.x/2, y - 42 + (idBoxWidth - idSize.y)/2,ImVec4{255, 255, 255, 255}, idText);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - totalWidth/2, y - 16), ImVec2(x + totalWidth/2, y - 11), ImColor(0, 0, 0, 200), 1, 0);
    if (iHealth > 0) {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x - totalWidth/2, y - 16), ImVec2(x - totalWidth/2 + (healthBarLength + idBoxWidth + padding) * (iHealth / 100.0f), y - 11), ImColor(255, 255, 0, 255), 1, 0);
    }
    auto nameSize = ImGui::GetFont()->CalcTextSizeA(20, FLT_MAX, -1, name);
    float nameX = x - nameSize.x/2; 
    DrawTf.绘制字体描边(20, nameX, y - 38, ImVec4{255, 255, 255, 255}, name);
    char distanceText[32];
    sprintf(distanceText, "[%dM]", static_cast<int>(fanhua.Distance));
    auto distanceSize = ImGui::GetFont()->CalcTextSizeA(20, FLT_MAX, -1, distanceText);
    DrawTf.绘制字体描边(20, x + totalWidth/2 - distanceSize.x - 5, y - 38, ImVec4{255, 255, 0, 255}, distanceText);
    break;
}
//繁华奚落
//主要修复队伍ID偏框，添加距离显示