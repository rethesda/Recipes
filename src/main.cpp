#include "hooks.h"
#include "database.h"
#include "Version.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_message)
	{
		switch (a_message->type) {
		case SKSE::MessagingInterface::kDataLoaded:
		{
			logger::info("Loading ingredients");
			DataBase::GetSingleton(); // Populate the database
			logger::info("Ingredients loaded");
			RecipeReadWorldHook::Install();
			RecipeReadInventoryHook::Install();
			if (REL::Module::IsAE()) {
				ValidateBookAE::Install();
				GetDescriptionHookAE::Install();
			} else {
				GetDescriptionHookSE::Install();
			}

		}
		break;
		default:
			break;
		}
	}

	void InitializeLog()
	{
		auto path = logger::log_directory();
		if (!path) {
			//stl::report_and_fail("Failed to find standard logging directory"sv); // Doesn't work in VR
		}

		*path /= Project::NAME;
		*path += ".log"sv;
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

		logger::info(FMT_STRING("{} v{}"), Project::NAME, Project::Version::NAME);
	}
}


SKSEPluginInfo(
	.Version = {Project::Version::MAJOR, Project::Version::MINOR, Project::Version::PATCH},
	.Name = Project::NAME,
	.Author = Project::AUTHOR,
	.SupportEmail = "N/A",
	.StructCompatibility = SKSE::StructCompatibility::Independent,
)

extern "C" DLLEXPORT const char* APIENTRY GetPluginVersion()
{
	return Project::Version::NAME.data();
}


SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	constexpr auto inlineHooksCount = 6;
	constexpr auto initInfo = SKSE::InitInfo{
		.trampoline = true,
		.trampolineSize = inlineHooksCount * 0x14
	};
	SKSE::Init(a_skse, initInfo);
	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);
	logger::info("Loaded Plugin");
	return true;
}
