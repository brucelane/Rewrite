#include "VDUIWarps.h"

using namespace videodromm;

VDUIWarps::VDUIWarps(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}

void VDUIWarps::Run(const char* title) {
	//static int currentNode = 0;
	
	//xPos = mVDSettings->uiMargin;
	//yPos = mVDSettings->uiYPosRow2;
	for (int w = 0; w < mVDSession->getWarpCount(); w++) {

		
		xPos = mVDSettings->uiMargin + mVDSettings->uiXPosCol1 + ((mVDSettings->uiLargePreviewW + mVDSettings->uiMargin) * (w));//+1
		yPos = mVDSettings->uiYPosRow2;
		ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ImGui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);

		int hue = 1;
		(mUseMixette) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		mUseMixette ^= ImGui::Button("mixette");
		ImGui::PopStyleColor(3);
		hue++;

		if (mUseMixette) {
			if (ImGui::TreeNode("Vertical Sliders"))
				{
			ImGui::Unindent();
			const float spacing = 4;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

			ImGui::PushID("fbomixes");
			for (int m = 0; m < mVDSession->getModesCount(); m++)
			{
				if (m > 0) ImGui::SameLine();
				if (mVDSession->getMode() == m) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(m / 16.0f, 1.0f, 0.5f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(m / 16.0f, 0.1f, 0.1f));
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(m / 16.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(m / 16.0f, 0.8f, 0.8f));
				sprintf(buf, "%s##mode", mVDSession->getModeName(m).c_str());
				if (ImGui::Button(buf)) mVDSession->setMode(m);
				sprintf(buf, "Set mode to %s", mVDSession->getModeName(m).c_str());
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::PopStyleColor(3);
				ImGui::SameLine();
				ctrl = mVDSettings->IWEIGHT0 + m;
				float iWeight = mVDSession->getFloatUniformValueByIndex(ctrl);
				ImGui::PushID(m);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(m / 16.0f, 0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(m / 16.0f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(m / 16.0f, 0.7f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(m / 16.0f, 0.9f, 0.9f));
				if (ImGui::VSliderFloat("##v", ImVec2(18, 60), &iWeight, 0.0f, 1.0f, ""))
				{
					setValue(ctrl, iWeight);
				};
				if (ImGui::IsItemActive() || ImGui::IsItemHovered())
					ImGui::SetTooltip("%.3f", iWeight);
				ImGui::PopStyleColor(4);
				ImGui::PopID();
			}
			ImGui::PopID();


			ImGui::PopStyleVar();

			ImGui::Indent();

			ImGui::Indent();
			ImGui::TreePop();
		}
		}
		else {
			sprintf(buf, "%s##sh%d", mVDSession->getWarpName(w).c_str(), w);
			//sprintf(buf, "warp##sh%d", w);
			ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ImGui::PushID(w);
				int fboa = mVDSession->getWarpAFboIndex(w);

				//if (mVDSession->getFboRenderedTexture(fboa)) ImGui::Image((void*)mVDSession->getFboRenderedTexture(fboa)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				if (mVDSession->getFboRenderedTexture(fboa)) ImGui::Image(mVDSession->getFboRenderedTexture(fboa), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));

				/*ImGui::Image((void*)mVDSession->getMixTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(mVDSession->getWarpName(w).c_str());*/
				// loop on the fbos A
				for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
					if (a > 0 && (a % 6 != 0)) ImGui::SameLine();
					if (fboa == a) {
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 16.0f, 1.0f, 1.0f));
					}
					else {
						ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(a / 16.0f, 0.1f, 0.1f));
					}
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(a / 16.0f, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(a / 16.0f, 0.8f, 0.8f));

					sprintf(buf, "%d##wia%d%d", a, w, a);
					if (ImGui::Button(buf)) {
						mVDSession->setWarpAFboIndex(w, a);
					};
					//sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
					//if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
					ImGui::PopStyleColor(3);
				}


				// crossfade
				/*float xFade = mVDSession->getWarpCrossfade(w);
				sprintf(buf, "xfade##xf%d", w);
				if (ImGui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
				{
					mVDSession->setWarpCrossfade(w, xFade);
				}*/
				// nodes
				/*if (currentNode == w) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.5f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 0.8f, 0.8f));
				sprintf(buf, "N##n%d", w);
				if (ImGui::Button(buf)) {
					if (w == currentNode) {
						// if the same button pressed we hide the nodes
						currentNode = -1;
					}
					else {
						currentNode = w;
						setCurrentEditIndex(w);
					}
				}
				ImGui::PopStyleColor(3);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Show nodes");
				ImGui::SameLine();
				// spout output
				if (getSharedMixIndex() == w && isSharedOutputActive()) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
				sprintf(buf, "O##sp%d", w);
				if (ImGui::Button(buf)) {
					toggleSharedOutput(w);
				}
				ImGui::PopStyleColor(3);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Spout output");
				ImGui::SameLine();
				// active
				if (isWarpActive(w)) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
					sprintf(buf, "AC##a%d", w);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
					sprintf(buf, "A##a%d", w);
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
				if (ImGui::Button(buf)) {
					toggleWarpActive(w);
				}
				ImGui::PopStyleColor(3);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Warp Active");
				ImGui::SameLine();
				// solo
				if (isWarpSolo(w)) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
					sprintf(buf, "SOLO##s%d", w);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
					sprintf(buf, "S##s%d", w);
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
				if (ImGui::Button(buf)) {
					toggleWarpSolo(w);
				}
				ImGui::PopStyleColor(3);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Warp Solo");
				ImGui::SameLine();
				// delete
				if (isWarpDeleted(w)) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
				}
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));
				sprintf(buf, "X##x%d", w);
				if (ImGui::Button(buf)) {
					toggleDeleteWarp(w);
				}
				ImGui::PopStyleColor(3);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Delete Warp");
	*/
				ImGui::PopID();
				ImGui::PopItemWidth();
			}

			/*
#pragma region Nodes
			if (currentNode == w) {
				// A (left)
				int t = 0;
				int fboIndex = getWarpAFboIndex(currentNode);
				int inputTexture = getFboInputTextureIndex(getWarpAFboIndex(currentNode));

				ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
				ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
				sprintf(buf, "%s##txa", getInputTextureName(inputTexture).c_str());
				ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
				{
					ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
					ImGui::Image((void*)getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
					ImGui::PopItemWidth();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::Text("WoH %dx%d", getInputTextureOriginalWidth(inputTexture), getInputTextureOriginalHeight(inputTexture));
				ImGui::Text("WxH %dx%d", getInputTexture(inputTexture)->getWidth(), getInputTexture(inputTexture)->getHeight());
				ImGui::Text("tx A %s", getInputTextureName(inputTexture).c_str());
				ImGui::End();

				t++;
				ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
				ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
				sprintf(buf, "%s##fboa", getFboName(fboIndex).c_str());
				ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
				{
					ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
					ImGui::Image((void*)getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
					ImGui::PopItemWidth();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::Text("WxH %dx%d", getFboRenderedTexture(fboIndex)->getWidth(), getFboRenderedTexture(fboIndex)->getHeight());
				ImGui::End();

				// mix
				t++;
				ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
				ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
				ImGui::Begin(getWarpName(currentNode).c_str(), NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
				{
					ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
					ImGui::Image((void*)getMixTexture(currentNode)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
					ImGui::PopItemWidth();
				}
				ImGui::Text("WxH %dx%d", getMixTexture(currentNode)->getWidth(), getMixTexture(currentNode)->getHeight());
				ImGui::End();

				// B (right)
				t++;
				fboIndex = getWarpBFboIndex(currentNode);
				inputTexture = getFboInputTextureIndex(getWarpBFboIndex(currentNode));
				ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
				ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
				sprintf(buf, "%s##fbob", getFboName(fboIndex).c_str());
				ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
				{
					ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
					ImGui::Image((void*)getFboRenderedTexture(fboIndex)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
					ImGui::PopItemWidth();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::Text("WxH %dx%d", getFboRenderedTexture(fboIndex)->getWidth(), getFboRenderedTexture(fboIndex)->getHeight());
				ImGui::End();

				t++;

				ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
				ImGui::SetNextWindowPos(ImVec2((t * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin + mVDSettings->uiLargeW, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
				sprintf(buf, "%s##txb", getInputTextureName(inputTexture).c_str());
				ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
				{
					ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
					ImGui::Image((void*)getInputTexture(inputTexture)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
					ImGui::PopItemWidth();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
				ImGui::Text("WoH %dx%d", getInputTextureOriginalWidth(inputTexture), getInputTextureOriginalHeight(inputTexture));
				ImGui::Text("WxH %dx%d", getInputTexture(inputTexture)->getWidth(), getInputTexture(inputTexture)->getHeight());
				ImGui::Text("tx B %s", getInputTextureName(inputTexture).c_str());
				ImGui::End();
#pragma endregion Nodes
			} */

			ImGui::End();
		}
		/*xPos += mVDSettings->uiLargePreviewW + mVDSettings->uiMargin;
		if (xPos > mVDSettings->mRenderWidth)
		{
			xPos = mVDSettings->uiMargin;
			yPos += mVDSettings->uiLargePreviewH + mVDSettings->uiMargin;
		}*/
	}
}
