#include "Emulator/Emulator.h"
#include "Emulator/SceModuleSystem.h"
#include "Emulator/TLSHandler.h"
#include "Loader/ModuleLoader.h"

#include <cxxopts/cxxopts.hpp>
#include <memory>

LOG_CHANNEL(Main);

cxxopts::ParseResult processCommandLine(int argc, char* argv[])
{
	cxxopts::Options opts("GPCS4", "PlayStation 4 Emulator");
	opts.allow_unrecognised_options();

	opts.positional_help("<[-e] eboot.bin>").show_positional_help();

	opts.add_options()                                                                                                                  //
		("e,eboot", "Set main executable. The folder where GPCS4.exe located will be mapped to /app0.", cxxopts::value<std::string>())  //
		// ("c,conf", "Path of config file", cxxopts::value<std::string>())                                                                //
		;
	opts.add_options("Debug")  //
		// ("system-info", "Print system compability info")                                                                //
		("debug-channel", "Enable debug channel. 'ALL' for all channels.", cxxopts::value<std::vector<std::string>>())  //
		("list-debug-channels", "List avaliable debug channels.");
	opts.add_options("Misc")  //
		("h,help", "Print this help message.");

	opts.parse_positional({ "eboot" });

	// Backup arg count,
	// because cxxopts will change argc value internally,
	// which I think is a bad design.
	const uint32_t argCount = argc;

	auto optResult = opts.parse(argc, argv);
	if (optResult.count("help") || argCount < 2)
	{
		auto helpString = opts.help();
		printf("%s\n", helpString.c_str());
		exit(-1);
	}

	return optResult;
}

int main(int argc, char* argv[])
{
	std::unique_ptr<CEmulator> pEmulator = std::make_unique<CEmulator>();
	int nRet                             = -1;

	do
	{
		auto optResult = processCommandLine(argc, argv);

		// Initialize log system.
		logsys::init(optResult);

		if (!optResult["eboot"].count())
		{
			break;
		}

		// Initialize the whole emulator.

		LOG_DEBUG("GPCS4 start.");

		if (!pEmulator->Init())
		{
			break;
		}

		if (!installTLSManager())
		{
			break;
		}

		CLinker linker      = { *CSceModuleSystem::GetInstance() };
		ModuleLoader loader = { *CSceModuleSystem::GetInstance(), linker };

		auto eboot                      = optResult["eboot"].as<std::string>();
		MemoryMappedModule* ebootModule = nullptr;
		if (!loader.loadModule(eboot, &ebootModule))
		{
			break;
		}

		if (!pEmulator->Run(*ebootModule))
		{
			break;
		}

		uninstallTLSManager();
		pEmulator->Unit();

		nRet = 0;
	} while (false);

	return nRet;
}
