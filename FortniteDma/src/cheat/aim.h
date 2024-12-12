
#include "..\kmbox\kmbox.hpp"
#include "..\settings.h"

namespace aim {

	Vector3 predictLocation(Vector3 target, Vector3 target_velocity, float projectile_speed, float projectile_gravity_scale, float distance)
	{
		if (projectile_speed == 0)
		{
			return target;
		}
		float horizontalTime = distance / projectile_speed;
		float verticalTime = distance / projectile_speed;

		target.x += target_velocity.x * horizontalTime;
		target.y += target_velocity.y * horizontalTime;
		target.z += target_velocity.z * verticalTime + abs(-980 * projectile_gravity_scale) * 0.5f * (verticalTime * verticalTime);

		return target;
	}

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
		if (!point::Player || !point::PlayerState)
			return;

		static PlayerCache target{};

		bool Targetting = false;
		bool ValidTarget = true;

		if (mem.IsKeyDown(settings::config::AimKey)) {
			std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;

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

			Vector3 target2D = closestPlayer.Head2D;
			if (settings::config::Prediction) {
				if (point::ProjectileSpeed != 0) {
					target2D = w2s(predictLocation(closestPlayer.Head3D, closestPlayer.Velocity, point::ProjectileSpeed, point::ProjectileGravity, (float)closestPlayer.Head3D.Distance(mainCamera.Location)));
				}
			}

			float screen_center_x = settings::window::Width / 2;
			float screen_center_y = settings::window::Height / 2;
			float TargetX = 0;
			float TargetY = 0;

			if (target2D.x > screen_center_x)
			{
				TargetX = -(screen_center_x - target2D.x);
			}
			else if (target2D.x < screen_center_x)
			{
				TargetX = target2D.x - screen_center_x;
			}

			if (target2D.y > screen_center_y)
			{
				TargetY = -(screen_center_y - target2D.y);
			}
			else if (target2D.y < screen_center_y)
			{
				TargetY = target2D.y - screen_center_y;
			}

			ImVec2 Angles(TargetX / settings::config::AimSmoothing, TargetY / settings::config::AimSmoothing);

			target = closestPlayer;
			if (settings::kmbox::NetKmbox) {
				kmNet_mouse_move_auto(Angles.x, Angles.y, 3);
			}
			else {
				km_move(Angles.x, Angles.y);
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
			std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;

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