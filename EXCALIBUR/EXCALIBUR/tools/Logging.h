#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Rendering {

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}


#define TRACE(...)         ::Rendering::Log::GetClientLogger()->trace(__VA_ARGS__)
#define INFO(...)          ::Rendering::Log::GetClientLogger()->info(__VA_ARGS__)
#define WARN(...)          ::Rendering::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ERROR(...)         ::Rendering::Log::GetClientLogger()->error(__VA_ARGS__)
#define CRITICAL(...)      ::Rendering::Log::GetClientLogger()->critical(__VA_ARGS__)