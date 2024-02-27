// Logger.cpp
#include "Logger.h"
#include <iostream>
#include <iomanip>

Logger::Logger(const std::string& filename) {
    file.open(filename, std::ios::out | std::ios::app);
    statsFile.open("stats_" + filename, std::ios::out | std::ios::app); // Open a separate file for statistics

    if (!file.is_open() || !statsFile.is_open()) {
        std::cerr << "Error opening log files" << std::endl;
    }
}

Logger::~Logger() {
    if (file.is_open()) {
        file.close();
    }

    if (statsFile.is_open()) {
        statsFile.close();
    }
}

auto Logger::getInstance(const std::string& filename) -> Logger& {
    static Logger instance(filename);
    return instance;
}

auto Logger::log(const std::string& message) -> void{
    if (file.is_open()) {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        file << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;

        if (flushAfterEachLog) {
            file.flush();
        }
    }
}

auto Logger::logStatistics(const std::string& key, const std::string& value) -> void{
    if (statsFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        statsFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] " << key << ": " << value << std::endl;

        if (flushAfterEachLog) {
            statsFile.flush();
        }
    }
}

auto Logger::setFlushAfterEachLog(bool flush) -> void{
    flushAfterEachLog = flush;
}
