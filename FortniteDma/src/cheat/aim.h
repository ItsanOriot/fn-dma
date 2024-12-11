
#include "..\kmbox\kmbox.hpp"
#include "..\settings.h"

namespace aim {

	bool isHit(Vector3 loc, double margin = 20) {
		if (mainCamera.LocationUnderReticle.x >= loc.x - margin && mainCamera.LocationUnderReticle.x <= loc.x + margin && mainCamera.LocationUnderReticle.y >= loc.y - margin && mainCamera.LocationUnderReticle.y <= loc.y + margin)
			return true;
		else
			return false;
	}

	void updateAimbot()
	{
		if (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox) 
			return;
		if (!settings::config::Aimbot) 
			return;

		// not in game / alive
		if (!point::Player)
			return;

		static PlayerCache target{};

		bool Targetting = false;
		bool ValidTarget = true;

		if (mem.IsKeyDown(settings::config::AimKey)) {
			std::unordered_map<uintptr_t, PlayerCache> PlayerList = mainPlayerList;

			if (target.PlayerState) {

				// check if player still exist
				auto it = mainPlayerList.find(target.PlayerState);
				if (it != mainPlayerList.end()) {
					// get lil bro
					target = it->second;

					// check if he's good
					double dist = abs(target.Head2D.x - settings::window::Width / 2) + abs(target.Head2D.y - settings::window::Height / 2);

					bool IsVis = point::Seconds - target.last_render <= 0.06f;

					if (target.isDying || !IsVis || dist > settings::config::AimFov)
						ValidTarget = false;

				}
				else {
					ValidTarget = false;
				}

				Targetting = true;
			}

			if (Targetting && !ValidTarget)
				return;
				

			double closest = HUGE;
			PlayerCache closestPlayer{};
			if (!Targetting) {
				bool closePlayerFound = false;
				for (auto it : PlayerList) {
					PlayerCache player = it.second;

					if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

					bool IsVis = point::Seconds - player.last_render <= 0.06f;

					if (player.isDying || !IsVis) continue;

					if (player.PlayerState == point::PlayerState) continue;

					if (player.TeamId == local_player::localTeam) continue;

					double distance2D = abs(player.Head2D.x - settings::window::Width / 2) + abs(player.Head2D.y - settings::window::Height / 2);

					if (distance2D < settings::config::AimFov) {
						if (distance2D < closest) {
							closest = abs(player.Head2D.x - settings::window::Width / 2) + abs(player.Head2D.y - settings::window::Height / 2);
							closestPlayer = player;
							closePlayerFound = true;
						}
					}
				}

				if (!closePlayerFound)
					return;
			}
			else {
				closestPlayer = target;
			}

			Vector2 moveDistance = { (closestPlayer.Head2D.x - settings::window::Width / 2) / settings::config::AimSmoothing, (closestPlayer.Head2D.y - settings::window::Height / 2) / settings::config::AimSmoothing };
			
			double maxMove = 20.0f;

			moveDistance.x = std::clamp(moveDistance.x, -maxMove, maxMove);
			moveDistance.y = std::clamp(moveDistance.y, -maxMove, maxMove);

			target = closestPlayer;
			if (settings::kmbox::NetKmbox) {
				kmNet_mouse_move_auto(moveDistance.x, moveDistance.y, 5);
			}
			else {
				km_move(moveDistance.x, moveDistance.y);
			}
		}
		else {
			target.PlayerState = 0;
			Targetting = false;
		}
	}

	void updateTriggerBot()
	{

		if (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox)
			return;

		if (!settings::config::Aimbot)
			return;

		static bool clicked = false;

		if (clicked) {
			kmNet_mouse_left(false);
			clicked = false;
		}

		static chrono::steady_clock::time_point lastClick = std::chrono::steady_clock::now();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastClick).count() < settings::config::TriggerDelay)
			return;

		if (mem.IsKeyDown(settings::config::TriggerKey)) {
			std::unordered_map<uintptr_t, PlayerCache> PlayerList = mainPlayerList;

			for (auto it : PlayerList) {
				PlayerCache player = it.second;

				if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

				bool IsVis = point::Seconds - player.last_render <= 0.06f;

				if (player.isDying || !IsVis) continue;

				if (player.PlayerState == point::PlayerState) continue;

				if (player.TeamId == local_player::localTeam) continue;

				if (isHit(player.Head3D)) {
					lastClick = std::chrono::steady_clock::now();
					if (settings::kmbox::NetKmbox) {
						kmNet_mouse_left(true);
						clicked = true;
					}
					else {
						km_click();
					}
				}

			}
		}

	}
}