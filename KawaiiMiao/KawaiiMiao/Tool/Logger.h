#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Render {

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


#define K_TRACE(...)         ::Render::Log::GetClientLogger()->trace(__VA_ARGS__)
#define K_INFO(...)          ::Render::Log::GetClientLogger()->info(__VA_ARGS__)
#define K_WARN(...)          ::Render::Log::GetClientLogger()->warn(__VA_ARGS__)
#define K_ERROR(...)         ::Render::Log::GetClientLogger()->error(__VA_ARGS__)
#define K_CRITICAL(...)      ::Render::Log::GetClientLogger()->critical(__VA_ARGS__)