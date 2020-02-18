#include "VDUIRender.h"

using namespace videodromm;

VDUIRender::VDUIRender(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	
	// contour
	minContour = getMinUniformValueByIndex(mVDSettings->ICONTOUR);
	maxContour = getMaxUniformValueByIndex(mVDSettings->ICONTOUR);
	iResolutionX = (int)getValueByName("iResolutionX");
	iResolutionY = (int)getValueByName("iResolutionY");
	iOutW = getIntValue(mVDSettings->IOUTW);
	iOutH = getIntValue(mVDSettings->IOUTH);
}

int VDUIRender::getIntValue(unsigned int aCtrl) {
	return mVDSession->getIntUniformValueByIndex(aCtrl);
}
float VDUIRender::getValueByName(string aCtrlName) {
	return mVDSession->getFloatUniformValueByName(aCtrlName);
}
float VDUIRender::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIRender::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIRender::setIntValue(unsigned int aCtrl, int aValue) {
	mVDSession->setIntUniformValueByIndex(aCtrl, aValue);
}
void VDUIRender::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIRender::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIRender::toggleSpoutSender() {
	//mVDSettings->mSpoutSender = !mVDSettings->mSpoutSender;
}
bool VDUIRender::getBoolValue(unsigned int aCtrl) {
	return mVDSession->getBoolUniformValueByIndex(aCtrl);
}
void VDUIRender::toggleValue(unsigned int aCtrl) {
	mVDSession->toggleValue(aCtrl);
}
void VDUIRender::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIRender::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIRender::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIRender::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH * 1.76), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

#pragma region render

	ImGui::Begin("Render");
	{
		int hue = 0;
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// output resolution
		ctrl = mVDSettings->IOUTW;
		if (ImGui::Button("x##ioutw")) { iOutW = 1280; setIntValue(ctrl, 1280); }
		ImGui::SameLine();
		if (ImGui::SliderInt("iOutW", &iOutW, 320, 5000))
		{
			setIntValue(ctrl, iOutW);
		}
		ctrl = mVDSettings->IOUTH;
		if (ImGui::Button("x##iouth")) { iOutH = 800; setIntValue(ctrl, 800); }
		ImGui::SameLine();
		if (ImGui::SliderInt("iOutH", &iOutH, 240, 2000))
		{
			setIntValue(ctrl, iOutH);
		}
		// iResolution
		ctrl = mVDSettings->IRESX;
		//iResolutionX = getValueByName("iResolutionX");
		if (ImGui::Button("x##iresx")) { iResolutionX = 1280; setValue(ctrl, 1280); }
		ImGui::SameLine();
		if (ImGui::SliderInt("iResolutionX", &iResolutionX, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionX);
		}
		ctrl = mVDSettings->IRESY;
		//iResolutionY = getValueByName("iResolutionY");
		if (ImGui::Button("x##iresy")) { iResolutionY = 720; setValue(ctrl, 720); }
		ImGui::SameLine();
		if (ImGui::SliderInt("iResolutionY", &iResolutionY, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionY);
		}

		// mRenderXY
		static float mx = mVDSettings->mRenderXY.x;
		if (ImGui::SliderFloat("mx", &mx, 0.01, 1.0))
		{
			mVDSettings->mRenderXY.x = mx;
		}
		ImGui::SameLine();
		static float my = mVDSettings->mRenderXY.y;
		if (ImGui::SliderFloat("my", &my, 0.01, 1.0))
		{
			mVDSettings->mRenderXY.y = my;
		}

		// mRenderXY
		static float texMultW = mVDSettings->mTexMult.x;
		if (ImGui::SliderFloat("texWx", &texMultW, 0.2, 4.0))
		{
			mVDSettings->mTexMult.x = texMultW;
		}
		ImGui::SameLine();
		static float texMultH = mVDSettings->mTexMult.y;
		if (ImGui::SliderFloat("texHx", &texMultH, 0.2, 4.0))
		{
			mVDSettings->mTexMult.x = texMultH;
		}


		// rect mxLeft
		static int mxLeft = mVDSettings->mxLeft;
		if (ImGui::SliderInt("mxL", &mxLeft, 0, 2280))
		{
			mVDSettings->mxLeft = mxLeft;
		}
		ImGui::SameLine();
		// rect myLeft
		static int myLeft = mVDSettings->myLeft;
		if (ImGui::SliderInt("myL", &myLeft, 0, 2280))
		{
			mVDSettings->myLeft = myLeft;
		}
		// rect mxRight
		static int mxRight = mVDSettings->mxRight;
		if (ImGui::SliderInt("mxR", &mxRight, 0, 2280))
		{
			mVDSettings->mxRight = mxRight;
		}
		ImGui::SameLine();
		// rect myRight
		static int myRight = mVDSettings->myRight;
		if (ImGui::SliderInt("myR", &myRight, 0, 2280))
		{
			mVDSettings->myRight = myRight;
		}
		
		// iVignette
		ctrl = mVDSettings->IVIGN;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 7.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 7.0f, 0.8f, 0.8f));
		if (ImGui::Button("vignette")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ctrl = mVDSettings->IVAMOUNT;
		iVAmount = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("Amount", &iVAmount, 0.001f, 0.0f, 1.0f))
		{
			setValue(ctrl, iVAmount);
		}
		ctrl = mVDSettings->IVFALLOFF;
		iVFallOff = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("FallOff", &iVFallOff, 0.001f, 0.0f, 0.99f))
		{
			setValue(ctrl, iVFallOff);
		}

		// iContour
		ctrl = mVDSettings->ICONTOUR;
		if (ImGui::Button("a##contour")) { toggleAuto(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("t##contour")) { toggleTempo(ctrl); }
		ImGui::SameLine();
		if (ImGui::Button("x##contour")) { resetAutoAnimation(ctrl); }
		ImGui::SameLine();
		contour = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ImGui::DragFloat("contour", &contour, 0.001f, minContour, maxContour))
		{
			setValue(ctrl, contour);
		}
		ImGui::DragFloat("mincr", &minContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
		ImGui::SameLine();
		ImGui::DragFloat("maxcr", &maxContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
        // windows
		ImGui::Text("render window %dx%d", mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.9f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ImGui::Button("Auto Layout")) {
			mVDSession->toggleAutoLayout();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Auto Layout for render window");

		// Auto Layout for render window
		if (mVDSession->isAutoLayout()) {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.1f, 0.1f));
			// render window width
			static int rw = mVDSettings->mRenderWidth;
			if (ImGui::SliderInt("rdr w", &rw, 640, 4080))
			{
				//mVDSession->setRenderWidth(rw);
				mVDSettings->mRenderWidth = rw;
			}
			ImGui::SameLine();
			// render window height
			static int rh = mVDSettings->mRenderHeight;
			if (ImGui::SliderInt("rdr h", &rh, 480, 1280))
			{
				//mVDSession->setRenderHeight(rh);
				mVDSettings->mRenderHeight = rh;
			}
		}
		ImGui::PopStyleColor(3);

		/*if (ImGui::Button("Create Window")) {
			mVDSession->createWindow();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Window")) {
			mVDSession->deleteWindow();
		}*/

		ImGui::Text("fp %dx%d f %dx%d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		ImGui::Text("main %dx%d", mVDSettings->mMainWindowWidth, mVDSettings->mMainWindowHeight);
		//ImGui::SameLine();
		//ImGui::Text("solo %d", mVDSession->getSolo());
		
		ImGui::PopItemWidth();
	}
	ImGui::End();

#pragma endregion render

}
