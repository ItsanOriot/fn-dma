
// dont move
#include "kmbox/kmboxNet.h"
#include "kmbox/HidTable.h"
//

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <chrono>
#include <iostream>
#include "console/console.h"

#include "settings.h"
#include "performance.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "rendering/Font.h"

#include "menu/menu.h"

#include "rendering/window.h"

#include "dma/DMAHandler.h"
#include "kmbox/kmbox_interface.h"

#include "cheat/cheat.h"
#include "cheat/esp.h"

vector<feature> memoryList;
vector<feature> mainList;

void memRefreshLight() {
	//auto start = std::chrono::high_resolution_clock::now();
	mem.RefreshLight();
	//__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	//std::cout << "took -> " << elapsed << "ms" << std::endl;
}

bool on_initialize() {

	string port = find_port("USB-SERIAL CH340");

	if (!port.empty()) {

		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << hue::green << "[+] " << hue::white << "Serial kmbox found" << std::endl;
			settings::SerialKmbox = true;
		}
		else
		{
			std::cout << hue::yellow << "[/] " << hue::white << "Couldnt connect to serial kmbox" << std::endl;
		}
	}
	else {
		std::cout << hue::yellow << "[/] " << hue::white << "No serial kmbox found" << std::endl;
	}
	
	if (mem.Init(L"FortniteClient-Win64-Shipping.exe", settings::dma::MemoryMap) < 0) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize" << std::endl;
		return false;
	}

	bool fixedDtb = false;
	for (int i = 0; i < 5 && !fixedDtb; i++) {
		if (mem.FixDTB())
			fixedDtb = true;
	}

	std::cout << "\n";

	if (!fixedDtb) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to fix dtb" << std::endl;
		return false;
	}

	mem.cachePML4();

	if (!mem.SCreate()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize all handles" << std::endl;
		return false;
	}

	point::Base = mem.GetBaseAddress();
	if (!point::Base)
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to get base address" << std::endl;
		return false;
	}
	
	if (!mem.InitKeyboard()) 
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize keyboard hotkeys" << std::endl;
		return false;
	}

	if (!update_va_text()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to get text_va" << std::endl;
		return false;
	}

	// memory features
	{
		// refresh memory LOW/MID PRIORITY
		feature RefreshLight = { memRefreshLight , 1, 5000 };
		memoryList.push_back(RefreshLight);

		// update uworld and basics LOW PRIORITY
		feature GDataUpdate = { newInfo , 1, 2000 };
		memoryList.push_back(GDataUpdate);

		// update player list MID PRIORITY
		feature PlayerListUpdate = { updatePlayerList , 1, 1000 };
		memoryList.push_back(PlayerListUpdate);

		// update camera location and rotation HIGH PRIORITY
		feature CameraUpdate = { updateCamera , 1, 0 };
		memoryList.push_back(CameraUpdate);

		// update players location HIGH PRIORITY
		feature PlayersUpdate = { updatePlayers , 1, 0 };
		memoryList.push_back(PlayersUpdate);
	}

	// main thread features
	{
		// esp
		feature Walls = { esp::renderPlayers, 1, 0 };
		mainList.push_back(Walls);

		// health checks (not many)
		feature healthCheck = { healthChecks, 1, 100 };
		mainList.push_back(healthCheck);
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

	// cant toogle menu rn
	menu::Menu();

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