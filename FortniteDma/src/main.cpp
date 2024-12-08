#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <chrono>
#include <iostream>
#include "console/console.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

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

void aim() {

}

struct feature {
	void (*func)();
	__int64 lasttime;
	__int64 period;
};

vector<feature> featurelist;

int on_initialize() {

	string port = find_port("USB-SERIAL CH340");

	if (!port.empty()) {

		if (open_port(hSerial, port.c_str(), CBR_115200)) {
			std::cout << hue::green << "[+] " << hue::white << "serial kmbox found" << std::endl;
		}
		else
		{
			std::cout << hue::yellow << "[+] " << hue::white << "couldnt connect to serial kmbox" << std::endl;
		}
	}
	else {
		std::cout << hue::yellow << "[+] " << hue::white << "no serial kmbox found" << std::endl;
	}
	
	mem.Init(L"FortniteClient-Win64-Shipping.exe"); // make this return a bool and not just exit if an error happens

	bool fixedDtb = false;
	for (int i = 0; i < 5 && !fixedDtb; i++) {
		if (mem.FixDTB())
			fixedDtb = true;
	}

	if (!fixedDtb) {
		std::cout << "Failed to fix dtb.\n";
		return 1;
	}

	mem.cachePML4();

	if (!mem.SCreate()) {
		std::cout << "Failed to initialize all handles.\n";
		return 1;
	}

	point::Base = mem.GetBaseAddress();
	if (!point::Base)
	{
		std::cout << "Failed to get base address\n";
		return 1;
	}
	
	if (!mem.InitKeyboard()) 
	{
		std::cout << "Failed to initialize keyboard hotkeys through kernel.\n";
		return 1;
	}


	////Register features
	//feature Aimbot = {aim,1,1000};
	//featurelist.push_back(Aimbot);

	if (!update_va_text()) {
		std::cout << hue::red << "[!] " << hue::white << "Failed to get text_va\n";
		return 1;
	}

	feature GDataUpdate = { newInfo , 1, 1000 };
	featurelist.push_back(GDataUpdate);

	return 0;
}

void on_exit() {
	CloseHandle(hSerial);
}


void mainloop() {
	// these are just some example shapes. if u wanna find more shapes just type ImGui::GetBackgroundDrawList()-> and look at the autofill suggestions.
	//ImGui::GetBackgroundDrawList()->AddCircleFilled({ 500,500 }, 10.f, ImColor(1.f, 0.f, 0.f));
	//ImGui::GetBackgroundDrawList()->AddRectFilled({ 700, 900 }, { 1000, 1000 }, ImColor(1.f, 0.f, 1.f));
	//ImGui::GetBackgroundDrawList()->AddLine({ 100,100 }, { 200,200 }, ImColor(1.f, 0.f, 0.f));

	//move(10, 10);

	for (feature& i : featurelist) {
		if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - i.lasttime) >= i.period) {
			i.lasttime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
			i.func();
		}
	}

	return;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return 0L;
	}

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}



INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);


	int initialized = on_initialize();
	if (initialized != 0) {
		system("pause");
		return initialized;
	}

	while (true) {
		mainloop();
	}


	return 0;

	// nah the window buggin

	WNDCLASSEXW wc{};
	wc.cbSize		= sizeof(WNDCLASSEXW);
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= window_procedure;
	wc.hInstance	= instance;
	wc.lpszClassName = L"External Overlay Class";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT,
		wc.lpszClassName,
		L"DMA Overlay",
		WS_POPUP,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 144U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else {
		std::cout << "Error creating render target view \n";
		system("pause");
		return 1;
	}

	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;

	while (running) {
		
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				running = false;
			}
		}

		if (!running) {
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		mainloop();

		ImGui::Render();

		constexpr float color[4]{ 0.f,0.f, 0.f, 0.f }; //color of background. last param is opacity. i have it transparent rn so u can see the console but we will set to 1 when we wanna use it
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1U, 0U);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (swap_chain) {
		swap_chain->Release();
	}
	if (device_context) {
		device_context->Release();
	}
	if (device) {
		device->Release();
	}
	if (render_target_view) {
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	on_exit();
	return 0;
}