
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

	bool isHit2D(Vector3 loc, double margin = 20) {
		if (settings::window::Width / 2 >= loc.x - margin && settings::window::Width / 2 <= loc.x + margin && settings::window::Height / 2 >= loc.y - margin && settings::window::Height / 2 <= loc.y + margin)
			return true;
		else
			return false;
	}

	float computeMax(Vector3 loc, double marginStart, double marginEnd, float maxStart, float maxEnd) {
		// Calculate distance from the target to the reticle
		float distanceX = std::abs(settings::window::Width / 2 - loc.x);
		float distanceY = std::abs(settings::window::Width / 2 - loc.y);
		float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

		// Clamp the distance to the defined margin range
		float clampedDistance = std::clamp(distance, static_cast<float>(marginStart), static_cast<float>(marginEnd));

		// Normalize the clamped distance (0 at marginStart, 1 at marginEnd)
		float normalized = (clampedDistance - marginStart) / (marginEnd - marginStart);

		// Linearly interpolate `max` between maxStart and maxEnd based on normalized value
		return maxStart + normalized * (maxEnd - maxStart);
	}

	double isClose(Vector3 loc2D) {

		const double maxDistance = std::sqrt(std::pow(settings::window::Width, 2) + std::pow(settings::window::Height, 2)) / 2.0;
		
		double distance = std::sqrt(std::pow(loc2D.x - settings::window::Width / 2, 2) + std::pow(loc2D.y - settings::window::Height / 2, 2));
		
		double closeness = 1.0f - (distance / maxDistance);
		
		closeness = std::clamp(closeness, 0.0, 1.0);
		
		return closeness;
	}

	void updateAimbot()
	{
		if (!settings::runtime::hotKeys)
			return;
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
				auto it = secondPlayerList.find(target.PlayerState);
				if (it != secondPlayerList.end()) {
					// get lil bro
					target = it->second;

					// check if he's good
					double dist = std::sqrt(std::pow(target.Head2D.x - settings::window::Width / 2, 2) + std::pow(target.Head2D.y - settings::window::Height / 2, 2));

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

					double distance2D = std::sqrt(std::pow(player.Head2D.x - settings::window::Width / 2, 2) + std::pow(player.Head2D.y - settings::window::Height / 2, 2));

					if (distance2D < settings::config::AimFov) {
						if (distance2D < closest) {
							closest = distance2D;
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

			target = closestPlayer;

			Vector3 target3D = closestPlayer.Head3D;
			Vector3 target2D = closestPlayer.Head2D;

			if (settings::config::Prediction) {
				if (point::ProjectileSpeed != 0) {
					target3D = predictLocation(target3D, closestPlayer.Velocity, point::ProjectileSpeed, point::ProjectileGravity, (float)mainCamera.Location.Distance(target3D));
					target2D = w2s(target3D);
				}
			}

			const float screenCenterX = settings::window::Width / 2;
			const float screenCenterY = settings::window::Height / 2;

			float AngleX = 0;
			float AngleY = 0;

			// ugly
			if (target2D.x > screenCenterX)
				AngleX = ( ( - ((screenCenterX - target2D.x) )));
			else if (target2D.x < screenCenterX)
				AngleX = ( (target2D.x - screenCenterX));

			if (target2D.y > screenCenterY)
				AngleY = ( ( - ((screenCenterY - target2D.y))));
			else if (target2D.y < screenCenterY)
				AngleY = ( (target2D.y - screenCenterY));

			AngleX = AngleX / settings::config::AimSmoothing;
			AngleY = AngleY / settings::config::AimSmoothing;

			const double MaxDistance = std::sqrt(std::pow(settings::window::Width, 2) + std::pow(settings::window::Height, 2)) / 2.0;

			float max = 20.0f; // Default value
			if (isHit2D(target2D, 100)) {
				max = computeMax(target2D, 30, 100, 1.0f, 20.0f); // Smoothly interpolate max between 30 and 60 margins
				AngleX = std::clamp(AngleX, -max, max);
				AngleY = std::clamp(AngleY, -max, max);
			}
			else {
				AngleX = std::clamp(AngleX, -max, max);
				AngleY = std::clamp(AngleY, -max, max);
			}

			if (settings::kmbox::NetKmbox) {
				kmNet_mouse_move(static_cast<short>(AngleX), static_cast<short>(AngleY));
			}
			else {
				km_move(static_cast<short>(AngleX), static_cast<short>(AngleY));
			}
		}
		else {
			target.PlayerState = 0;
			Targetting = false;
		}
	}

	void updateTriggerBot()
	{
		if (!settings::runtime::hotKeys)
			return;

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