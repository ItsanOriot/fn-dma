#pragma once

#include "definitions.h"

#include <unordered_map>

namespace local_player {
    inline  Vector3 localPos = Vector3(); // not used
    inline uint32_t localTeam = NULL;
}

inline Camera mainCamera;

inline std::unordered_map<uintptr_t, PlayerCache> newCache; // this cache is constantly updated by the memory thread

inline std::unordered_map<uintptr_t, PlayerCache> readyCache; // this cache is a copy of newCache that is always complete and not being worked on directly

namespace point {
    inline uintptr_t va_text = 0;
    inline uintptr_t Base = 0;
    inline uintptr_t Uworld = 0;
    inline uintptr_t Player = 0;
    inline uintptr_t PlayerState = 0;
    inline uintptr_t GameInstance = 0;
    inline uintptr_t PersistentLevel = 0;
    inline uintptr_t GameState = 0;
    inline uintptr_t Mesh = 0;
    inline uintptr_t RootComponent = 0;
    inline uintptr_t LocalPlayerState = 0;
    inline tarray PlayerArray{};
    inline uintptr_t ULevelArray = 0;
    inline uintptr_t ULevels = 0;
    inline uintptr_t AActorArray = 0;
    inline uintptr_t AActors = 0;
    inline uintptr_t LocalPlayers = 0;
    inline uintptr_t LocalPlayer = 0;
    inline uintptr_t PlayerController = 0;
    inline uintptr_t PlayerCameraManager = 0;
    inline uintptr_t LocationPointer = 0;
    inline uintptr_t RotationPointer = 0;

    // maybe move into main camera
    inline double Seconds = 0; // for vischeck
}