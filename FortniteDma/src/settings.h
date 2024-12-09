#pragma once

namespace settings {

	// general
	namespace runtime {
		bool headless = false; // no console
		bool windowless = false; // no window
		bool graphicsOnly = false; // just the menu
	}

	// window settings
	namespace window {
		LPCWSTR ClassName = L"Castiel";
		LPCWSTR WindowName = L"Castiel";
		int Width = GetSystemMetrics(SM_CXSCREEN);
		int Height = GetSystemMetrics(SM_CYSCREEN);
	}

	// dma settings
	namespace dma {
		bool MemoryMap = false;
	}

	// aim settings
	bool SerialKmbox = false;

}

