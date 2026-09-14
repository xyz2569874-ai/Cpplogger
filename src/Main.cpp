#include "Log.hpp"
#include "ThreadPool.hpp"

using namespace LogModule;

int main()
{
    // logger(Loglevel::DEBUG, "main.cpp", 10) << "hello,world";
    Enable_Console_Log_Strategy();
    LOG(Loglevel::DEBUG) << "hello world" << 3.141;
    LOG(Loglevel::DEBUG) << "hello world" << 3.142;
    return 0;
}