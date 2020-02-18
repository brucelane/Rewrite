#include "VDUITempo.h"

using namespace videodromm;

VDUITempo::VDUITempo(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUITempo::~VDUITempo() {
}

void VDUITempo::Run(const char* title) {


	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH * 1.76), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

	ImGui::Begin("Warp");
	{
		
	}
	ImGui::End();


}
