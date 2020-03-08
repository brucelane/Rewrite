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


		sprintf(buf, "%s##sh%d", mVDSession->getWarpName(w).c_str(), w);
		//sprintf(buf, "warp##sh%d", w);
		ImGui::Begin(buf, NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{

			int hue = 0;

			sprintf(buf, "w %d", mVDSession->getWarpWidth(w));
			ImGui::TextColored(ImColor(150, 220, 0), buf);
			ImGui::SameLine();
			sprintf(buf, "h %d", mVDSession->getWarpHeight(w));
			ImGui::TextColored(ImColor(150, 220, 0), buf);
			int ww = mVDSession->getWarpWidth(w);
			sprintf(buf, "WarpWidth##ww%d", w);
			if (ImGui::SliderInt(buf, &ww, 0.0f, mVDSettings->mFboWidth*2))
			{
				mVDSession->setWarpWidth(w, ww);
			}
			
			int wh = mVDSession->getWarpHeight(w);
			sprintf(buf, "WarpHeight##wh%d", w);
			if (ImGui::SliderInt(buf, &wh, 0.0f, mVDSettings->mFboHeight*2))
			{
				mVDSession->setWarpHeight(w, wh);
			}


			(mUseMixette) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
			sprintf(buf, "mixette##mxt%d", w);
			mUseMixette ^= ImGui::Button(buf);
			ImGui::PopStyleColor(3);
			hue++;

			if (mUseMixette) {
				
					const float spacing = 4;
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

					ImGui::PushID("fbomixes");
					for (int m = 0; m < mVDSession->getFboListSize(); m++)
					{
						if (m > 0) ImGui::SameLine();
						
						//ctrl = mVDSettings->IWEIGHT0 + m;
						ctrl = math<int>::min(mVDSettings->IWEIGHT8, mVDSettings->IWEIGHT0 + m);
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
			}
			else {
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
				ImGui::PopID();
				ImGui::PopItemWidth();
			}
		}

		ImGui::End();
	}


}
