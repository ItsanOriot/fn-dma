#pragma once

namespace esp {
	void renderPlayers()
	{
		// we make a copy of the cache we dont want modifications from elsewhere while using it
		std::unordered_map<uintptr_t, PlayerCache> PlayerList = readyCache;

		for (auto it : PlayerList) {
			PlayerCache player = it.second;

			// valid?
			if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			bool IsBot = player.isBot & 0x08;

			bool IsDying = player.isDying & 0x10;

			// knocked
			bool IsDBNO = player.isDBNO & 0x10;

			// check if dying (dead) ◉_◉
			if (IsDying) continue;

			// check if its me ༼ つ ◕_◕ ༽つ
			if (player.PlayerState == point::PlayerState) continue;

			// chcek if in not lobby and check if its a teamate (omg friends) (◕‿◕✿)
			if (point::Player && player.TeamId == local_player::localTeam) continue;

			// ok maybe we can draw now

			// just a circle on the head for now
			if (IsVis)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(player.Head2D.x, player.Head2D.y), 10.f, ImColor(0, 255, 0, 255), 100);
			else
				ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(player.Head2D.x, player.Head2D.y), 10.f, ImColor(255, 0, 0, 255), 100);


			// this is how skeleton would be but i repeat CalcMatrix should be done elsewhere. maybe.
			/*
			Vector3 vHip = CalcMatrix(pair.second.HipBone, component_to_world);
			Vector3 vNeck = CalcMatrix(pair.second.NeckBone, component_to_world);
			Vector3 vUpperArmLeft = CalcMatrix(pair.second.UpperArmLeftBone, component_to_world);
			Vector3 vUpperArmRight = CalcMatrix(pair.second.UpperArmRightBone, component_to_world);
			Vector3 vLeftHand = CalcMatrix(pair.second.LeftHandBone, component_to_world);
			Vector3 vRightHand = CalcMatrix(pair.second.RightHandBone, component_to_world);
			Vector3 vLeftHand1 = CalcMatrix(pair.second.LeftHand1Bone, component_to_world);
			Vector3 vRightHand1 = CalcMatrix(pair.second.RightHand1Bone, component_to_world);
			Vector3 vRightThigh = CalcMatrix(pair.second.RightThighBone, component_to_world);
			Vector3 vLeftThigh = CalcMatrix(pair.second.LeftThighBone, component_to_world);
			Vector3 vRightCalf = CalcMatrix(pair.second.RightCalfBone, component_to_world);
			Vector3 vLeftCalf = CalcMatrix(pair.second.LeftCalfBone, component_to_world);
			Vector3 vLeftFoot = CalcMatrix(pair.second.LeftFootBone, component_to_world);
			Vector3 vRightFoot = CalcMatrix(pair.second.RightFootBone, component_to_world);

			Vector3 vHeadBoneOut = Head2D;
			Vector3 vHipOut = ProjectWorldToScreen(vHip);
			Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
			Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
			Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
			Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
			Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
			Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
			Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
			Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
			Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
			Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
			Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
			Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
			Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);

			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vNeckOut.x, vNeckOut.y), ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), colSK, tk);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), colSK, tk);
			*/

		}
	}
}