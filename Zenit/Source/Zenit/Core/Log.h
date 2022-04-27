#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>

namespace Zenit {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> clientLogger;

	};
}

#define ZN_CORE_INFO(...)  ::Zenit::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ZN_CORE_WARN(...)  ::Zenit::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ZN_CORE_TRACE(...) ::Zenit::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ZN_CORE_ERROR(...) ::Zenit::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ZN_CORE_FATAL(...) ::Zenit::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define ZN_INFO(...)  ::Zenit::Log::GetClientLogger()->info(__VA_ARGS__)
#define ZN_WARN(...)  ::Zenit::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ZN_TRACE(...) ::Zenit::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ZN_ERROR(...) ::Zenit::Log::GetClientLogger()->error(__VA_ARGS__)
#define ZN_FATAL(...) ::Zenit::Log::GetClientLogger()->fatal(__VA_ARGS__)
