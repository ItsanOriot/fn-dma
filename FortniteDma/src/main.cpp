
// dont move
#include "kmbox/kmboxNet.h"
#include "kmbox/HidTable.h"
//

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include "console/console.h"

#include "settings.h"
#include "performance.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "rendering/Font.h"
#define STB_IMAGE_IMPLEMENTATION
#include "rendering/stb_image.h"
#include "rendering/images.h"

#include "menu/menu.h"

#include "rendering/window.h"

#include "dma/DMAHandler.h"
#include "kmbox/kmbox_interface.h"

#include "kmbox/kmbox_util.h"

#include "cheat/cheat.h"
#include "cheat/aim.h"
#include "cheat/radar.h"
#include "cheat/esp.h"

vector<feature> memoryList;
vector<feature> mainList;

void memRefreshLight() {
	mem.RefreshLight();
}

void memKeysUpdate() {
	mem.UpdateKeys();
}

bool on_initialize() {

	if (connect_serial_kmbox()) {
		settings::kmbox::SerialKmbox = true;
	}

	// configs
	settings::loadConfig();

	// try to connect to kmbox net
	if (connect_net_kmbox()) {
		settings::kmbox::NetKmbox = true;
		kmNet_lcd_picture_bottom((unsigned char*)images::mini_supernatural_image);
	}
	
	if (mem.Init(L"FortniteClient-Win64-Shipping.exe", settings::dma::MemoryMap) < 0) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize" << std::endl;
		return false;
	}

	std::cout << hue::green << "[+] " << hue::white << "Initialized VMMDLL" << std::endl;


	// retry 5 times but when it doesnt want to work it just doesnt
	bool fixedDtb = false;
	for (int i = 0; i < 5 && !fixedDtb; i++) {
		if (mem.FixDTB())
			fixedDtb = true;
	}

	std::cout << "\n";

	if (!fixedDtb) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to find correct dtb" << std::endl;
		return false;
	}

	if (!mem.cachePML4()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to cache tables" << std::endl;
		return false;
	}

	std::cout << hue::green << "[+] " << hue::white << "Cached tables" << std::endl;

	if (!mem.SCreate()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize all handles" << std::endl;
		return false;
	}

	std::cout << hue::green << "[+] " << hue::white << "Scatter handles Created" << std::endl;

	// idk why sometimes it fails
	point::Base = mem.GetBaseAddress();
	if (!point::Base)
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to get base address" << std::endl;
		return false;
	}

	std::cout << hue::green << "[+] " << hue::white << "Successfully refreshed process" << std::endl;
	
	if (!mem.InitKeyboard()) 
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize keyboard hotkeys" << std::endl;
		return false;
	}

	std::cout << hue::green << "[+] " << hue::white << "Initialized keyboard hotkeys" << std::endl;

	// no longer any offset (for now)
	point::va_text = point::Base;
	//if (!update_va_text()) {
	//	std::cout << hue::red << "[!] " << hue::white << "Failed to get text_va" << std::endl;
	//	return false;
	//}

	// memory features
	{
		// refresh memory LOW PRIORITY
		feature RefreshLight = { memRefreshLight , 1, 5000 };
		memoryList.push_back(RefreshLight);

		// update uworld and basics LOW PRIORITY
		feature GDataUpdate = { newInfo , 1, 2000 };
		memoryList.push_back(GDataUpdate);

		// update local weapon projectile staes MID PRIORITY
		feature WeaponStatsUpdate = { weaponUpdate, 1, 500 };
		memoryList.push_back(WeaponStatsUpdate);

		// update player list MID PRIORITY
		feature PlayerListUpdate = { updatePlayerList , 1, 1000 };
		memoryList.push_back(PlayerListUpdate);

		// update keys MID/HIGH PRIORITY
		feature KeysUpdate = { memKeysUpdate , 1, 5 };
		memoryList.push_back(KeysUpdate);

		// update camera location and rotation HIGH PRIORITY
		feature CameraUpdate = { updateCamera , 1, 0 };
		memoryList.push_back(CameraUpdate);

		// update players location HIGH PRIORITY
		feature PlayersUpdate = { updatePlayers , 1, 0 };
		memoryList.push_back(PlayersUpdate);
	}

	// main thread features
	{
		// health checks (not many)
		feature HealthCheck = { healthChecks, 1, 100 };
		mainList.push_back(HealthCheck);

		// aimbot
		feature Aimbot = { aim::updateAimbot, 1, 3};
		mainList.push_back(Aimbot);

		// triggerbot
		feature Triggerbot = { aim::updateTriggerBot, 1, 5 };
		mainList.push_back(Triggerbot);

		// drawing features must be run every loop

		// debugging drawings
		feature Debug = { esp::Debug, 1, 0 };
		mainList.push_back(Debug);

		// radar
		feature Radar = { radar::Render, 1, 0 };
		mainList.push_back(Radar);

		// esp 
		feature Walls = { esp::renderPlayers, 1, 0 };
		mainList.push_back(Walls);
	}

	return true;
}

void on_exit() {
	CloseHandle(hSerial);

	if (!settings::runtime::headless) {
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);
	}
}

void memoryloop() {

	if (settings::runtime::criticalPriority) {
		// set thread priority
		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
			std::cout << hue::yellow << "[/] " << hue::white << "Failed to set critical priority to memory thread" << std::endl;
	}

	// never quit?
	while (true) {
		auto start = std::chrono::high_resolution_clock::now();

		for (feature& i : memoryList) {
			if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
				i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				i.func();
			}
		}

		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::memoryThreadData.addValue(static_cast<float>(elapsed));
	}
}

void mainFeatures() {
	for (feature& i : mainList) {
		if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
			i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			i.func();
		}
	}
}

void mainloop() {

	if (ImGui::IsKeyPressed(ImGuiKey_Insert))
		settings::menu::open = !settings::menu::open;

	if (settings::menu::open)
		menu::Menu();

	// fov idk where to put it
	if (settings::config::Aimbot) ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(settings::window::Width/2, settings::window::Height/2), settings::config::AimFov, ImColor(255,255,255,255), 1000, 1.f);

	mainFeatures();

	return;
}


INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

	if (!settings::runtime::headless) {
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}

	if (settings::runtime::criticalPriority) {
		// set thread priority
		if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
			std::cout << hue::yellow << "[/] " << hue::white << "Failed to set critical priority to main thread" << std::endl;
	}
	
	if (!settings::runtime::graphicsOnly) {
		if (!on_initialize()) {
			std::cout << hue::yellow << "[/] " << hue::white << "Press enter to exit" << std::endl;
			std::cin.get();
			return 1;
		}

		thread memoryThread(memoryloop);
		memoryThread.detach();
	}

	// wanted to make it run without a window for debugging not sure if needed
	if (!settings::runtime::windowless) {
		InitWindow(instance, cmd_show);

		while (UpdateWindow(mainloop)) {

		}

		DestroyWindow();
	}
	else {
		while (true) {
			auto start = std::chrono::high_resolution_clock::now();

			mainFeatures();

			__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			stats::mainThreadData.addValue(static_cast<float>(elapsed));
		}
	}

	on_exit();
	return 0;
}