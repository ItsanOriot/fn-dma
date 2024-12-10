#pragma once

namespace esp {
	void renderPlayers()
	{
		// we make a copy of the cache we dont want modifications from elsewhere while using it
		std::unordered_map<uintptr_t, PlayerCache> PlayerList = readyCache;

		// stats
		int playersRendered = 0;
		int playersLooped = 0;
		int validPlayersLooped = 0;
		int invalidPlayersLooped = 0;
		int teammatesSkipped = 0;

		for (auto it : PlayerList) {
			PlayerCache player = it.second;

			playersLooped++;

			// valid?
			if (!player.Pawn || !player.BoneArray) {
				invalidPlayersLooped++;
				continue;
			}

			validPlayersLooped++;

			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			// player.isBot
			// player.isDBNO

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
			if (settings::config::skeleton)
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
			
		}

		// stats
		info::render::playersRendered = playersRendered;
		info::render::playersLooped = playersLooped;
		info::render::validPlayersLooped = validPlayersLooped;
		info::render::invalidPlayersLooped = invalidPlayersLooped;
		info::render::teammatesSkipped = teammatesSkipped;

	}
}