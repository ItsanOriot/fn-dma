#pragma once

namespace settings {

	// general
	namespace runtime {
		inline int version_major = 1;
		inline int version_minor = 0;
		inline bool criticalPriority = true;
		inline bool headless = false; // no console
		inline bool windowless = false; // no window
		inline bool graphicsOnly = false; // just the menu
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
		inline bool MemoryMap = false;
	}

	// menus
	namespace menu {
		inline bool AdvancedDebug = false;
		inline bool PlayerList = false;
		inline bool Statistics = false;
		inline bool Internals = false;
	}

	// kmbox
	inline bool SerialKmbox = false;
	inline bool NetKmbox = false;

	namespace config {
		// esp
		inline bool skeleton = true;
	}

}

struct feature {
	void (*func)();
	__int64 lasttime;
	__int64 period;
};