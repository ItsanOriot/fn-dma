#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <chrono>
#include <iostream>
#include "console/console.h"

#include "settings.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "rendering/Font.h"

#include "menu/menu.h"

#include "rendering/window.h"

#include "dma/DMAHandler.h"
#include "kmbox/kmbox_interface.h"

inline void km_move(int X, int Y) {
	std::string command = "km.move(" + std::to_string(X) + "," + std::to_string(Y) + ")\r\n";
	send_command(hSerial, command.c_str());
}

inline void km_click(int clickDelay) {
	std::string command = "km.click(0)\r\n";
	send_command(hSerial, command.c_str());
}

#include "cheat/cheat.h"
#include "cheat/esp.h"

void aim() {

}

struct feature {
	void (*func)();
	__int64 lasttime;
	__int64 period;
};

vector<feature> memoryList;
vector<feature> mainList;

int on_initialize() {

	string port = find_port("USB-SERIAL CH340");

	if (!port.empty()) {

		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << hue::green << "[+] " << hue::white << "serial kmbox found" << std::endl;
			settings::SerialKmbox = true;
		}
		else
		{
			std::cout << hue::yellow << "[/] " << hue::white << "couldnt connect to serial kmbox" << std::endl;
		}
	}
	else {
		std::cout << hue::yellow << "[/] " << hue::white << "no serial kmbox found" << std::endl;
	}
	
	mem.Init(L"FortniteClient-Win64-Shipping.exe", settings::dma::MemoryMap); // make this return a bool and not just exit if an error happens

	bool fixedDtb = false;
	for (int i = 0; i < 5 && !fixedDtb; i++) {
		if (mem.FixDTB())
			fixedDtb = true;
	}

	if (!fixedDtb) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to fix dtb" << std::endl;
		return 1;
	}

	mem.cachePML4();

	if (!mem.SCreate()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize all handles" << std::endl;
		return 1;
	}

	point::Base = mem.GetBaseAddress();
	if (!point::Base)
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to get base address" << std::endl;
		return 1;
	}
	
	if (!mem.InitKeyboard()) 
	{
		std::cout << hue::red << "[!] " << hue::white << "Failed to initialize keyboard hotkeys" << std::endl;
		return 1;
	}


	////Register features
	//feature Aimbot = {aim,1,1000};
	//featurelist.push_back(Aimbot);

	if (!update_va_text()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to get text_va" << std::endl;
		return 1;
	}

	// memory features
	{
		// update uworld and basics LOW PRIORITY
		feature GDataUpdate = { newInfo , 1, 5000 };
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
		// something in the future
		// aim maybe? if it doesnt have its own thread
	}

	return 0;
}

void on_exit() {
	CloseHandle(hSerial);
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
}

void memoryloop() {
	// never quit?
	while (true) {
		for (feature& i : memoryList) {
			if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
				i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
				i.func();
			}
		}
	}

}

void mainloop() {

	// cant toogle menu rn
	menu::Menu();

	esp::renderPlayers();

	for (feature& i : mainList) {
		if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
			i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			i.func();
		}
	}

	return;
}


INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	int initialized = on_initialize();
	if (initialized != 0) {
		std::cin.get();
		return initialized;
	}

	thread memoryThread(memoryloop);
	memoryThread.detach();

	InitWindow(instance, cmd_show);

	while (UpdateWindow(mainloop)) {

	}

	DestroyWindow();

	on_exit();
	return 0;
}