#pragma once
#include <cstdint>

namespace offsets {
	uintptr_t temp1 = 0x0;
	uintptr_t temp2 = 0x0;

	uintptr_t Uworld = 0x170D9D88;
	uintptr_t CameraLocationPointer = 0x130;
	uintptr_t CameraRotationPointer = 0x140;
	uintptr_t GameInstance = 0x200;
	uintptr_t GameState = 0x188;
	uintptr_t PersistentLevel = 0x40;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t FieldOfView = 0x4AC; // point::LocalPlayer + 0x4AC
	uintptr_t PlayerController = 0x30;
	uintptr_t LocalPawn = 0x350;
	uintptr_t Cameramanager = 0x348;
	uintptr_t PlayerState = 0x2c8;
	uintptr_t PawnPrivate = 0x320;
	uintptr_t PlayerArray = 0x2c0;
	uintptr_t Mesh = 0x328;
	uintptr_t ComponetToWorld = 0x1E0;
	uintptr_t Seconds = 0x140; // UWorld + 0x140
	uintptr_t LastRenderTime = 0x30C;
	uintptr_t BoneArray = 0x5A8;
	uintptr_t Velocity = 0x180;
	uintptr_t IsDBNO = 0x962;
	uintptr_t isDying = 0x718;
	uintptr_t bIsABot = 0x2b2;
	uintptr_t TeamId = 0x1261;
	uintptr_t RootComponent = 0x1b0;
	uintptr_t LocationUnderReticle = 0x2700;

	uintptr_t FNamePool = 0x12BA9140;
	uintptr_t FNameKey = 0x18;
	uintptr_t ItemDefinition = 0x368;
	uintptr_t OwningWorld = 0xC8;
	uintptr_t ULevelArray = 0x1a0;
	uintptr_t ULevelCount = ULevelArray + 8;
	uintptr_t AActorArray = 0x128;
	uintptr_t AActorCount = AActorArray + 8;
	uintptr_t Kills = 0x1274; //AFortPlayerStateAthena.KillScore: 0x1274
	uintptr_t PlayerWeapon = 0xA80;
	uintptr_t WeaponData = 0x568;
	uintptr_t TextPtr = 0x38;
	uintptr_t NameOffset = 0xAF0;
	uintptr_t ProjectileSpeed = 0x1D70;
	uintptr_t ProjectileGravity = 0x1DF0;
}