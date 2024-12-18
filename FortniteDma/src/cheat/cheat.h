#pragma once

#include "global.h"
#include "utils.h"
#include "offsets.h"

bool update_va_text() {
	for (int i = 0, e = 0; i < INT_MAX && e < 3; i++) {
		point::va_text = point::Base + i * 0x1000;
		auto uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);
		auto level = mem.Read<uintptr_t>(uworld + offsets::PersistentLevel);
		if (uworld && level && mem.Read<uintptr_t>(level + offsets::OwningWorld) == uworld) {
			return true;
		}
		if (i > 1000) { i = 0; e++; };
	}
	return false;
}

void newInfo()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::GData)
		goto END;

	if (point::Uworld || point::GameInstance || point::GameState) {

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::va_text + offsets::Uworld, sizeof(uintptr_t));

		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraLocationPointer, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::CameraRotationPointer, sizeof(uintptr_t));

		mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));

		if (point::GameInstance) {
			mem.SPrepare(mem.hS, point::GameInstance + offsets::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayers) {
			mem.SPrepare(mem.hS, point::LocalPlayers, sizeof(uintptr_t));
		}

		if (point::LocalPlayer) {
			mem.SPrepare(mem.hS, point::LocalPlayer + offsets::PlayerController, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::LocalPlayer + 0x378, sizeof(uintptr_t));
		}
		
		if (point::PlayerController) {
			mem.SPrepare(mem.hS, point::PlayerController + offsets::LocalPawn, sizeof(uintptr_t));
			mem.SPrepare(mem.hS, point::PlayerController + offsets::Cameramanager, sizeof(uintptr_t));
		}

		if (point::Player) {
			mem.SPrepare(mem.hS, point::Player + offsets::PlayerState, sizeof(uintptr_t));
		}

		if (point::GameState) {
			mem.SPrepare(mem.hS, point::GameState + offsets::PlayerArray, sizeof(tarray));
		}

		if (point::PlayerState) {
			mem.SPrepare(mem.hS, point::PlayerState + offsets::TeamId, sizeof(uint32_t));
		}

		mem.ExecuteMemoryReads(mem.hS);

		point::Uworld = mem.SReads<uintptr_t>(mem.hS, point::va_text + offsets::Uworld);

		point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraLocationPointer);
		point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::CameraRotationPointer);
		point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
		point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
		point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);

		if (point::GameInstance) {
			point::LocalPlayers = mem.SReads<uintptr_t>(mem.hS, point::GameInstance + offsets::LocalPlayers);
		}

		if (point::LocalPlayers) {
			point::LocalPlayer = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayers);
		}

		if (point::LocalPlayer) {
			point::PlayerController = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + offsets::PlayerController);
			point::Settings = mem.SReads<uintptr_t>(mem.hS, point::LocalPlayer + 0x378);
			if (point::Settings) {
				point::MouseSensX = mem.Read<float>(point::Settings + 0x664);
				point::MouseSensY = mem.Read<float>(point::Settings + 0x668);
			}
		}

		if (point::PlayerController) {
			point::Player = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::LocalPawn);
			point::PlayerCameraManager = mem.SReads<uintptr_t>(mem.hS, point::PlayerController + offsets::Cameramanager);
		}

		if (point::Player) {
			point::PlayerState = mem.SReads<uintptr_t>(mem.hS, point::Player + offsets::PlayerState);
		}

		if (point::GameState) {
			point::PlayerArray = mem.SReads<tarray>(mem.hS, point::GameState + offsets::PlayerArray);
		}

		if (point::PlayerState) {
			local_player::localTeam = mem.SReads<uint32_t>(mem.hS, point::PlayerState + offsets::TeamId);
		}

	}
	else {

		point::Uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);

		if (!point::Uworld) goto END;

		mem.SClear(mem.hS);

		mem.SPrepare(mem.hS, point::Uworld + 0x120, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + 0x130, sizeof(uintptr_t));

		mem.SPrepare(mem.hS, point::Uworld + offsets::GameInstance, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::PersistentLevel, sizeof(uintptr_t));
		mem.SPrepare(mem.hS, point::Uworld + offsets::GameState, sizeof(uintptr_t));

		mem.ExecuteMemoryReads(mem.hS);

		point::LocationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + 0x120);
		point::RotationPointer = mem.SReads<uintptr_t>(mem.hS, point::Uworld + 0x130);
		point::GameInstance = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameInstance);
		point::PersistentLevel = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::PersistentLevel);
		point::GameState = mem.SReads<uintptr_t>(mem.hS, point::Uworld + offsets::GameState);

		if (!point::GameState) goto END;

		if (point::GameInstance) point::LocalPlayers = mem.Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);

		if (!point::LocalPlayers) goto END;

		point::LocalPlayer = mem.Read<uintptr_t>(point::LocalPlayers);

		if (!point::LocalPlayer) goto END;

		point::PlayerController = mem.Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);


		if (point::PlayerController) point::Player = mem.Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

		if (point::PlayerController) point::PlayerCameraManager = mem.Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

		if (point::Player) point::PlayerState = mem.Read<uintptr_t>(point::Player + offsets::PlayerState);

		point::PlayerArray = mem.Read<tarray>(point::GameState + offsets::PlayerArray);

		if (point::PlayerState) local_player::localTeam = mem.Read<uint32_t>(point::PlayerState + offsets::TeamId);
	}

END:

__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
stats::GDataData.addValue(static_cast<float>(elapsed));

}

void updateCamera()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::Camera)
		goto END;

	// check if we can update the camera
	if (!point::RotationPointer || !point::LocationPointer || !point::LocalPlayer)
		goto END;

	FNRot rotation;

	mem.SClear(mem.hS2);

	mem.SPrepare(mem.hS2, point::RotationPointer, sizeof(FNRot));
	mem.SPrepare(mem.hS2, point::LocationPointer, sizeof(Vector3));
	mem.SPrepare(mem.hS2, point::LocalPlayer + offsets::FieldOfView, sizeof(float));

	// while we are at it update seconds for vischeck
	mem.SPrepare(mem.hS2, point::Uworld + offsets::Seconds, sizeof(double));

	// and location under reticle
	if (point::PlayerController)
		mem.SPrepare(mem.hS2, point::PlayerController + offsets::LocationUnderReticle, sizeof(Vector3));

	mem.ExecuteMemoryReads(mem.hS2);

	rotation = mem.SReads<FNRot>(mem.hS2, point::RotationPointer);

	mainCamera.Location = mem.SReads<Vector3>(mem.hS2, point::LocationPointer);

	mainCamera.FieldOfView = mem.SReads<float>(mem.hS2, point::LocalPlayer + offsets::FieldOfView);

	point::Seconds = mem.SReads<double>(mem.hS2, point::Uworld + offsets::Seconds);

	if (point::PlayerController)
		mainCamera.LocationUnderReticle = mem.SReads<Vector3>(mem.hS2, point::PlayerController + offsets::LocationUnderReticle);

	// fix rotation ?
	mainCamera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	mainCamera.Rotation.y = atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI);

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updateCameraData.addValue(static_cast<float>(elapsed));
}

void updatePlayerList()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::PlayerList) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}

	// check if we can update players
	if (!point::Uworld && !point::GameState && !point::PlayerArray.data && point::PlayerArray.count > 0 && point::PlayerArray.count < 150) {
		__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
		return;
	}

	// we will save the data used for this update (so they dont change while we are using them)
	tarray playerArray = point::PlayerArray;

	std::unique_ptr<uintptr_t[]> players(new uintptr_t[playerArray.count]); // this is an array of player states

	// read the whole player list
	mem.ReadA(playerArray.data, players.get(), playerArray.count * sizeof(uintptr_t));

	// clean cache off of people that dont exist anymore
	auto it = mainPlayerList.begin();
	while (it != mainPlayerList.end()) {
		//PlayerCache cachedPlayer = it->second;
		bool found = false;
		for (int i = 0; i < playerArray.count; i++) {
			if (players[i] == it->first) {
				found = true;
				break;
			}
		}
		if (!found) {
			it = mainPlayerList.erase(it);
		}
		else {
			++it;
		}
	}


	// add people that havent been cached yet
	std::vector<PlayerCache> playersToAdd;
	for (int i = 0; i < playerArray.count; i++) {
		if (!players[i])
			continue;
		auto it = mainPlayerList.find(players[i]);
		if (it == mainPlayerList.end()) {
			PlayerCache newPlayer;
			newPlayer.PlayerState = players[i];
			playersToAdd.push_back(newPlayer);
		}
	}

	// are there new players to add?
	if (playersToAdd.size() > 0) {
		// start looping over the new players to prepare data
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId, sizeof(uint32_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot, sizeof(BYTE));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// read the prepared data
		for (int i = 0; i < playersToAdd.size(); i++) {
			playersToAdd[i].Pawn = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate);
			playersToAdd[i].TeamId = mem.SReads<uint32_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId);
			playersToAdd[i].isBot = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot) & 0x08;
		}


		// now that we have the pawn we can read more so prepare
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].Pawn)
				continue;
			mem.SPrepare(mem.hS3, playersToAdd[i].Pawn + offsets::Mesh, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].Pawn + offsets::RootComponent, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].Pawn + offsets::isDying, sizeof(BYTE));
			mem.SPrepare(mem.hS3, playersToAdd[i].Pawn + offsets::IsDBNO, sizeof(BYTE));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// and read
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].Pawn)
				continue;
			playersToAdd[i].Mesh = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].Pawn + offsets::Mesh);
			playersToAdd[i].RootComponent = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].Pawn + offsets::RootComponent);
			playersToAdd[i].isDying = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].Pawn + offsets::isDying) & 0x20;
			playersToAdd[i].isDBNO = (mem.SReads<BYTE>(mem.hS3, playersToAdd[i].Pawn + offsets::IsDBNO) >> 6) & 1;
		}


		// again, now we have the mesh and root, prepare
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].Mesh || !playersToAdd[i].RootComponent)
				continue;
			mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray, 0x18); // includes both
			//mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray + 0x10, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::ComponentToWorld, sizeof(FTransform));
			mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::LastRenderTime, sizeof(float));
			mem.SPrepare(mem.hS3, playersToAdd[i].RootComponent + offsets::Velocity, sizeof(Vector3));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// and read
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].Mesh || !playersToAdd[i].RootComponent)
				continue;
			playersToAdd[i].BoneArray1 = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray);
			playersToAdd[i].BoneArray2 = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray + 0x10);
			it->second.BoneArray = it->second.BoneArray1;
			if (!it->second.BoneArray) it->second.BoneArray = it->second.BoneArray2;
			playersToAdd[i].component_to_world = mem.SReads<FTransform>(mem.hS3, playersToAdd[i].Mesh + offsets::ComponentToWorld);
			playersToAdd[i].last_render = mem.SReads<float>(mem.hS3, playersToAdd[i].Mesh + offsets::LastRenderTime);
			playersToAdd[i].Velocity = mem.SReads<Vector3>(mem.hS3, playersToAdd[i].RootComponent + offsets::Velocity);
		}

		// not sure if this will be kept here, prepare
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {

			if (!it->second.BoneArray)
				continue;

			mem.SPrepare(mem.hS3, it->second.BoneArray, (82 * 0x60) + sizeof(FTransform));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// and read
		for (int i = 0; i < playersToAdd.size(); i++) {
			// same thing
			if (!it->second.BoneArray)
				continue;

			playersToAdd[i].HeadBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (68 * 0x60));
			playersToAdd[i].BottomBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (0 * 0x60));
			playersToAdd[i].NeckBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (66 * 0x60));
			playersToAdd[i].HipBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (2 * 0x60));
			playersToAdd[i].UpperArmLeftBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (9 * 0x60));
			playersToAdd[i].UpperArmRightBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (38 * 0x60));
			playersToAdd[i].LeftHandBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (10 * 0x60));
			playersToAdd[i].RightHandBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (39 * 0x60));
			playersToAdd[i].LeftHandTBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (11 * 0x60));
			playersToAdd[i].RightHandTBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (40 * 0x60));
			playersToAdd[i].RightThighBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (78 * 0x60));
			playersToAdd[i].LeftThighBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (71 * 0x60));
			playersToAdd[i].RightCalfBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (79 * 0x60));
			playersToAdd[i].LeftCalfBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (72 * 0x60));
			playersToAdd[i].LeftFootBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (75 * 0x60));
			playersToAdd[i].RightFootBone = mem.SReads<FTransform>(mem.hS3, it->second.BoneArray + (82 * 0x60));

			// do all the players w2s and calculate with matrix the bone positions
			{
				// get bones world position
				playersToAdd[i].Head3D = CalcMatrix(playersToAdd[i].HeadBone, playersToAdd[i].component_to_world);
				playersToAdd[i].Bottom3D = CalcMatrix(playersToAdd[i].BottomBone, playersToAdd[i].component_to_world);

				playersToAdd[i].Hip3D  = CalcMatrix(playersToAdd[i].HipBone, playersToAdd[i].component_to_world);
				playersToAdd[i].Neck3D  = CalcMatrix(playersToAdd[i].NeckBone, playersToAdd[i].component_to_world);
				playersToAdd[i].UpperArmLeft3D  = CalcMatrix(playersToAdd[i].UpperArmLeftBone, playersToAdd[i].component_to_world);
				playersToAdd[i].UpperArmRight3D  = CalcMatrix(playersToAdd[i].UpperArmRightBone, playersToAdd[i].component_to_world);
				playersToAdd[i].LeftHand3D  = CalcMatrix(playersToAdd[i].LeftHandBone, playersToAdd[i].component_to_world);
				playersToAdd[i].RightHand3D  = CalcMatrix(playersToAdd[i].RightHandBone, playersToAdd[i].component_to_world);
				playersToAdd[i].LeftHandT3D  = CalcMatrix(playersToAdd[i].LeftHandTBone, playersToAdd[i].component_to_world);
				playersToAdd[i].RightHandT3D  = CalcMatrix(playersToAdd[i].RightHandTBone, playersToAdd[i].component_to_world);
				playersToAdd[i].RightThigh3D  = CalcMatrix(playersToAdd[i].RightThighBone, playersToAdd[i].component_to_world);
				playersToAdd[i].LeftThigh3D  = CalcMatrix(playersToAdd[i].LeftThighBone, playersToAdd[i].component_to_world);
				playersToAdd[i].RightCalf3D  = CalcMatrix(playersToAdd[i].RightCalfBone, playersToAdd[i].component_to_world);
				playersToAdd[i].LeftCalf3D  = CalcMatrix(playersToAdd[i].LeftCalfBone, playersToAdd[i].component_to_world);
				playersToAdd[i].LeftFoot3D  = CalcMatrix(playersToAdd[i].LeftFootBone, playersToAdd[i].component_to_world);
				playersToAdd[i].RightFoot3D = CalcMatrix(playersToAdd[i].RightFootBone, playersToAdd[i].component_to_world);

				// and do world to screen
				playersToAdd[i].Head2D = w2s(playersToAdd[i].Head3D);
				playersToAdd[i].Bottom2D = w2s(playersToAdd[i].Bottom3D);

				playersToAdd[i].Hip2D = w2s(playersToAdd[i].Hip3D);
				playersToAdd[i].Neck2D = w2s(playersToAdd[i].Neck3D);
				playersToAdd[i].UpperArmLeft2D = w2s(playersToAdd[i].UpperArmLeft3D);
				playersToAdd[i].UpperArmRight2D = w2s(playersToAdd[i].UpperArmRight3D);
				playersToAdd[i].LeftHand2D = w2s(playersToAdd[i].LeftHand3D);
				playersToAdd[i].RightHand2D = w2s(playersToAdd[i].RightHand3D);
				playersToAdd[i].LeftHandT2D = w2s(playersToAdd[i].LeftHandT3D);
				playersToAdd[i].RightHandT2D = w2s(playersToAdd[i].RightHandT3D);
				playersToAdd[i].RightThigh2D = w2s(playersToAdd[i].RightThigh3D);
				playersToAdd[i].LeftThigh2D = w2s(playersToAdd[i].LeftThigh3D);
				playersToAdd[i].RightCalf2D = w2s(playersToAdd[i].RightCalf3D);
				playersToAdd[i].LeftCalf2D = w2s(playersToAdd[i].LeftCalf3D);
				playersToAdd[i].LeftFoot2D = w2s(playersToAdd[i].LeftFoot3D);
				playersToAdd[i].RightFoot2D = w2s(playersToAdd[i].RightFoot3D);
			}
		}

		// now that we have everything we just add them to the cache
		for (int i = 0; i < playersToAdd.size(); i++) {
			mainPlayerList[playersToAdd[i].PlayerState] = playersToAdd[i];
		}
	}

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayerListData.addValue(static_cast<float>(elapsed));
}

#include <execution>

void updatePlayers()
{
	auto start = std::chrono::high_resolution_clock::now();

	if (!settings::cheat::Players) {
		goto END;
	}

	// are there any players
	if (mainPlayerList.size() > 0) {
		mem.SClear(mem.hS4);

		// prepare all the reads in parallel
		std::for_each(std::execution::par, mainPlayerList.begin(), mainPlayerList.end(), [&](auto& it) {
			if (it.second.BoneArray) {
				mem.SPrepare(mem.hS4, it.second.BoneArray, (82 * 0x60) + sizeof(FTransform)); // 82
			}
			if (it.second.PlayerState) {
				mem.SPrepare(mem.hS4, it.second.PlayerState + offsets::PawnPrivate, sizeof(uintptr_t));
				mem.SPrepare(mem.hS4, it.second.PlayerState + offsets::TeamId, sizeof(uint32_t));
			}
			if (it.second.Pawn) {
				mem.SPrepare(mem.hS4, it.second.Pawn + offsets::Mesh, sizeof(uintptr_t));
				mem.SPrepare(mem.hS4, it.second.Pawn + offsets::RootComponent, sizeof(uintptr_t));
				mem.SPrepare(mem.hS4, it.second.Pawn + offsets::isDying, sizeof(BYTE));
				mem.SPrepare(mem.hS4, it.second.Pawn + offsets::IsDBNO, sizeof(BYTE));
			}
			if (it.second.Mesh) {
				mem.SPrepare(mem.hS4, it.second.Mesh + offsets::BoneArray, 0x18);
				mem.SPrepare(mem.hS4, it.second.Mesh + offsets::ComponentToWorld, sizeof(FTransform));
				mem.SPrepare(mem.hS4, it.second.Mesh + offsets::LastRenderTime, sizeof(float));
			}
			if (it.second.RootComponent) {
				mem.SPrepare(mem.hS4, it.second.RootComponent + offsets::Velocity, sizeof(Vector3));
			}
		});

		auto start1 = std::chrono::high_resolution_clock::now();
		// EXECUTE AHAHAHAHHAHAH
		mem.ExecuteMemoryReads(mem.hS4);
		__int64 elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

		// all reads in parallel
		std::for_each(std::execution::par, mainPlayerList.begin(), mainPlayerList.end(), [&](auto& it) {

			it.second.lastUpdate = chrono::system_clock::now();

			if (it.second.BoneArray) {
				it.second.HeadBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (68 * 0x60), it.second.HeadBone);
				it.second.BottomBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (0 * 0x60), it.second.BottomBone);
				it.second.NeckBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (66 * 0x60), it.second.NeckBone);
				it.second.HipBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (2 * 0x60), it.second.HipBone);
				it.second.UpperArmLeftBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (9 * 0x60), it.second.UpperArmLeftBone);
				it.second.UpperArmRightBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (38 * 0x60), it.second.UpperArmRightBone);
				it.second.LeftHandBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (10 * 0x60), it.second.LeftHandBone);
				it.second.RightHandBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (39 * 0x60), it.second.RightHandBone);
				it.second.LeftHandTBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (11 * 0x60), it.second.LeftHandTBone);
				it.second.RightHandTBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (40 * 0x60), it.second.RightHandTBone);
				it.second.RightThighBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (78 * 0x60), it.second.RightThighBone);
				it.second.LeftThighBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (71 * 0x60), it.second.LeftThighBone);
				it.second.RightCalfBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (79 * 0x60), it.second.RightCalfBone);
				it.second.LeftCalfBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (72 * 0x60), it.second.LeftCalfBone);
				it.second.LeftFootBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (75 * 0x60), it.second.LeftFootBone);
				it.second.RightFootBone = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.BoneArray + (82 * 0x60), it.second.RightFootBone);
			}

			if (it.second.PlayerState) {
				it.second.Pawn = mem.SReadsSuccess<uintptr_t>(mem.hS4, it.second.PlayerState + offsets::PawnPrivate, it.second.Pawn);
				it.second.TeamId = mem.SReadsSuccess<uint32_t>(mem.hS4, it.second.PlayerState + offsets::TeamId, it.second.TeamId);
			}

			if (it.second.Pawn) {
				it.second.Mesh = mem.SReadsSuccess<uintptr_t>(mem.hS4, it.second.Pawn + offsets::Mesh, it.second.Mesh);
				it.second.RootComponent = mem.SReadsSuccess<uintptr_t>(mem.hS4, it.second.Pawn + offsets::RootComponent, it.second.RootComponent);
				it.second.isDying = mem.SReadsSuccess<BYTE>(mem.hS4, it.second.Pawn + offsets::isDying, it.second.isDying) & 0x20;
				it.second.isDBNO = (mem.SReadsSuccess<BYTE>(mem.hS4, it.second.Pawn + offsets::IsDBNO, it.second.isDBNO) >> 6) & 1;
			}

			if (it.second.Mesh) {
				it.second.BoneArray1 = mem.SReadsSuccess<uintptr_t>(mem.hS4, it.second.Mesh + offsets::BoneArray, it.second.BoneArray1);
				it.second.BoneArray2 = mem.SReadsSuccess<uintptr_t>(mem.hS4, it.second.Mesh + offsets::BoneArray + 0x10, it.second.BoneArray2);
				it.second.BoneArray = it.second.BoneArray1;
				if (!it.second.BoneArray) it.second.BoneArray = it.second.BoneArray2;
				it.second.component_to_world = mem.SReadsSuccess<FTransform>(mem.hS4, it.second.Mesh + offsets::ComponentToWorld, it.second.component_to_world);
				it.second.last_render = mem.SReadsSuccess<float>(mem.hS4, it.second.Mesh + offsets::LastRenderTime, it.second.last_render);
			}

			if (it.second.RootComponent) {
				it.second.Velocity = mem.SReadsSuccess<Vector3>(mem.hS4, it.second.RootComponent + offsets::Velocity, it.second.Velocity);
			}

			// while we are at it do all the players w2s and calculate with matrix the bone positions
			if (it.second.BoneArray)
			{
				// get bones world position
				it.second.Head3D = CalcMatrix(it.second.HeadBone, it.second.component_to_world);
				it.second.Bottom3D = CalcMatrix(it.second.BottomBone, it.second.component_to_world);

				it.second.Hip3D  = CalcMatrix(it.second.HipBone, it.second.component_to_world);
				it.second.Neck3D  = CalcMatrix(it.second.NeckBone, it.second.component_to_world);
				it.second.UpperArmLeft3D  = CalcMatrix(it.second.UpperArmLeftBone, it.second.component_to_world);
				it.second.UpperArmRight3D  = CalcMatrix(it.second.UpperArmRightBone, it.second.component_to_world);
				it.second.LeftHand3D  = CalcMatrix(it.second.LeftHandBone, it.second.component_to_world);
				it.second.RightHand3D  = CalcMatrix(it.second.RightHandBone, it.second.component_to_world);
				it.second.LeftHandT3D  = CalcMatrix(it.second.LeftHandTBone, it.second.component_to_world);
				it.second.RightHandT3D  = CalcMatrix(it.second.RightHandTBone, it.second.component_to_world);
				it.second.RightThigh3D  = CalcMatrix(it.second.RightThighBone, it.second.component_to_world);
				it.second.LeftThigh3D  = CalcMatrix(it.second.LeftThighBone, it.second.component_to_world);
				it.second.RightCalf3D  = CalcMatrix(it.second.RightCalfBone, it.second.component_to_world);
				it.second.LeftCalf3D  = CalcMatrix(it.second.LeftCalfBone, it.second.component_to_world);
				it.second.LeftFoot3D  = CalcMatrix(it.second.LeftFootBone, it.second.component_to_world);
				it.second.RightFoot3D = CalcMatrix(it.second.RightFootBone, it.second.component_to_world);

				// and do world to screen
				it.second.Head2D = w2s(it.second.Head3D);

				it.second.Top2D = w2s(Vector3(it.second.Head3D.x, it.second.Head3D.y, it.second.Head3D.z + 15.f));
				it.second.Bottom2D = w2s(it.second.Bottom3D);

				it.second.Hip2D = w2s(it.second.Hip3D);
				it.second.Neck2D = w2s(it.second.Neck3D);
				it.second.UpperArmLeft2D = w2s(it.second.UpperArmLeft3D);
				it.second.UpperArmRight2D = w2s(it.second.UpperArmRight3D);
				it.second.LeftHand2D = w2s(it.second.LeftHand3D);
				it.second.RightHand2D = w2s(it.second.RightHand3D);
				it.second.LeftHandT2D = w2s(it.second.LeftHandT3D);
				it.second.RightHandT2D = w2s(it.second.RightHandT3D);
				it.second.RightThigh2D = w2s(it.second.RightThigh3D);
				it.second.LeftThigh2D = w2s(it.second.LeftThigh3D);
				it.second.RightCalf2D = w2s(it.second.RightCalf3D);
				it.second.LeftCalf2D = w2s(it.second.LeftCalf3D);
				it.second.LeftFoot2D = w2s(it.second.LeftFoot3D);
				it.second.RightFoot2D = w2s(it.second.RightFoot3D);
			}

		});

	}

	secondPlayerList = mainPlayerList;

END:

	__int64 elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
	stats::updatePlayersData.addValue(static_cast<float>(elapsed));
}

void weaponUpdate()
{
	// features that need the weapon updated
	if (!settings::config::Aimbot || !settings::config::TriggerBot)
		return;

	mem.SClear(mem.hS5);

	if (point::Player)
		mem.SPrepare(mem.hS5, point::Player + offsets::CurrentWeapon, sizeof(uintptr_t));

	if (point::CurrentWeapon){
		mem.SPrepare(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileSpeed, sizeof(float));
		mem.SPrepare(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileGravity, sizeof(float));
	}

	mem.ExecuteMemoryReads(mem.hS5);

	float projectileSpeed = 0;

	if (point::Player)
		point::CurrentWeapon = mem.SReads<uintptr_t>(mem.hS5, point::Player + offsets::CurrentWeapon);

	if (point::Player && point::CurrentWeapon){
		point::ProjectileSpeed = mem.SReads<float>(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileSpeed) / settings::config::PredictionMultiplier;
		point::ProjectileGravity = mem.SReads<float>(mem.hS5, point::CurrentWeapon + offsets::WeaponProjectileGravity);
	}
	else {
		point::ProjectileSpeed = 0;
		point::ProjectileGravity = 0;
	}
}


void healthChecks() {
	
	// future

}