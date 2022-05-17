#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Zenit {

    std::shared_ptr<spdlog::logger> Log::coreLogger;
    std::shared_ptr<spdlog::logger> Log::clientLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        coreLogger = spdlog::stdout_color_mt("ZENIT");
        coreLogger->set_level(spdlog::level::trace);
        clientLogger = spdlog::stdout_color_mt("APP");
        coreLogger->set_level(spdlog::level::trace);

    }

}
