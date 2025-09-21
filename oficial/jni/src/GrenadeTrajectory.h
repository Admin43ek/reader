#ifndef GRENADE_TRAJECTORY_H
#define GRENADE_TRAJECTORY_H

#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstring>
#include <imgui.h>
#include "../include/Tools/VecTool.h"

struct GrenadePath {
    Vec3 position;
    float timeStamp;
};

class GrenadeTrajectory {
public:
    explicit GrenadeTrajectory(float lifespan = 1.2f)
        : lifespan(lifespan) {
        std::memset(lastMatrix, 0, sizeof(lastMatrix));
        screenWidth = 0;
        screenHeight = 0;
    }

    void Update(const Vec3& grenadePos);
    void Draw() const;
    void SetRenderContext(const float matrix[16], int width, int height);
    size_t GetPathSize() const { return path.size(); }

private:
    float lifespan;
    std::vector<GrenadePath> path;
    float lastMatrix[16];
    int screenWidth;
    int screenHeight;

    float GetCurrentTime() const;
    void CleanUpOldPaths(float currentTime);
    ImVec2 WorldToScreen(const Vec3& pos) const;
};

inline std::unordered_map<uintptr_t, GrenadeTrajectory>& GetGrenadeTrajectoryMap() {
    static std::unordered_map<uintptr_t, GrenadeTrajectory> map;
    return map;
}

inline void UpdateAndDrawGrenade(uintptr_t objAddr, const Vec3& grenadePos, const float matrix[16], int screenX, int screenY) {
    auto& trajectory = GetGrenadeTrajectoryMap()[objAddr];
    trajectory.SetRenderContext(matrix, screenX, screenY);
    trajectory.Update(grenadePos);
    trajectory.Draw();
}

inline void GrenadeTrajectory::SetRenderContext(const float matrix[16], int width, int height) {
    std::memcpy(lastMatrix, matrix, sizeof(lastMatrix));
    screenWidth = width;
    screenHeight = height;
}

inline void GrenadeTrajectory::Update(const Vec3& grenadePos) {
    const float currentTime = GetCurrentTime();
    path.push_back({ grenadePos, currentTime });
    CleanUpOldPaths(currentTime);
}

inline void GrenadeTrajectory::Draw() const {
    if (path.empty() || screenWidth <= 0 || screenHeight <= 0) {
        return;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    for (size_t i = 0; i < path.size(); ++i) {
        ImVec2 screenPos = WorldToScreen(path[i].position);
        if (screenPos.x < 0.0f && screenPos.y < 0.0f) {
            continue;
        }
        drawList->AddCircleFilled(screenPos, 2.5f, IM_COL32(255, 0, 0, 200));
        if (i == 0) {
            continue;
        }
        ImVec2 prevScreenPos = WorldToScreen(path[i - 1].position);
        if (prevScreenPos.x < 0.0f && prevScreenPos.y < 0.0f) {
            continue;
        }
        drawList->AddLine(prevScreenPos, screenPos, IM_COL32(255, 255, 255, 255), 2.0f);
    }
}

inline float GrenadeTrajectory::GetCurrentTime() const {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration_cast<std::chrono::duration<float>>(clock::now().time_since_epoch()).count();
}

inline void GrenadeTrajectory::CleanUpOldPaths(float currentTime) {
    while (!path.empty() && (currentTime - path.front().timeStamp) > lifespan) {
        path.erase(path.begin());
    }
}

inline ImVec2 GrenadeTrajectory::WorldToScreen(const Vec3& pos) const {
    const float* m = lastMatrix;
    float camera = m[3] * pos.x + m[7] * pos.y + m[11] * pos.z + m[15];
    if (camera <= 0.001f) {
        return ImVec2(-1.0f, -1.0f);
    }
    const float halfWidth = static_cast<float>(screenWidth) * 0.5f;
    const float halfHeight = static_cast<float>(screenHeight) * 0.5f;
    float r_x = halfWidth + (m[0] * pos.x + m[4] * pos.y + m[8] * pos.z + m[12]) / camera * halfWidth;
    float r_y = halfHeight - (m[1] * pos.x + m[5] * pos.y + m[9] * pos.z + m[13]) / camera * halfHeight;
    return ImVec2(r_x, r_y);
}

#endif // GRENADE_TRAJECTORY_H