#pragma once

#include "global.h"
#include "offsets.h"

bool update_va_text() {
	for (int i = 0, e = 0; i < INT_MAX && e < 3; i++) {
		point::va_text = point::Base + i * 0x1000;
		std::cout << hue::green << "[info] " << hue::white << "trying text_va -> 0x" << std::hex << point::va_text << std::endl;
		auto uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);
		auto level = mem.Read<uintptr_t>(uworld + offsets::PersistentLevel);
		if (uworld && level && mem.Read<uintptr_t>(level + offsets::OwningWorld) == uworld) {
			std::cout << hue::green << "[info] " << hue::white << "text_va found -> 0x" << std::hex << point::va_text << std::endl;
			return true;
		}
		if (i > 1000) { i = 0; e++; };
	}
	return false;
}

void newInfo()
{

	point::Uworld = mem.Read<uintptr_t>(point::va_text + offsets::Uworld);

	std::cout << "point::Uworld: 0x" << std::hex << point::Uworld << std::endl;

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


	std::cout << "point::GameInstance: 0x" << std::hex << point::GameInstance << std::endl;
	std::cout << "point::PersistentLevel: 0x" << std::hex << point::PersistentLevel << std::endl;
	std::cout << "point::GameState: 0x" << std::hex << point::GameState << std::endl;


	if (!point::GameInstance) return;

	if (!point::PersistentLevel) return;

	if (!point::GameState) return;

	point::LocalPlayers = mem.Read<uintptr_t>(point::GameInstance + offsets::LocalPlayers);

	std::cout << "point::LocalPlayers: 0x" << std::hex << point::LocalPlayers << std::endl;

	if (!point::LocalPlayers) return;

	point::LocalPlayer = mem.Read<uintptr_t>(point::LocalPlayers);

	std::cout << "point::LocalPlayer: 0x" << std::hex << point::LocalPlayer << std::endl;

	if (!point::LocalPlayer) return;

	point::PlayerController = mem.Read<uintptr_t>(point::LocalPlayer + offsets::PlayerController);

	std::cout << "point::PlayerController: 0x" << std::hex << point::PlayerController << std::endl;


	if (point::PlayerController) point::Player = mem.Read<uintptr_t>(point::PlayerController + offsets::LocalPawn);

	if (point::PlayerController) point::PlayerCameraManager = mem.Read<uintptr_t>(point::PlayerController + offsets::Cameramanager);

	if (point::Player) point::PlayerState = mem.Read<uintptr_t>(point::Player + offsets::PlayerState);

	point::PlayerArray = mem.Read<tarray>(point::GameState + offsets::PlayerArray);

	std::cout << "point::PlayerArray: 0x" << std::hex << point::PlayerArray.data << std::endl;
	std::cout << "g_nPlayers: " << std::dec << point::PlayerArray.count << std::endl;


	if (point::LocationPointer) local_player::localPos = mem.Read<Vector3>(point::LocationPointer);

	if (point::PlayerState) local_player::localTeam = mem.Read<uint32_t>(point::PlayerState + offsets::TeamId);



}