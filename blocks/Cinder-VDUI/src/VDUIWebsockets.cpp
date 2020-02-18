#include "VDUIWebsockets.h"

using namespace videodromm;

VDUIWebsockets::VDUIWebsockets(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIWebsockets::~VDUIWebsockets() {

}

void VDUIWebsockets::Run(const char* title) {
	#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiSmallH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

	ImGui::Begin("Hydra");
	{
		

	}
	ImGui::End();
	#endif
}
