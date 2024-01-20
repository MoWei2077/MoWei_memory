#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
// 钢铁后台配置
bool isIronBackendEnabled() {
    std::string configFile = "/data/adb/modules/NEW_MW_A0/配置文件.conf";
    std::ifstream file(configFile);
    std::string line;
   bool isIronBackendEnabled = false;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("钢铁后台=") != std::string::npos) {
                std::string value = line.substr(line.find("=") + 1);
                if (value == "true") {
                    isIronBackendEnabled = true;
                }
                break;
            }
        }
        file.close();
    } else {
        std::cout << "无法打开配置文件：" << configFile << std::endl;
    }

    return isIronBackendEnabled;
}
// 乖巧进程配置
bool kill_isIronBackendEnabled() {
    std::string configFile = "/data/adb/modules/NEW_MW_A0/配置文件.conf";
    std::ifstream file(configFile);
    std::string line;
    bool kill_isIronBackendEnabled = false;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("乖巧进程=") != std::string::npos) {
                std::string value = line.substr(line.find("=") + 1);
                if (value == "true") {
                    kill_isIronBackendEnabled = true;
                }
                break;
            }
        }
        file.close();
    } else {
        std::cout << "无法打开配置文件：" << configFile << std::endl;
    }

    return kill_isIronBackendEnabled;
}
// 乖巧进程日志配置
bool kill_LOG_isIronBackendEnabled() {
    std::string configFile = "/data/adb/modules/NEW_MW_A0/配置文件.conf";
    std::ifstream file(configFile);
    std::string line;
    bool kill_logisIronBackendEnabled = false;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("钢铁后台=") != std::string::npos) {
                std::string value = line.substr(line.find("=") + 1);
                if (value == "true") {
                    kill_logisIronBackendEnabled = true;
                }
                break;
            }
        }
        file.close();
    } else {
        std::cout << "无法打开配置文件：" << configFile << std::endl;
    }

    return kill_logisIronBackendEnabled;
}
// 钢铁后台主要代码
void setOomAdjValue(const std::string& pid, int oomAdjValue) {
    std::string filePath = "/proc/" + pid + "/oom_adj";
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << oomAdjValue;
        file.close();
    } else {
        std::cerr << "无法打开文件：" << filePath << std::endl;
    }
}

void setOomScoreAdjValue(const std::string& pid, int oomScoreAdjValue) {
    std::string filePath = "/proc/" + pid + "/oom_score_adj";
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << oomScoreAdjValue;
        file.close();
    } else {
        std::cerr << "无法打开文件：" << filePath << std::endl;
    }
}

std::vector<std::string> getSubdirectories(const std::string& directory) {
    std::vector<std::string> subdirectories;
    DIR* dir = opendir(directory.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name != "." && name != "..") {
                subdirectories.push_back(name);
            }
        }
        closedir(dir);
    }
    return subdirectories;
}
bool processFile() {
    std::ifstream file("config.txt"); // 配置文件路径
    if (!file) {
        std::cerr << "无法打开配置文件\n";
        return false;
    }

    std::string line;
    std::string lastActivity;
    int counter = 0;
    std::ofstream logFile("/sdcard/Android/MW_A0/日志.txt", std::ios_base::app); // 打开日志文件，以追加模式写入
    while (true) {
        FILE* pipe = popen("dumpsys window windows | grep -E 'mCurrentFocus|mFocusedApp' | awk -F '[ /}]' '{print $5}'", "r");
        if (!pipe) {
            std::cerr << "无法执行shell命令\n";
            return false;
        }
        char buffer[128];
        std::string activity = "";
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            activity += buffer;
        }
        pclose(pipe);
// activity计数器
        if (activity != lastActivity) {
            counter++;
            lastActivity = activity;
        }
// 当应用切换计数器的值为3时再进行kill
        if (counter == 3) {
            while (std::getline(file, line)) {
                if (line.find("KILL") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos == std::string::npos) {
                        std::cerr << "配置错误：线程名缺少':'\n";
                        return false;
                    } else {
                        std::string threadName = line.substr(pos + 1);
                        std::string command = "kill -9 " + threadName;
                        int result = system(command.c_str());
                        if (result == 0) {
                            if (isIronBackendEnabled()) {
                                logFile << "成功杀死的线程: " << threadName << "\n";
                            }
                        } else {
                            if (isIronBackendEnabled()) {
                                logFile << "无法杀死的线程: " << threadName << "\n";
                            }
                        }
                    }
                }
            }
            break;
        }

        sleep(1); // 每秒检查一次
    }

    return true;
}

int main(){
  pid_t pid = fork();  // 创建新进程并获取进程ID
  if (pid == -1) {
        std::cerr << "Fork子进程失败!" << std::endl;
        return 1;
    } else if (pid == 0) {
        // 这个是判断是否需要执行
        if (isIronBackendEnabled) {
     //    开始判断是否需要执行
    std::string procDirectory = "/proc";
    std::vector<std::string> subdirectories = getSubdirectories(procDirectory);
    for (const std::string& subdirectory : subdirectories) {
        setOomAdjValue(subdirectory, -17);  // -17将不会杀死后台 关于这些值会在应用切换时发生变动 就只能通过APP的hook系统框架解决
        setOomScoreAdjValue(subdirectory, -1000);  
         }
    } 
} else {
    // 这个是不执行的操作
    system("");
// 这个是父ID该进行的操作
   
        if (kill_isIronBackendEnabled) {
     //    开始判断是否需要执行
while (true){
  if (!processFile()) {
        std::cerr << "处理文件时出错\n";
        return 1;
               }
            }
        }
       return 0;
    } 
}