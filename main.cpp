#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

// 简单的JSON解析函数
std::string extractField(const std::string& json, const std::string& field) {
    std::string searchStr = "\"" + field + "\":";
    size_t pos = json.find(searchStr);
    if (pos == std::string::npos) return "";

    pos += searchStr.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == ':' || json[pos] == '\t' || json[pos] == '"')) {
        pos++;
    }

    if (pos >= json.length()) return "";

    size_t end = pos;
    while (end < json.length() && json[end] != ',' && json[end] != '}' && json[end] != ']' && json[end] != '"' && json[end] != ' ' && json[end] != '\n' && json[end] != '\t') {
        end++;
    }
    return json.substr(pos, end - pos);
}

// 时间差计算
int getMinutesDifference(const std::string& time1, const std::string& time2) {
    int hour1 = std::stoi(time1.substr(11, 2));
    int min1 = std::stoi(time1.substr(14, 2));
    int hour2 = std::stoi(time2.substr(11, 2));
    int min2 = std::stoi(time2.substr(14, 2));

    return (hour2 - hour1) * 60 + (min2 - min1);
}

struct Anomaly {
    double magnetic_field;
    std::string sensor_id;
    std::string sent_time;

    bool operator<(const Anomaly& other) const {
        return sent_time < other.sent_time;
    }
};

void processFile(const std::string& filename) {
    std::cout << "=== Processing " << filename << " ===" << std::endl;

    try {
        // 读取文件
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        // 解析received_time
        std::string received_time = extractField(content, "received_time");

        // 解析所有readings
        std::vector<Anomaly> allReadings;
        size_t pos = 0;

        while ((pos = content.find("\"magnetic_field\"", pos)) != std::string::npos) {
            size_t start = content.rfind('{', pos);
            size_t end = content.find('}', pos) + 1;
            if (start == std::string::npos || end == std::string::npos) break;

            std::string readingStr = content.substr(start, end - start);

            double magnetic_field = std::stod(extractField(readingStr, "magnetic_field"));
            std::string sensor_id = extractField(readingStr, "sensor_id");
            std::string sent_time = extractField(readingStr, "sent_time");

            allReadings.push_back({ magnetic_field, sensor_id, sent_time });
            pos = end;
        }

        // 检测异常
        std::vector<Anomaly> anomalies;
        std::map<std::string, std::vector<double>> sensorHistory;

        for (const auto& reading : allReadings) {
            // 检查数据是否过期
            if (getMinutesDifference(reading.sent_time, received_time) > 30) {
                continue;
            }

            std::string sensor_id = reading.sensor_id;
            double current = reading.magnetic_field;

            // 如果历史数据不足5个，先记录
            if (sensorHistory[sensor_id].size() < 5) {
                sensorHistory[sensor_id].push_back(current);
                continue;
            }

            // 计算最近5个的平均值
            double sum = 0.0;
            for (int i = sensorHistory[sensor_id].size() - 5; i < sensorHistory[sensor_id].size(); ++i) {
                sum += sensorHistory[sensor_id][i];
            }
            double average = sum / 5.0;

            // 检查异常
            if (current > 5.0 * average) {
                anomalies.push_back({ current, sensor_id, reading.sent_time });
            }

            // 记录当前读数
            sensorHistory[sensor_id].push_back(current);
        }

        // 按时间排序
        std::sort(anomalies.begin(), anomalies.end());

        // 输出结果
        std::cout << "{\n";
        std::cout << "    \"anomalies\": [\n";
        for (size_t i = 0; i < anomalies.size(); ++i) {
            const auto& anomaly = anomalies[i];
            std::cout << "        {\n";
            std::cout << "            \"magnetic_field\": " << anomaly.magnetic_field << ",\n";
            std::cout << "            \"sensor_id\": \"" << anomaly.sensor_id << "\",\n";
            std::cout << "            \"sent_time\": \"" << anomaly.sent_time << "\"\n";
            std::cout << "        }";
            if (i < anomalies.size() - 1) std::cout << ",";
            std::cout << "\n";
        }
        std::cout << "    ]\n";
        std::cout << "}\n\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error processing " << filename << ": " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // 如果没有参数，自动运行三个测试文件
    if (argc == 1) {
        processFile("test1.json");
        processFile("test2.json");
        processFile("test3.json");
    }
    else if (argc == 2) {
        processFile(argv[1]);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " [input_file.json]" << std::endl;
        std::cerr << "If no file specified, runs test1.json, test2.json, test3.json" << std::endl;
        return 1;
    }
    system("pause");
    return 0;
}
