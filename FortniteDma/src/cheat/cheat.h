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
	point::Uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);

	if (!point::Uworld) return;

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

	if (!point::GameState) return;

	if (point::GameInstance) point::LocalPlayers = mem.Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);

	if (!point::LocalPlayers) return;

	point::LocalPlayer = mem.Read<uintptr_t>(point::LocalPlayers);

	if (!point::LocalPlayer) return;

	point::PlayerController = mem.Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);


	if (point::PlayerController) point::Player = mem.Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

	if (point::PlayerController) point::PlayerCameraManager = mem.Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

	if (point::Player) point::PlayerState = mem.Read<uintptr_t>(point::Player + offsets::PlayerState);

	point::PlayerArray = mem.Read<tarray>(point::GameState + offsets::PlayerArray);

	if (point::LocationPointer) local_player::localPos = mem.Read<Vector3>(point::LocationPointer);

	if (point::PlayerState) local_player::localTeam = mem.Read<uint32_t>(point::PlayerState + offsets::TeamId);
}

void updateCamera()
{
	// check if we can update the camera
	if (!point::RotationPointer || !point::LocationPointer || !point::LocalPlayer)
		return;

	FNRot rotation;

	mem.SClear(mem.hS2);

	mem.SPrepare(mem.hS2, point::RotationPointer, sizeof(FNRot));
	mem.SPrepare(mem.hS2, point::LocationPointer, sizeof(Vector3));
	mem.SPrepare(mem.hS2, point::LocalPlayer + offsets::FieldOfView, sizeof(float));

	// while we are at it update seconds for vischeck
	mem.SPrepare(mem.hS2, point::Uworld + offsets::Seconds, sizeof(double));

	mem.ExecuteMemoryReads(mem.hS2);

	rotation = mem.SReads<FNRot>(mem.hS2, point::RotationPointer);

	mainCamera.Location = mem.SReads<Vector3>(mem.hS2, point::LocationPointer);

	mainCamera.FieldOfView = mem.SReads<float>(mem.hS2, point::LocalPlayer + offsets::FieldOfView);

	point::Seconds = mem.SReads<double>(mem.hS2, point::Uworld + offsets::Seconds);

	// fix rotation ?
	mainCamera.Rotation.x = asin(rotation.c) * (180.0 / M_PI);
	mainCamera.Rotation.y = atan2(rotation.a * -1, rotation.b) * (180.0 / M_PI);
}

void updatePlayerList()
{
	// check if we can update players
	if (!point::Uworld && !point::GameState && !point::PlayerArray.data)
		return;

	// we will save the data used for this update (so they dont change while we are using them)
	tarray playerArray = point::PlayerArray;

	std::unique_ptr<uintptr_t[]> players(new uintptr_t[playerArray.count]); // this is an array of player states

	// read the whole player list
	mem.ReadA(playerArray.data, players.get(), playerArray.count * sizeof(uintptr_t));

	// clean cache off of people that dont exist anymore
	auto it = newCache.begin();
	while (it != newCache.end()) {
		//PlayerCache cachedPlayer = it->second;
		bool found = false;
		for (int i = 0; i < playerArray.count; i++) {
			if (players[i] == it->first) {
				found = true;
				break;
			}
		}
		if (!found) {
			it = newCache.erase(it);
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
		auto it = newCache.find(players[i]);
		if (it == newCache.end()) {
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
			if (!playersToAdd[i].PlayerState)
				continue;
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId, sizeof(uint32_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot, sizeof(BYTE));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// read the prepared data
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].PlayerState)
				continue;
			playersToAdd[i].Pawn = mem.SReads<uintptr_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::PawnPrivate);
			playersToAdd[i].TeamId = mem.SReads<uint32_t>(mem.hS3, playersToAdd[i].PlayerState + offsets::TeamId);
			playersToAdd[i].isBot = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].PlayerState + offsets::bIsABot);
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
			playersToAdd[i].isDying = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].Pawn + offsets::isDying);
			playersToAdd[i].isDBNO = mem.SReads<BYTE>(mem.hS3, playersToAdd[i].Pawn + offsets::IsDBNO);
		}


		// again, now we have the mesh and root, prepare
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			if (!playersToAdd[i].Mesh || !playersToAdd[i].RootComponent)
				continue;
			mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray, 0x18); // includes both
			//mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::BoneArray + 0x10, sizeof(uintptr_t));
			mem.SPrepare(mem.hS3, playersToAdd[i].Mesh + offsets::ComponetToWorld, sizeof(FTransform));
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
			playersToAdd[i].component_to_world = mem.SReads<FTransform>(mem.hS3, playersToAdd[i].Mesh + offsets::ComponetToWorld);
			playersToAdd[i].last_render = mem.SReads<float>(mem.hS3, playersToAdd[i].Mesh + offsets::LastRenderTime);
			playersToAdd[i].Velocity = mem.SReads<Vector3>(mem.hS3, playersToAdd[i].RootComponent + offsets::Velocity);
		}

		// not sure if this will be kept here, prepare
		mem.SClear(mem.hS3);
		for (int i = 0; i < playersToAdd.size(); i++) {
			// lets use the good bone array
			uintptr_t BoneArray = playersToAdd[i].BoneArray1;
			if (!BoneArray) BoneArray = playersToAdd[i].BoneArray2;

			if (!BoneArray)
				continue;

			mem.SPrepare(mem.hS3, BoneArray, (82 * 0x60) + sizeof(FTransform));
		}
		mem.ExecuteMemoryReads(mem.hS3);
		// and read
		for (int i = 0; i < playersToAdd.size(); i++) {
			// same thing
			uintptr_t BoneArray = playersToAdd[i].BoneArray1;
			if (!BoneArray) BoneArray = playersToAdd[i].BoneArray2;

			if (!BoneArray)
				continue;

			playersToAdd[i].HeadBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (68 * 0x60));
			playersToAdd[i].BottomBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (0 * 0x60));
			playersToAdd[i].NeckBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (66 * 0x60));
			playersToAdd[i].HipBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (2 * 0x60));
			playersToAdd[i].UpperArmLeftBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (9 * 0x60));
			playersToAdd[i].UpperArmRightBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (38 * 0x60));
			playersToAdd[i].LeftHandBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (10 * 0x60));
			playersToAdd[i].RightHandBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (39 * 0x60));
			playersToAdd[i].LeftHand1Bone = mem.SReads<FTransform>(mem.hS3, BoneArray + (11 * 0x60));
			playersToAdd[i].RightHand1Bone = mem.SReads<FTransform>(mem.hS3, BoneArray + (40 * 0x60));
			playersToAdd[i].RightThighBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (78 * 0x60));
			playersToAdd[i].LeftThighBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (71 * 0x60));
			playersToAdd[i].RightCalfBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (79 * 0x60));
			playersToAdd[i].LeftCalfBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (72 * 0x60));
			playersToAdd[i].LeftFootBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (75 * 0x60));
			playersToAdd[i].RightFootBone = mem.SReads<FTransform>(mem.hS3, BoneArray + (82 * 0x60));
		}

		// now that we have everything we just add them to the cache
		for (int i = 0; i < playersToAdd.size(); i++) {
			newCache[playersToAdd[i].PlayerState] = playersToAdd[i];
		}
	}
}

void updatePlayers()
{
	// are there any players
	if (newCache.size() > 0) {
		mem.SClear(mem.hS4);
		// prepare all the reads
		auto it = newCache.begin();
		while (it != newCache.end()) {
			uintptr_t BoneArray = it->second.BoneArray1;
			if (!BoneArray) BoneArray = it->second.BoneArray2;

			
			if (BoneArray) {
				mem.SPrepare(mem.hS4, BoneArray, (82 * 0x60) + sizeof(FTransform));
			}

			if (it->second.Pawn) {
				mem.SPrepare(mem.hS4, it->second.Pawn + offsets::Mesh, sizeof(uintptr_t));
			}

			if (it->second.Mesh) {
				mem.SPrepare(mem.hS4, it->second.Mesh + offsets::BoneArray, 0x18); // includes both
				//mem.SPrepare(mem.hS3, it->second.Mesh + offsets::BoneArray + 0x10, sizeof(uintptr_t));
				mem.SPrepare(mem.hS4, it->second.Mesh + offsets::ComponetToWorld, sizeof(FTransform));
				mem.SPrepare(mem.hS4, it->second.Mesh + offsets::LastRenderTime, sizeof(float));
			}

			if (it->second.RootComponent) {
				mem.SPrepare(mem.hS4, it->second.RootComponent + offsets::Velocity, sizeof(Vector3));
			}

			if (it->second.Pawn) {
				mem.SPrepare(mem.hS4, it->second.Pawn + offsets::isDying, sizeof(BYTE));
				mem.SPrepare(mem.hS4, it->second.Pawn + offsets::IsDBNO, sizeof(BYTE));
			}

			++it;
		}

		// EXECUTE AHAHAHAHHAHAH
		mem.ExecuteMemoryReads(mem.hS4);

		it = newCache.begin();
		while (it != newCache.end()) {
			uintptr_t BoneArray = it->second.BoneArray1;
			if (!BoneArray) BoneArray = it->second.BoneArray2;

			if (BoneArray) {
				it->second.HeadBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (68 * 0x60));
				it->second.BottomBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (0 * 0x60));
				it->second.NeckBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (66 * 0x60));
				it->second.HipBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (2 * 0x60));
				it->second.UpperArmLeftBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (9 * 0x60));
				it->second.UpperArmRightBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (38 * 0x60));
				it->second.LeftHandBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (10 * 0x60));
				it->second.RightHandBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (39 * 0x60));
				it->second.LeftHand1Bone = mem.SReads<FTransform>(mem.hS4, BoneArray + (11 * 0x60));
				it->second.RightHand1Bone = mem.SReads<FTransform>(mem.hS4, BoneArray + (40 * 0x60));
				it->second.RightThighBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (78 * 0x60));
				it->second.LeftThighBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (71 * 0x60));
				it->second.RightCalfBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (79 * 0x60));
				it->second.LeftCalfBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (72 * 0x60));
				it->second.LeftFootBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (75 * 0x60));
				it->second.RightFootBone = mem.SReads<FTransform>(mem.hS4, BoneArray + (82 * 0x60));
			}

			if (it->second.Pawn) {
				it->second.Mesh = mem.SReads<uintptr_t>(mem.hS3, it->second.Pawn + offsets::Mesh);
			}

			if (it->second.Mesh) {
				it->second.BoneArray1 = mem.SReads<uintptr_t>(mem.hS4, it->second.Mesh + offsets::BoneArray);
				it->second.BoneArray2 = mem.SReads<uintptr_t>(mem.hS4, it->second.Mesh + offsets::BoneArray + 0x10);
				it->second.component_to_world = mem.SReads<FTransform>(mem.hS4, it->second.Mesh + offsets::ComponetToWorld);
				it->second.last_render = mem.SReads<float>(mem.hS4, it->second.Mesh + offsets::LastRenderTime);
			}

			if (it->second.RootComponent) {
				it->second.Velocity = mem.SReads<Vector3>(mem.hS4, it->second.RootComponent + offsets::Velocity);
			}

			if (it->second.Pawn) {
				it->second.isDying = mem.SReads<BYTE>(mem.hS4, it->second.Pawn + offsets::isDying);
				it->second.isDBNO = mem.SReads<BYTE>(mem.hS4, it->second.Pawn + offsets::IsDBNO);
			}

			++it;
		}
	}

	readyCache = newCache;
}