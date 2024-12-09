#pragma once

namespace settings {

	// window settings

	namespace window {
		LPCWSTR ClassName = L"External Overlay Class";
		LPCWSTR WindowName = L"DMA Overlay";
		int Width = GetSystemMetrics(SM_CXSCREEN);
		int Height = GetSystemMetrics(SM_CYSCREEN);
	}

	namespace dma {
		bool MemoryMap = false;
	}


	
	bool SerialKmbox = false;

}