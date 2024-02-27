// Logger.h
#pragma once

#include <fstream>
#include <string>
#include <chrono>

class Logger {
public:
        static auto getInstance(const std::string& filename) -> Logger&;
        
        // Log message for general use
        auto log(const std::string& message) -> void;
        
        // Log message for statistics
        auto logStatistics(const std::string& key, const std::string& value) -> void;

        auto setFlushAfterEachLog(bool flush) -> void;

        auto flush() -> void {
            file.flush();
            statsFile.flush();
        }

    private:
        Logger(const std::string& filename);
        ~Logger();

        std::ofstream file;
    std::ofstream statsFile; // Separate file for statistics
    bool flushAfterEachLog = false;
};
