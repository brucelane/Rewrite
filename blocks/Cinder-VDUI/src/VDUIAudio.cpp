#include "VDUIAudio.h"

using namespace videodromm;

VDUIAudio::VDUIAudio(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIAudio::~VDUIAudio() {

}

void VDUIAudio::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH * 1.76), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

	sprintf(buf, "%s##inpt", mVDSession->getAudioTextureName().c_str());// TODO 20200221 mVDSession->getInputTextureName(0).c_str()
	ImGui::Begin(buf);
	{
		if (ImGui::CollapsingHeader("Audio", NULL, true, true))
		{
			ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth * 2);
			//ImGui::Image((void*)mVDSession->getAudioTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			// TODO 20200221 ImGui::Text("Position %d", mVDSession->getPosition(0));

			static int iFreq0 = mVDSession->getFreqIndex(0);
			sprintf(buf, "f0 %4.2f##f0", mVDSession->getFreq(0));
			if (ImGui::SliderInt(buf, &iFreq0, 0, mVDSession->getFFTWindowSize()))
			{
				mVDSession->setFreqIndex(0, iFreq0);
			}
			static int iFreq1 = mVDSession->getFreqIndex(1);
			sprintf(buf, "f1 %4.2f##f1", mVDSession->getFreq(1));
			if (ImGui::SliderInt(buf, &iFreq1, 0, mVDSession->getFFTWindowSize()))
			{
				mVDSession->setFreqIndex(1, iFreq1);
			}

			static int iFreq2 = mVDSession->getFreqIndex(2);
			sprintf(buf, "f2 %4.2f##f2", mVDSession->getFreq(2));
			if (ImGui::SliderInt(buf, &iFreq2, 0, mVDSession->getFFTWindowSize()))
			{
				mVDSession->setFreqIndex(2, iFreq2);
			}

			static int iFreq3 = mVDSession->getFreqIndex(3);
			sprintf(buf, "f3 %4.2f##f3", mVDSession->getFreq(3));
			if (ImGui::SliderInt(buf, &iFreq3, 0, mVDSession->getFFTWindowSize()))
			{
				mVDSession->setFreqIndex(3, iFreq3);
			}
			// TODO 20200221 
			/*(mVDSession->getFreqWSSend()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.0f, 0.8f, 0.8f));
			if (ImGui::Button("Send WS Freqs")) {
				mVDSession->toggleFreqWSSend();
			}
			ImGui::PopStyleColor(3);*/

			//ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Tempo", NULL, true, true))
		{

			//ImGui::Text("start %f ", mVDSettings->iStart);


			if (ImGui::Button("x##startx")) { mVDSettings->iStart = 0.0f; }
			ImGui::SameLine();
			ImGui::SliderFloat("start", &mVDSettings->iStart, 0.01f, 1.0f, "%.4f");

			if (ImGui::Button("x##spdx")) { mVDSettings->iSpeedMultiplier = 1.0f; }
			ImGui::SameLine();
			ImGui::SliderFloat("speed x", &mVDSettings->iSpeedMultiplier, 0.01f, 1.0f, "%.4f");//, 2.01f

			/*static int tf = 5;
			if (ImGui::Button("x##tfx")) { tf = 5; mVDSession->setTimeFactor(5); }
			ImGui::SameLine();
			if (ImGui::SliderInt("time x", &tf, 0, 9)) mVDSession->setTimeFactor(tf);

			ImGui::SliderFloat("iTimeFactor", &mVDSettings->iTimeFactor, 0.01f, 1.0f, "%.4f");
			*/
			ImGui::Text("beat %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBEAT));
			ImGui::SameLine();
			ImGui::Text("bar %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBAR));
			ImGui::SameLine();
			ImGui::Text("bb %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBARBEAT));

			ImGui::Text("Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITIME));
			ImGui::SameLine();
			ImGui::Text("Tempo Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITEMPOTIME));

			ImGui::Text("Delta Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->IDELTATIME));
			// LiveOSC Obsolete ImGui::Text("Trk %s %.2f", mVDSettings->mTrackName.c_str(), mVDSettings->liveMeter);
			ImGui::SameLine();
			//			ImGui::Checkbox("Playing", &mVDSettings->mIsPlaying);
			ImGui::Text("Tempo %.2f ", mVDSession->getBpm());

			//ImGui::Text("Elapsed %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->IELAPSED));
			// duration			
			//ImGui::SameLine();
			//ImGui::Text("duration", &mVDSettings->iBarDuration);
			//ImGui::SameLine();
			//ImGui::Text("phase %.2f ", mVDSession->getFloatUniformValueByIndex(mVDSettings->IPHASE));
			// BUG taptempo
			if (ImGui::Button("Tap toggle")) { toggleSpinalTap(); }
			if (spinalTap) {
				static float tempo = mVDSession->getBpm();
				//ImGui::Text("Tempo %.2f ", tempo);
				if (ImGui::DragFloat("Tempo", &tempo, 0.01f, 0.01f, 200.0f, "%.2f"))
				{
					mVDSession->setBpm(tempo);
				}
				if (ImGui::SliderFloat("TempoS", &tempo, 0.01f, 200.0f, "%.01f")) {
					mVDSession->setBpm(tempo);
				}
			}
			else {
				if (ImGui::Button("Tap tempo")) { mVDSession->tapTempo(); }
			}

			if (mVDSession->getUseTimeWithTempo()) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			if (ImGui::Button("Time tempo")) { mVDSession->toggleUseTimeWithTempo(); }
			ImGui::PopStyleColor(3);


			//ImGui::SameLine();
			ImGui::PopItemWidth();
		}

#pragma endregion Tempo	

	}
	ImGui::End();

}
