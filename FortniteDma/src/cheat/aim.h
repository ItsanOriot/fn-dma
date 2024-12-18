
#include "..\kmbox\kmbox.hpp"
#include "..\settings.h"

namespace aim {

	Vector3 predictLocation(Vector3 target, Vector3 target_velocity, float projectile_speed, float projectile_gravity_scale, float distance)
	{
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

	double isClose(Vector3 loc2D) {

		const double maxDistance = std::sqrt(std::pow(settings::window::Width, 2) + std::pow(settings::window::Height, 2)) / 2.0;
		
		double distance = std::sqrt(std::pow(loc2D.x - settings::window::Width / 2, 2) + std::pow(loc2D.y - settings::window::Height / 2, 2));
		
		double closeness = 1.0f - (distance / maxDistance);
		
		closeness = std::clamp(closeness, 0.0, 1.0);
		
		return closeness;
	}

	constexpr float RadiansToDegrees(float radians) {
		return radians * (180.0f / M_PI);
	}

	Rotation targetRotation(const Vector3& currentPosition, const Vector3& targetPosition) {
		float directionX = targetPosition.x - currentPosition.x;
		float directionY = targetPosition.y - currentPosition.y;
		float directionZ = targetPosition.z - currentPosition.z;

		float yawRadians = std::atan2(directionY, directionX);
		float yawDegrees = RadiansToDegrees(yawRadians);

		float distanceXY = std::sqrt(directionX * directionX + directionY * directionY); // Horizontal distance
		float pitchRadians = std::atan2(directionZ, distanceXY);
		float pitchDegrees = RadiansToDegrees(pitchRadians);

		return { yawDegrees, pitchDegrees };
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

			Vector3 originalTarget3D = closestPlayer.Head3D;
			Vector3 originalTarget2D = closestPlayer.Head2D;

			Vector3 target3D = originalTarget3D;
			Vector3 target2D = originalTarget2D;

			if (settings::config::Prediction) {
				if (point::ProjectileSpeed > 0) {
					target3D = predictLocation(target3D, closestPlayer.Velocity, point::ProjectileSpeed, point::ProjectileGravity, (float)mainCamera.Location.Distance(target3D));
					target2D = w2s(target3D);
				}
			}

			Rotation target = targetRotation(mainCamera.Location, target3D);

			float targety = target.yaw - mainCamera.Rotation.y;
			float targetx = target.pitch - mainCamera.Rotation.x;

			//targety /= settings::config::AimSmoothing;
			//targetx /= settings::config::AimSmoothing;

			while (targety > 180.0f) targety -= 360.0f;
			while (targety < -180.0f) targety += 360.0f;

			while (targetx > 89.9f) targetx = 89.9f;
			while (targetx < -89.9f) targetx = -89.9f;

			float AngleX = targety * (settings::config::StepPerDegreeX / settings::config::AimSmoothing);
			float AngleY = targetx * (settings::config::StepPerDegreeY / settings::config::AimSmoothing);

			if (settings::kmbox::NetKmbox) {
				kmNet_mouse_move(AngleX, AngleY);
			}
			else {
				km_move(AngleX, AngleY);
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

		if (!settings::config::TriggerBot)
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