#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <iostream>
#include <string>
#include "Mutex.hpp"
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <ctime>
#include <cstdio>

namespace LogModule
{
    const std::string gsep = "\r\n";
    using namespace LockModule;
    class LogStrategy
    {
    public:
        ~LogStrategy() = default;

        virtual void SyncLog(const std::string &message) = 0;
    };

    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy() {}

        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);
            std::cout << message << gsep;
        }

        ~ConsoleLogStrategy() {}

    private:
        Mutex _mutex;
    };

    const std::string default_path = "./log";
    const std::string default_file = "my.log";
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &path = default_path, const std::string &file = default_file)
            : _path(path), _file(file)
        {
            LockGuard lockguard(_mutex);
            if (std::filesystem::exists(_path))
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_path);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << 'n';
            }
        }

        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);
            std::string filename = _path + (_path.back() == '/' ? "" : "/") + _file;
            std::ofstream out(filename, std::ios::app);
            if (!out.is_open())
            {
                return;
            }

            out << message << gsep;

            out.close();
        }

        ~FileLogStrategy()
        {
        }

    private:
        std::string _path;
        std::string _file;
        Mutex _mutex;
    };

    enum class Loglevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    std::string Level2Str(Loglevel level)
    {
        switch (level)
        {
        case Loglevel::DEBUG:
            return "DEBUG";
        case Loglevel::INFO:
            return "INFO";
        case Loglevel::WARNING:
            return "WARNING";
        case Loglevel::ERROR:
            return "ERROR";
        case Loglevel::FATAL:
            return "FATAl";
        default:
            return "UNKOWN";
        }
    }

    std::string GetTimeStamp()
    {
        time_t curr = time(nullptr);
        struct tm curr_tm;
        localtime_r(&curr, &curr_tm);
        char timebuffer[128];
        snprintf(timebuffer, sizeof(timebuffer), "%04d-%02d-%02d %02d:%02d:%02d",
                 curr_tm.tm_year + 1900,
                 curr_tm.tm_mon + 1,
                 curr_tm.tm_mday,
                 curr_tm.tm_hour,
                 curr_tm.tm_min,
                 curr_tm.tm_sec);
        return timebuffer;
    }

    class Logger
    {
    public:
        Logger()
        {
            EnableConsoleLogStrategy();
        }

        void EnableFileLogStrategy()
        {
            _fflush_strategy = std::make_unique<FileLogStrategy>();
        }

        void EnableConsoleLogStrategy()
        {
            _fflush_strategy = std::make_unique<ConsoleLogStrategy>();
        }

        class LogMessage
        {
        public:
            LogMessage(Loglevel level, const std::string &src_name, int line_number, Logger &logger)
                : _current_time(GetTimeStamp()), _level(level), _pid(getpid()), _src_name(src_name), _line_number(line_number), _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _current_time << "] "
                   << "[" << Level2Str(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _src_name << "] "
                   << "[" << _line_number << "] "
                   << "- ";
                _loginfo = ss.str();
            }

            // LogMessage() << "hello,world" << "XXX" << 3.13 << 1234
            template <class T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }

            ~LogMessage()
            {
                if (_logger._fflush_strategy)
                {
                    _logger._fflush_strategy->SyncLog(_loginfo);
                }
            }

        private:
            std::string _current_time;
            Loglevel _level;
            pid_t _pid;
            std::string _src_name;
            int _line_number;
            std::string _loginfo;
            Logger &_logger;
        };

        LogMessage operator()(Loglevel level, std::string name, int line)
        {
            return LogMessage(level, name, line, *this);
        }

        ~Logger()
        {
        }

    private:
        std::unique_ptr<LogStrategy> _fflush_strategy;
    };

    Logger logger;

#define LOG(level) logger(level, __FILE__, __LINE__)
#define Enable_Console_Log_Strategy() logger.EnableConsoleLogStrategy()
#define EnableFileLogStrategy() logger.EnableFileLogStrategy()
}

#endif
