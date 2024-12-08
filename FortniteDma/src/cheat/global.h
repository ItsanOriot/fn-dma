#pragma once

#include "util.h"

#include <unordered_map>

namespace local_player {
    Vector3 localPos = Vector3();
    uint32_t localTeam = NULL;
}

std::unordered_map<uintptr_t, PlayerCache> newCache;

std::unordered_map<uintptr_t, PlayerCache> readyCache;

namespace point {
    uintptr_t va_text = 0;
    uintptr_t Base = 0;
    uintptr_t Uworld = 0;
    uintptr_t Player = 0;
    uintptr_t PlayerState = 0;
    uintptr_t GameInstance = 0;
    uintptr_t PersistentLevel = 0;
    uintptr_t GameState = 0;
    uintptr_t Mesh = 0;
    uintptr_t RootComponent = 0;
    uintptr_t LocalPlayerState = 0;
    tarray PlayerArray{};
    uintptr_t ULevelArray = 0;
    uintptr_t ULevels = 0;
    uintptr_t AActorArray = 0;
    uintptr_t AActors = 0;
    uintptr_t LocalPlayers = 0;
    uintptr_t LocalPlayer = 0;
    uintptr_t PlayerController = 0;
    uintptr_t PlayerCameraManager = 0;
    uintptr_t LocationPointer = 0;
    uintptr_t RotationPointer = 0;
}