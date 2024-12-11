#pragma once

namespace radar {
	static Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
	{
		float r_1, r_2;
		float x_1, y_1;

		r_1 = -(EntityPos.y - LocalPlayerPos.y);
		r_2 = (EntityPos.x - LocalPlayerPos.x);
		float Yaw = angle - 90;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

		*viewCheck = y_1 < 0;

		x_1 *= -zoom;
		y_1 *= zoom;

		int sizX = sizeX / 2;
		int sizY = sizeY / 2;

		x_1 += sizX;
		y_1 += sizY;

		if (x_1 < 5)
			x_1 = 5;

		if (x_1 > sizeX - 5)
			x_1 = sizeX - 5;

		if (y_1 < 5)
			y_1 = 5;

		if (y_1 > sizeY - 5)
			y_1 = sizeY - 5;


		x_1 += posX;
		y_1 += posY;


		return Vector3(x_1, y_1, 0);
	}

	void DrawHUD(int xAxis, int yAxis, int width, int height)
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		ImVec2 pos(xAxis, yAxis);
		ImVec2 siz(width, height);
		ImVec2 RadarCenter(pos.x + siz.x / 2, pos.y + siz.y / 2);

		// Draw outline box
		ImU32 outlineColor = IM_COL32(75, 0, 95, 255); // Adjust color as needed
		drawList->AddRect(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), outlineColor, NULL, NULL, 3.f);

		// Draw filled box
		ImU32 fillColor = IM_COL32(25, 25, 25, 255); // Adjust color as needed
		drawList->AddRectFilled(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), fillColor);

		// Draw lines
		ImU32 lineColor = IM_COL32(100, 100, 100, 255); // Adjust color as needed
		drawList->AddLine(RadarCenter, pos, lineColor);
		drawList->AddLine(RadarCenter, ImVec2(pos.x + siz.x, pos.y), lineColor);
		drawList->AddLine(ImVec2(pos.x, RadarCenter.y), ImVec2(pos.x + siz.x, RadarCenter.y), lineColor);
		drawList->AddLine(RadarCenter, ImVec2(RadarCenter.x, pos.y + siz.y), lineColor);

		// Draw circle
		float circleRadius = 2.0f;
		drawList->AddCircle(RadarCenter, circleRadius, lineColor, 12, 1.0f);
	}

	void DrawPoint(Vector3 position, int distance, Vector3 localPosition, int scale, float zoom, double localYaw, int xAxis, int yAxis, int width, int height, ImColor color)
	{
		bool out = false;
		Vector3 siz;
		siz.x = width;
		siz.y = height;
		Vector3 pos;
		pos.x = xAxis;
		pos.y = yAxis;
		bool ck = false;
		Vector3 single = RotatePoint(position, localPosition, pos.x, pos.y, siz.x, siz.y, localYaw, zoom, &ck);
		if (distance >= 0.f && distance < scale)
		{
			ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(single.x, single.y), 5, color);
			//ImGui::GetBackgroundDrawList()->AddText(ImVec2(single.x, single.y), color, std::to_string((int)distance).c_str());

		}
	}

	void Render()
	{
		std::unordered_map<uintptr_t, PlayerCache> PlayerList = mainPlayerList;

		// draw the radar
		DrawHUD(settings::config::RadarX, settings::config::RadarY, settings::config::RadarXSize, settings::config::RadarYSize);

		// draw the points in the radar
		for (auto it : PlayerList) {
			PlayerCache player = it.second;

			if (!isPlayerValid(player))
				continue;

			if (player.PlayerState == point::PlayerState) {
				continue;
			}

			if (point::Player && player.TeamId == local_player::localTeam) {
				continue;
			}

			// should be in the updates
			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			ImColor color = ImColor(255, 0, 0, 255);

			// the order matters

			// red not visible
			if (IsVis)
				color = ImColor(0, 255, 0, 255);

			// orange knocked players
			if (player.isDBNO)
				color = ImColor(255, 100, 0, 255);

			// blue bots
			if (player.isBot)
				color = ImColor(0, 0, 255, 255);

			DrawPoint(player.Head3D, mainCamera.Location.Distance(player.Head3D), mainCamera.Location, settings::config::RadarScale, settings::config::RadarZoom, mainCamera.Rotation.y, settings::config::RadarX, settings::config::RadarY, settings::config::RadarXSize, settings::config::RadarYSize, color);

		}
	}
}