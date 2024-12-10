
#include "menu.h"

#include "../cheat/global.h"
#include "../performance.h"
#include "../rendering/Font.h"
#include "../settings.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include <format>
#include <numeric>

void menu::Menu() {
	ImVec2 size = ImVec2(940, 500);
	ImGui::SetNextWindowSize(size);
	ImGui::PushFont((ImFont*)fonts::typenatural_font);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (ImGui::Begin(std::format("SuperNatural v{:d}.{:d}", settings::runtime::version_major, settings::runtime::version_minor).c_str(), NULL, ImGuiWindowFlags_NoResize)) {

		// aim options
		ImGui::SetCursorPos(ImVec2(10, 30));
		ImGui::BeginChild("Aim", ImVec2(300, 475), ImGuiChildFlags_Border);
		ImGui::Text("Aim");
		ImGui::Separator();
		ImGui::Text("no aim :(");
		ImGui::Separator();
		ImGui::EndChild();

		// esp options
		ImGui::SetCursorPos(ImVec2(320, 30));
		ImGui::BeginChild("Esp", ImVec2(300, 475), ImGuiChildFlags_Border);
		ImGui::Text("Esp");
		ImGui::Separator();
		ImGui::Checkbox("Skeleton", &settings::config::skeleton);
		ImGui::Separator();
		ImGui::EndChild();

		// debug options
		ImGui::SetCursorPos(ImVec2(630, 30));
		ImGui::BeginChild("Debug", ImVec2(300, 475), ImGuiChildFlags_Border);
		ImGui::Text("Debug");
		ImGui::Separator();
		ImGui::Checkbox("Statistics", &settings::menu::Statistics);
		ImGui::Separator();
		ImGui::Checkbox("Advanced Debug", &settings::menu::AdvancedDebug);
		ImGui::Separator();
		ImGui::Checkbox("Player List Viewer", &settings::menu::PlayerList);
		ImGui::Separator();
		ImGui::Checkbox("Internals", &settings::menu::Internals);
		ImGui::Separator();
		ImGui::EndChild();

	} ImGui::End();
	ImGui::PopFont();

	if (settings::menu::AdvancedDebug)
		AdvancedDebugWindow();

	if (settings::menu::PlayerList)
		PlayerListWindow();

	if (settings::menu::Statistics)
		StatisticsWindow();

	if (settings::menu::Internals)
		InternalsWindow();
	
}

void menu::InternalsWindow() {
	ImVec2 size = ImVec2(500, 350);
	ImGui::SetNextWindowSize(size);
	if (ImGui::Begin("Internals (Dev Only)", NULL, ImGuiWindowFlags_NoResize)) {

		ImGui::Separator();
		ImGui::Checkbox("GData", &settings::cheat::GData);
		ImGui::Separator();
		ImGui::Checkbox("Camera", &settings::cheat::Camera);
		ImGui::Separator();
		ImGui::Checkbox("PlayerList", &settings::cheat::PlayerList);
		ImGui::Separator();
		ImGui::Checkbox("Players", &settings::cheat::Players);
		ImGui::Separator();

	} ImGui::End();
}

void menu::StatisticsWindow() {
	ImVec2 size = ImVec2(500, 500);
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

		ImGui::Separator();

		static bool advanced = false;

		ImGui::Checkbox("Advanced view", &advanced);

		ImGui::Separator();

		if (advanced) {
			// GData
			const auto& GDataValues = stats::GDataData.getValues();
			float GDataAvgMs = std::accumulate(GDataValues.begin(), GDataValues.end(), 0.0f) / GDataValues.size();
			float GDataMaxMs = GDataValues.empty() ? 0.0f : *std::max_element(GDataValues.begin(), GDataValues.end());

			ImGui::Text("GDatad performance Graph (ms per loop)");
			ImGui::PlotLines("##Performance", GDataValues.data(), GDataValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", GDataAvgMs, GDataMaxMs);

			ImGui::Separator();

			// PlayerList
			const auto& PlayerListValues = stats::updatePlayerListData.getValues();
			float PlayerListAvgMs = std::accumulate(PlayerListValues.begin(), PlayerListValues.end(), 0.0f) / PlayerListValues.size();
			float PlayerListMaxMs = PlayerListValues.empty() ? 0.0f : *std::max_element(PlayerListValues.begin(), PlayerListValues.end());

			ImGui::Text("PlayerList performance Graph (ms per loop)");
			ImGui::PlotLines("##Performance", PlayerListValues.data(), PlayerListValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", PlayerListAvgMs, PlayerListMaxMs);

			ImGui::Separator();

			// Camera
			const auto& CameraValues = stats::updateCameraData.getValues();
			float CameraAvgMs = std::accumulate(CameraValues.begin(), CameraValues.end(), 0.0f) / CameraValues.size();
			float CameraMaxMs = CameraValues.empty() ? 0.0f : *std::max_element(CameraValues.begin(), CameraValues.end());

			ImGui::Text("Camera performance Graph (ms per loop)");
			ImGui::PlotLines("##Performance", CameraValues.data(), CameraValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", CameraAvgMs, CameraMaxMs);

			ImGui::Separator();

			// PlayerList
			const auto& PlayersValues = stats::updatePlayersData.getValues();
			float PlayersAvgMs = std::accumulate(PlayersValues.begin(), PlayersValues.end(), 0.0f) / PlayersValues.size();
			float PlayersMaxMs = PlayersValues.empty() ? 0.0f : *std::max_element(PlayersValues.begin(), PlayersValues.end());

			ImGui::Text("PlayersPos performance Graph (ms per loop)");
			ImGui::PlotLines("##Performance", PlayersValues.data(), PlayersValues.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));
			ImGui::Text("Average: %.2f ms Max: %.2f ms", PlayersAvgMs, PlayersMaxMs);
		}

	} ImGui::End();
}

void menu::PlayerListWindow() {
	ImVec2 size = ImVec2(510, 750);

	std::unordered_map<uintptr_t, PlayerCache> PlayerList = readyCache;
	static uintptr_t selectedPlayer = 0;

	// player list
	ImGui::SetNextWindowSize(size);

	if (ImGui::Begin("Player List", NULL, ImGuiWindowFlags_NoResize)) {
		ImGui::SetCursorPos(ImVec2(10, 30));
		ImGui::BeginChild("Player List", ImVec2(490, 725), ImGuiChildFlags_Border);

		for (auto it : PlayerList) {

			bool isSelected = (selectedPlayer == it.first);

			if (ImGui::Selectable(std::format("PlayerState -> 0x{:X} {} {} ", it.first, (!it.second.Pawn || !it.second.BoneArray) ? "(invalid)" : "(valid)", it.first == point::PlayerState ? "(me)" : "").c_str())) {
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
			ImGui::Text("World Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head3D.x, PlayerList[selectedPlayer].Head3D.y, PlayerList[selectedPlayer].Head3D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom3D.x, PlayerList[selectedPlayer].Bottom3D.y, PlayerList[selectedPlayer].Bottom3D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip3D.x, PlayerList[selectedPlayer].Hip3D.y, PlayerList[selectedPlayer].Hip3D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck3D.x, PlayerList[selectedPlayer].Neck3D.y, PlayerList[selectedPlayer].Neck3D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft3D.x, PlayerList[selectedPlayer].UpperArmLeft3D.y, PlayerList[selectedPlayer].UpperArmLeft3D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight3D.x, PlayerList[selectedPlayer].UpperArmRight3D.y, PlayerList[selectedPlayer].UpperArmRight3D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand3D.x, PlayerList[selectedPlayer].LeftHand3D.y, PlayerList[selectedPlayer].LeftHand3D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand3D.x, PlayerList[selectedPlayer].RightHand3D.y, PlayerList[selectedPlayer].RightHand3D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT3D.x, PlayerList[selectedPlayer].LeftHandT3D.y, PlayerList[selectedPlayer].LeftHandT3D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT3D.x, PlayerList[selectedPlayer].RightHandT3D.y, PlayerList[selectedPlayer].RightHandT3D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh3D.x, PlayerList[selectedPlayer].RightThigh3D.y, PlayerList[selectedPlayer].RightThigh3D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh3D.x, PlayerList[selectedPlayer].LeftThigh3D.y, PlayerList[selectedPlayer].LeftThigh3D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf3D.x, PlayerList[selectedPlayer].RightCalf3D.y, PlayerList[selectedPlayer].RightCalf3D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf3D.x, PlayerList[selectedPlayer].LeftCalf3D.y, PlayerList[selectedPlayer].LeftCalf3D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot3D.x, PlayerList[selectedPlayer].LeftFoot3D.y, PlayerList[selectedPlayer].LeftFoot3D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot3D.x, PlayerList[selectedPlayer].RightFoot3D.y, PlayerList[selectedPlayer].RightFoot3D.z).c_str());
			ImGui::Separator();
			ImGui::Text("Screen Postions");
			ImGui::Text(std::format("Head -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Head2D.x, PlayerList[selectedPlayer].Head2D.y, PlayerList[selectedPlayer].Head2D.z).c_str());
			ImGui::Text(std::format("Bottom -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Bottom2D.x, PlayerList[selectedPlayer].Bottom2D.y, PlayerList[selectedPlayer].Bottom2D.z).c_str());
			ImGui::Text(std::format("Hip -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Hip2D.x, PlayerList[selectedPlayer].Hip2D.y, PlayerList[selectedPlayer].Hip2D.z).c_str());
			ImGui::Text(std::format("Neck -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].Neck2D.x, PlayerList[selectedPlayer].Neck2D.y, PlayerList[selectedPlayer].Neck2D.z).c_str());
			ImGui::Text(std::format("UpperArmLeft -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmLeft2D.x, PlayerList[selectedPlayer].UpperArmLeft2D.y, PlayerList[selectedPlayer].UpperArmLeft2D.z).c_str());
			ImGui::Text(std::format("UpperArmRight -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].UpperArmRight2D.x, PlayerList[selectedPlayer].UpperArmRight2D.y, PlayerList[selectedPlayer].UpperArmRight2D.z).c_str());
			ImGui::Text(std::format("LeftHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHand2D.x, PlayerList[selectedPlayer].LeftHand2D.y, PlayerList[selectedPlayer].LeftHand2D.z).c_str());
			ImGui::Text(std::format("RightHand -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHand2D.x, PlayerList[selectedPlayer].RightHand2D.y, PlayerList[selectedPlayer].RightHand2D.z).c_str());
			ImGui::Text(std::format("LeftHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftHandT2D.x, PlayerList[selectedPlayer].LeftHandT2D.y, PlayerList[selectedPlayer].LeftHandT2D.z).c_str());
			ImGui::Text(std::format("RightHand2 -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightHandT2D.x, PlayerList[selectedPlayer].RightHandT2D.y, PlayerList[selectedPlayer].RightHandT2D.z).c_str());
			ImGui::Text(std::format("RightThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightThigh2D.x, PlayerList[selectedPlayer].RightThigh2D.y, PlayerList[selectedPlayer].RightThigh2D.z).c_str());
			ImGui::Text(std::format("LeftThigh -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftThigh2D.x, PlayerList[selectedPlayer].LeftThigh2D.y, PlayerList[selectedPlayer].LeftThigh2D.z).c_str());
			ImGui::Text(std::format("RightCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightCalf2D.x, PlayerList[selectedPlayer].RightCalf2D.y, PlayerList[selectedPlayer].RightCalf2D.z).c_str());
			ImGui::Text(std::format("LeftCalf -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftCalf2D.x, PlayerList[selectedPlayer].LeftCalf2D.y, PlayerList[selectedPlayer].LeftCalf2D.z).c_str());
			ImGui::Text(std::format("LeftFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].LeftFoot2D.x, PlayerList[selectedPlayer].LeftFoot2D.y, PlayerList[selectedPlayer].LeftFoot2D.z).c_str());
			ImGui::Text(std::format("RightFoot -> x: {:.2f} y: {:.2f} z: {:.2f}", PlayerList[selectedPlayer].RightFoot2D.x, PlayerList[selectedPlayer].RightFoot2D.y, PlayerList[selectedPlayer].RightFoot2D.z).c_str());
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

		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Players Rendered -> {:d}", info::render::playersRendered).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Players Looped -> {:d}", info::render::playersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Valid Players Looped -> {:d}", info::render::validPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Invalid Players Looped -> {:d}", info::render::invalidPlayersLooped).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Teammates Skipped -> {:d}", info::render::teammatesSkipped).c_str());

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
		ImGui::Text(std::format("Camera FieldOfView -> {:.2f} ", mainCamera.FieldOfView).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("Seconds -> {:.2f}", point::Seconds).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("Local Team -> {:d}", local_player::localTeam).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayers -> 0x{:X}", point::LocalPlayers).c_str());
		//ImGui::SetCursorPos(ImVec2(x, y += 20));
		//ImGui::Text(std::format("LocalPlayer -> 0x{:X}", point::LocalPlayer).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 40));
		ImGui::Text(std::format("PlayerArray -> 0x{:X}", point::PlayerArray.data).c_str());
		ImGui::SetCursorPos(ImVec2(x, y += 20));
		ImGui::Text(std::format("PlayerArray Count -> {:d}", point::PlayerArray.count).c_str());

	} ImGui::End();


}