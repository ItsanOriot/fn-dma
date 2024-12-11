#pragma once

namespace esp {
	void renderPlayers()
	{
		// we make a copy of the cache we dont want modifications from elsewhere while using it
		//std::unordered_map<uintptr_t, PlayerCache> PlayerList = mainPlayerList;

		// stats
		int playersRendered = 0;
		int playersLooped = 0;
		int validPlayersLooped = 0;
		int invalidPlayersLooped = 0;
		int teammatesSkipped = 0;
		int bots = 0;

		for (auto it : mainPlayerList) {
			PlayerCache player = it.second;

			playersLooped++;

			// valid?
			if (!isPlayerValid(player)) {
				invalidPlayersLooped++;
				continue;
			}

			validPlayersLooped++;

			// should be in the updates
			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			// player.isDBNO

			if (player.isBot) {
				bots++;
			}

			// check if its me ༼ つ ◕_◕ ༽つ
			if (player.PlayerState == point::PlayerState) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "me");
				continue;
			}

			// chcek if not in lobby and check if its a teammate (omg friends) (◕‿◕✿)
			if (point::Player && player.TeamId == local_player::localTeam) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f ,ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255,255,255,255), "teammate");
				teammatesSkipped++;
				continue;
			}

			// check if dying (dead) ◉_◉
			if (player.isDying) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "dying");
				continue;
			}

			// ok maybe we can draw now

			playersRendered++;

			// skeleton
			if (settings::config::Skeleton)
			{
				// color
				ImColor colSK = ImColor(255,0,0,255);
				if (IsVis)
					colSK = ImColor(0, 255, 0, 255);

				//thickness
				float tk = 2.f;

				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Neck2D.x, player.Neck2D.y), ImVec2(player.Head2D.x, player.Head2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Hip2D.x, player.Hip2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.LeftHandT2D.x, player.LeftHandT2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.RightHandT2D.x, player.RightHandT2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftFoot2D.x, player.LeftFoot2D.y), ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), colSK, tk);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightFoot2D.x, player.RightFoot2D.y), ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), colSK, tk);
			}

			// box
			if (settings::config::Box)
			{
				ImColor colSK = ImColor(255, 0, 0, 255);
				if (IsVis)
					colSK = ImColor(0, 255, 0, 255);

				float tk = 1.f;
				float rd = 0.f;

				float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
				float box_width = 0.5f * box_height;
				ImGui::GetBackgroundDrawList()->AddRect(ImVec2(player.Bottom2D.x - box_width / 2, player.Bottom2D.y), ImVec2(player.Bottom2D.x + box_width / 2, player.Top2D.y), colSK, rd, 0, tk);

			}
			
		}

		// stats
		info::render::playersRendered = playersRendered;
		info::render::playersLooped = playersLooped;
		info::render::validPlayersLooped = validPlayersLooped;
		info::render::invalidPlayersLooped = invalidPlayersLooped;
		info::render::teammatesSkipped = teammatesSkipped;
		info::render::validBots = bots;
	}

	void Debug() 
	{

		

	}
}