#pragma once

#include "json.hpp"
using json = nlohmann::json;

namespace settings {

	// general
	namespace runtime {
		inline int version_major = 1;
		inline int version_minor = 0;
		inline bool criticalPriority = true;
		inline bool headless = false; // no console
		inline bool windowless = false; // no window
		inline bool graphicsOnly = false; // just the menu
		inline std::string configFile = "configs.txt";
	}

	// window settings
	namespace window {
		inline LPCWSTR ClassName = L"Castiel";
		inline LPCWSTR WindowName = L"Castiel";
		inline int Width = GetSystemMetrics(SM_CXSCREEN);
		inline int Height = GetSystemMetrics(SM_CYSCREEN);
	}

	// cheat internal settings
	namespace cheat {
		inline bool GData = true;
		inline bool Camera = true;
		inline bool PlayerList = true;
		inline bool Players = true;
	}

	// dma settings
	namespace dma {
		inline bool MemoryMap = true;

		inline json toJson() {
			return {
				{"MemoryMap", MemoryMap}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("MemoryMap")) MemoryMap = j["MemoryMap"];
		}

#pragma region config
		inline bool saveConfig() {
			std::ofstream file(settings::runtime::configFile);
			if (file.is_open()) {
				file << toJson().dump(4);
				file.close();
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}

		inline bool loadConfig() {
			std::ifstream file(settings::runtime::configFile);
			if (file.is_open()) {
				json j;
				file >> j;
				fromJson(j);
				file.close();
				return true;
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}
#pragma endregion

	}

	// menus
	namespace menu {
		inline bool AdvancedDebug = false;
		inline bool PlayerList = false;
		inline bool Statistics = false;
		inline bool Internals = false;

		inline json toJson() {
			return {
				{"AdvancedDebug", AdvancedDebug},
				{"PlayerList", PlayerList},
				{"Statistics", Statistics},
				{"Internals", Internals}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("AdvancedDebug")) AdvancedDebug = j["AdvancedDebug"];
			if (j.contains("PlayerList")) PlayerList = j["PlayerList"];
			if (j.contains("Statistics")) Statistics = j["Statistics"];
			if (j.contains("Internals")) Internals = j["Internals"];
		}

#pragma region config
		inline bool saveConfig() {
			std::ofstream file(settings::runtime::configFile);
			if (file.is_open()) {
				file << toJson().dump(4);
				file.close();
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}

		inline bool loadConfig() {
			std::ifstream file(settings::runtime::configFile);
			if (file.is_open()) {
				json j;
				file >> j;
				fromJson(j);
				file.close();
				return true;
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}
#pragma endregion

	}

	// kmbox
	namespace kmbox {
		inline bool SerialKmbox = false;
		inline bool NetKmbox = false;

		namespace net {
			inline std::string ip = "192.168.2.188";
			inline std::string port = "1234";
			inline std::string uuid = "1234ABCD";

			inline json toJson() {
				return {
					{"KmboxNetIp", ip},
					{"KmboxNetPort", port},
					{"KmboxNetUuid", uuid}
				};
			}
			inline void fromJson(const json& j) {
				if (j.contains("KmboxNetIp")) ip = j["KmboxNetIp"];
				if (j.contains("KmboxNetPort")) port = j["KmboxNetPort"];
				if (j.contains("KmboxNetUuid")) uuid = j["KmboxNetUuid"];
			}

#pragma region config
			inline bool saveConfig() {
				std::ofstream file(settings::runtime::configFile);
				if (file.is_open()) {
					file << toJson().dump(4);
					file.close();
				}
				else {
					std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
					return false;
				}
			}

			inline bool loadConfig() {
				std::ifstream file(settings::runtime::configFile);
				if (file.is_open()) {
					json j;
					file >> j;
					fromJson(j);
					file.close();
					return true;
				}
				else {
					std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
					return false;
				}
			}
#pragma endregion

		}
	}

	namespace config {
		// misc
		inline bool Fuser = false;

		// aim
		inline bool Aimbot = true;
		inline int AimFov = 100;
		inline int AimSmoothing = 10;

		// trigger
		inline bool TriggerBot = true;
		inline int TriggerDelay = 5; // ms

		// esp
		inline bool Skeleton = true;
		inline bool Box = true;

		// radar
		inline int RadarX = 30;
		inline int RadarY = 30;
		inline int RadarXSize = 300;
		inline int RadarYSize = 300;
		inline int RadarScale = INT_MAX;
		inline float RadarZoom = 1.f;

		inline json toJson() {
			return {
				{"Aimbot", Aimbot},
				{"AimFov", AimFov},
				{"AimSmoothing", AimSmoothing},
				{"TriggerBot", TriggerBot},
				{"TriggerDelay", TriggerDelay},
				{"Skeleton", Skeleton},
				{"Box", Box},
				{"RadarX", RadarX},
				{"RadarY", RadarY},
				{"RadarXSize", RadarXSize},
				{"RadarYSize", RadarYSize},
				{"radarScale", RadarScale},
				{"RadarZoom", RadarZoom}
			};
		}
		inline void fromJson(const json& j) {
			if (j.contains("Aimbot")) Aimbot = j["Aimbot"];
			if (j.contains("AimFov")) AimFov = j["AimFov"];
			if (j.contains("AimSmoothing")) AimSmoothing = j["AimSmoothing"];
			if (j.contains("TriggerDelay")) TriggerDelay = j["TriggerDelay"];
			if (j.contains("AimFov")) AimFov = j["AimFov"];
			if (j.contains("Skeleton")) Skeleton = j["Skeleton"];
			if (j.contains("Box")) Box = j["Box"];
			if (j.contains("RadarX")) RadarX = j["RadarX"];
			if (j.contains("RadarY")) RadarY = j["RadarY"];
			if (j.contains("RadarXSize")) RadarXSize = j["RadarXSize"];
			if (j.contains("RadarYSize")) RadarYSize = j["RadarYSize"];
			if (j.contains("RadarScale")) RadarScale = j["RadarScale"];
			if (j.contains("RadarZoom")) RadarZoom = j["RadarZoom"];
		}

#pragma region config
		inline bool saveConfig() {
			std::ofstream file(settings::runtime::configFile);
			if (file.is_open()) {
				file << toJson().dump(4);
				file.close();
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}

		inline bool loadConfig() {
			std::ifstream file(settings::runtime::configFile);
			if (file.is_open()) {
				json j;
				file >> j;
				fromJson(j);
				file.close();
				return true;
			}
			else {
				std::cout << hue::yellow << "[/] " << hue::white << "Failed to open configuration file" << std::endl;
				return false;
			}
		}
#pragma endregion

	}
	
#pragma region config
	 inline bool saveConfig() {
		dma::saveConfig();
		menu::saveConfig();
		kmbox::net::saveConfig();
		config::saveConfig();
	}

	 inline bool loadConfig() {
		dma::loadConfig();
		menu::loadConfig();
		kmbox::net::loadConfig();
		config::loadConfig();
	}
#pragma endregion

}

struct feature {
	void (*func)();
	__int64 lasttime;
	__int64 period;
};