#pragma once
#include <unordered_map>
#include <string_view>
#include <fstream>
#include <vector>
#include <iostream>

class DataReader {
private:
    // 存储键值对数据，避免频繁的内存分配
    std::unordered_map<std::string_view, std::string_view> dataMap;
    std::vector<char> dataBuffer;  // 用于存储所有的字符串数据

public:
    // 加载文件并保存数据到内存
    bool loadDataFromFile(const std::string& filePath) {
        dataMap.clear(); // 清空已有数据
        dataBuffer.clear(); // 清空数据缓冲区

        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false; // 文件打开失败
        }

        // 估算一个合适的 reserve 值，避免过多扩容
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        dataBuffer.reserve(fileSize);  // 估算缓存大小

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue; // 跳过空行

            size_t delimiterPos = line.find('@');
            if (delimiterPos == std::string::npos) continue; // 无效行，跳过

            // 在 dataBuffer 中存储字符串数据
            size_t bufferStart = dataBuffer.size();
            dataBuffer.insert(dataBuffer.end(), line.begin(), line.end());
            std::string_view key(dataBuffer.data() + bufferStart, delimiterPos);
            std::string_view value(dataBuffer.data() + bufferStart + delimiterPos + 1, line.size() - delimiterPos - 1);

            dataMap[key] = value;
        }
        return true;
    }

    // 根据键查询对应的值，返回 std::string_view
    std::string_view getValue(const std::string_view key) const {
        auto it = dataMap.find(key);
        return (it != dataMap.end()) ? it->second : std::string_view(); // 如果没有找到返回空 string_view
    }
};
