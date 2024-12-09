
#include "menu.h"

#include "../cheat/global.h"
#include "../performance.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include <format>
#include <numeric>

void menu::Menu() {
	AdvancedDebugWindow();
	PlayerListWindow();
	Statistics();
}

void menu::Statistics() {
	ImVec2 size = ImVec2(500, 350);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Statistics", NULL, ImGuiWindowFlags_NoResize)) {

		// memory thread
		const auto& MemoryValues = stats::memoryThreadData.getValues();
		float memoryAvgMs = std::accumulate(MemoryValues.begin(), MemoryValues.end(), 0.0f) / MemoryValues.size();
		float memoryFPS = (memoryAvgMs > 0) ? (1000.0f / memoryAvgMs) : 0.0f;

		ImGui::Text("Memory thread performance Graph (ms per loop)");
		ImGui::PlotLines("##Performance", MemoryValues.data(), MemoryValues.size(), 0, nullptr, 0.0f, 50.0f, ImVec2(0, 100));
		ImGui::Text("Average: %.2f ms (%.1f FPS)", memoryAvgMs, memoryFPS);

		ImGui::Separator();

		// main thread
		const auto& MainValues = stats::mainThreadData.getValues();
		float mainAvgMs = std::accumulate(MainValues.begin(), MainValues.end(), 0.0f) / MainValues.size();
		float mainFPS = (mainAvgMs > 0) ? (1000.0f / mainAvgMs) : 0.0f;

		ImGui::Text("Main thread performance Graph (ms per loop)");
		ImGui::PlotLines("##Performance", MainValues.data(), MainValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
		ImGui::Text("Average: %.2f ms (%.1f FPS)", mainAvgMs, mainFPS);

	} ImGui::End();
}

void menu::PlayerListWindow() {
	ImVec2 size = ImVec2(500, 750);

	std::unordered_map<uintptr_t, PlayerCache> PlayerList = newCache;
	static uintptr_t selectedPlayer = 0;

	// player list
	ImGui::SetNextWindowSize(size);

	if (ImGui::Begin("Player List", NULL, ImGuiWindowFlags_NoResize)) {
		
		ImGui::BeginChild("Player List", ImVec2(495, 745), true);

		for (auto it : PlayerList) {

			bool isSelected = (selectedPlayer == it.first);

			if (ImGui::Selectable(std::format("PlayerState -> 0x{:X}", it.first).c_str())) {
				selectedPlayer = it.first;
			}
		}

		ImGui::EndChild();
	}ImGui::End();

	// details window
	if (selectedPlayer && PlayerList.find(selectedPlayer) != PlayerList.end()) {
		ImGui::SetNextWindowSize(ImVec2(400, 350));
		if (ImGui::Begin(std::format("Player Details for -> 0x{:X}", selectedPlayer).c_str(), NULL, ImGuiWindowFlags_NoResize)) {
			ImGui::Separator();
			ImGui::Text(std::format("Pawn -> 0x{:X}", PlayerList[selectedPlayer].Pawn).c_str());
			ImGui::Text(std::format("Mesh -> 0x{:X}", PlayerList[selectedPlayer].Mesh).c_str());
			uintptr_t BoneArray = PlayerList[selectedPlayer].BoneArray1;
			if (!BoneArray) BoneArray = PlayerList[selectedPlayer].BoneArray2;
			ImGui::Text(std::format("BoneArray -> 0x{:X}", BoneArray).c_str());
			ImGui::Text(std::format("RootComponent -> 0x{:X}", PlayerList[selectedPlayer].RootComponent).c_str());
			ImGui::Separator();
			ImGui::Text(std::format("Team -> {:d}", PlayerList[selectedPlayer].TeamId).c_str());
			ImGui::Text(std::format("isDying -> {}", PlayerList[selectedPlayer].isDying & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isDBNO -> {}", PlayerList[selectedPlayer].isDBNO & 0x10 ? "true" : "false").c_str());
			ImGui::Text(std::format("isBot -> {}", PlayerList[selectedPlayer].isBot & (1 << 3) ? "true" : "false").c_str());
			ImGui::Separator();
			ImGui::Text(std::format("Velocity -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Velocity.x, PlayerList[selectedPlayer].Velocity.y, PlayerList[selectedPlayer].Velocity.z).c_str());
			ImGui::Text(std::format("Last Render -> {:.2f}", PlayerList[selectedPlayer].last_render).c_str());
			ImGui::Separator();
		}ImGui::End();
	}
}

void menu::AdvancedDebugWindow() {
	ImVec2 size = ImVec2(800, 500);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Advanced Debug"), NULL, ImGuiWindowFlags_NoResize) {

		int x = 0, y = 0;

		// first row
		x = 10; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Base -> 0x{:X}", point::Base).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("va_text -> 0x{:X}", point::va_text).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Uworld -> 0x{:X}", point::Uworld).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameInstance -> 0x{:X}", point::GameInstance).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PersistentLevel -> 0x{:X}", point::PersistentLevel).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("GameState -> 0x{:X}", point::GameState).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerController -> 0x{:X}", point::PlayerController).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocalPawn -> 0x{:X}", point::Player).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerCameraManager -> 0x{:X}", point::PlayerCameraManager).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerState -> 0x{:X}", point::PlayerState).c_str());

		// second row

		x = 310; y = 30;

		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("LocationPointer -> 0x{:X}", point::LocationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("RotationPointer -> 0x{:X}", point::RotationPointer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Location -> x: {:.2f} y: {:.2f} z: {:.2f}", mainCamera.Location.x, mainCamera.Location.y, mainCamera.Location.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Camera Rotation -> x: {:.2f} y: {:.2f}", mainCamera.Rotation.x, mainCamera.Rotation.y).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Seconds -> {:.2f}", point::Seconds).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Local Position -> x: {:.2f} y: {:.2f} z: {:.2f}", local_player::localPos.x, local_player::localPos.y, local_player::localPos.z).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Local Team -> {:d}", local_player::localTeam).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 60));
		ImGui::Text(std::format("PlayerArray -> 0x{:X}", point::PlayerArray.data).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerArray Count -> {:d}", point::PlayerArray.count).c_str());

	} ImGui::End();


}