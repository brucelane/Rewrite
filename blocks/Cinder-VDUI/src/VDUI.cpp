#include "VDUI.h"

using namespace videodromm;

VDUI::VDUI(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	// UITextures
	//mUITextures = VDUITextures::create(mVDSettings, mVDSession);
	// UIFbos
	mUIFbos = VDUIFbos::create(mVDSettings, mVDSession);
	// UIAnimation
	mUIAnimation = VDUIAnimation::create(mVDSettings, mVDSession);
	// UIMidi
	/*mUIMidi = VDUIMidi::create(mVDSettings, mVDSession);
	// UIAudio
	mUIAudio = VDUIAudio::create(mVDSettings, mVDSession);
	// UIColor
	mUIColor = VDUIColor::create(mVDSettings, mVDSession);
	// UITempo
	//mUITempo = VDUITempo::create(mVDSettings, mVDSession);
	// UIBlend
	mUIBlend = VDUIBlend::create(mVDSettings, mVDSession);
	// UIOsc
	mUIOsc = VDUIOsc::create(mVDSettings, mVDSession);
	// UIWebsockets
	mUIWebsockets = VDUIWebsockets::create(mVDSettings, mVDSession);
	// UIMouse
	mUIMouse = VDUIMouse::create(mVDSettings, mVDSession);
	// UIShaders
	mUIShaders = VDUIShaders::create(mVDSettings, mVDSession);
	// UIRender
	mUIRender = VDUIRender::create(mVDSettings, mVDSession);*/
	// UIWarps
	mUIWarps = VDUIWarps::create(mVDSettings, mVDSession);
	// imgui
	mouseGlobal = false;
	mouseZ = false;
	mIsResizing = true;
	mShowWarps = true;
	mShowFbos = true;
}

void VDUI::Run(const char* title, unsigned int fps) {
	static int currentWindowRow1 = 1;
	static int currentWindowRow2 = 0;

	ImGuiStyle& style = ImGui::GetStyle();

	if (mIsResizing) {
		mIsResizing = false;

		// set ui window and io events callbacks 
		ImGui::connectWindow(getWindow());
		ImGui::initialize();

#pragma region style
		// our theme variables
		style.WindowRounding = 8;
		style.WindowPadding = ImVec2(3, 3);
		style.FramePadding = ImVec2(2, 2);
		style.FrameRounding = 16;
		style.ItemSpacing = ImVec2(3, 3);
		style.ItemInnerSpacing = ImVec2(3, 3);
		style.WindowMinSize = ImVec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight);
		style.Alpha = 0.65f;

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.40f, 0.40f, 0.60f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.40f, 0.40f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



		/*style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.55f, 0.00f, 0.93f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.00f, 0.80f, 0.39f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.00f, 0.63f, 0.71f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.56f, 0.09f, 1.00f, 0.46f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.95f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.94f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.90f, 0.45f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);*/
#pragma endregion style
	}
#pragma region menu
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("Options"))
		{
			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

#pragma endregion menu
	if (ImGui::Button("Clear")) {
		mVDSettings->mMsg = "";
		mVDSettings->mMidiMsg = "";
		mVDSettings->mWebSocketsMsg = "";
		mVDSettings->mOSCMsg = "";
		mVDSettings->mErrorMsg = "";
		mVDSettings->mShaderMsg = "";
		mVDSettings->mFboMsg = "";
	}
	ImGui::TextColored(ImColor(200, 200, 0), "Msg: %s", mVDSettings->mMsg.c_str());
	//ImGui::TextWrapped("Msg: %s", mVDSettings->mMsg.c_str());
	ImGui::TextWrapped("Fbo: %s", mVDSettings->mFboMsg.c_str());
	ImGui::TextWrapped("Shader: %s", mVDSettings->mShaderMsg.c_str());
	ImGui::TextWrapped("Midi: %s", mVDSettings->mMidiMsg.c_str());
	ImGui::TextWrapped("WS Msg: %s", mVDSettings->mWebSocketsMsg.c_str());
	ImGui::TextWrapped("OSC Msg: %s", mVDSettings->mOSCMsg.c_str());
	//ImGui::TextWrapped("Last error: %s", mVDSettings->mErrorMsg.c_str());
	ImGui::TextColored(ImColor(255, 0, 0), "Last error: %s", mVDSettings->mErrorMsg.c_str());
	ImGui::SetNextWindowSize(ImVec2(800, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	//sprintf(buf, "Fps %c %d (%.2f)###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], fps, mVDSession->getTargetFps());
	sprintf(buf, "Fps %c %d ###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], fps);
	ImGui::Begin(buf);
	{
		// line 1
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		/*ImGui::Image((void*)mVDSession->getMixetteTexture(0)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Mixette");
		ImGui::SameLine();*/
		ImGui::Image((void*)mVDSession->getRenderedMixetteTexture(0)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("RenderedMixette");
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getPostFboTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Post");
		ImGui::SameLine();
		ImGui::Image((void*)mVDSession->getRenderedWarpFboTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Warp");

		ImGui::SameLine();
		// fps
		static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size() * sizeof(float)); }
		static int values_offset = 0;
		static float refresh_time = -1.0f;
		if (ImGui::GetTime() > refresh_time + 1.0f / 6.0f)
		{
			refresh_time = ImGui::GetTime();
			values[values_offset] = mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS);
			values_offset = (values_offset + 1) % values.size();
		}
		if (mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS) < 12.0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		// TODO ImGui::PlotLines("F", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, mVDSession->getTargetFps(), ImVec2(0, 30));
		ImGui::PlotLines("F", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, 100.0f, ImVec2(0, 30));
		if (mVDSession->getFloatUniformValueByIndex(mVDSettings->IFPS) < 12.0) ImGui::PopStyleColor();
		// audio
		ImGui::SameLine();
		static ImVector<float> timeValues; if (timeValues.empty()) { timeValues.resize(40); memset(&timeValues.front(), 0, timeValues.size() * sizeof(float)); }
		static int timeValues_offset = 0;
		// audio maxVolume
		static float tRefresh_time = -1.0f;
		if (ImGui::GetTime() > tRefresh_time + 1.0f / 20.0f)
		{
			tRefresh_time = ImGui::GetTime();
			timeValues[timeValues_offset] = mVDSession->getMaxVolume();
			timeValues_offset = (timeValues_offset + 1) % timeValues.size();
		}

		ImGui::PlotHistogram("H", mVDSession->getFreqs(), mVDSession->getFFTWindowSize(), 0, NULL, 0.0f, 255.0f, ImVec2(0, 30));
		ImGui::SameLine();
		if (mVDSession->getMaxVolume() > 240.0) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PlotLines("V", &timeValues.front(), (int)timeValues.size(), timeValues_offset, toString(int(mVDSession->getMaxVolume())).c_str(), 0.0f, 255.0f, ImVec2(0, 30));
		if (mVDSession->getMaxVolume() > 240.0) ImGui::PopStyleColor();

		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		ImGui::SameLine();
		/*		ImGui::Image((void*)mVDSession->getFboRenderedTexture(mVDSession->getFboFragmentShaderIndex(0))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::SameLine();
				ImGui::Image((void*)mVDSession->getMixTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ImGui::SameLine();
				ImGui::Image((void*)mVDSession->getFboRenderedTexture(mVDSession->getFboFragmentShaderIndex(1))->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				*/


		multx = mVDSession->getFloatUniformValueByIndex(mVDSettings->IAUDIOX); // 13
		if (ImGui::SliderFloat("mult x", &multx, 0.01f, 12.0f)) {
			mVDSession->setFloatUniformValueByIndex(13, multx);
		}
		ImGui::SameLine();
		int hue = 0;
		(mVDSession->isAudioBuffered()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3.0f, 0.8f, 0.8f));
		if (ImGui::Button("Wave")) {
			mVDSession->toggleAudioBuffered();
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();
		(mVDSession->getUseLineIn()) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4.0f, 0.8f, 0.8f));
		if (ImGui::Button("LineIn")) {
			mVDSession->toggleUseLineIn();
		}
		ImGui::PopStyleColor(3);
		hue++;
		
		// debug
		ctrl = mVDSettings->IDEBUG;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("Debug")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();


	
		if (ImGui::Button("CreateWarp")) {
			mVDSession->createWarp();
		}	
		hue++;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("Warps")) {
			mToggleShowWarps();
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("Fbos")) {
			mToggleShowFbos();
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

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

		ImGui::SameLine();

		ctrl = mVDSettings->IGLITCH;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("glitch")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		ctrl = mVDSettings->ITOGGLE;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("toggle")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		ctrl = mVDSettings->IINVERT;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("invert")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		ctrl = mVDSettings->IGREYSCALE;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("greyscale")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		// iClear
		ctrl = mVDSettings->ICLEAR;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("clear")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		// iflipv
		ctrl = mVDSettings->IFLIPV;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("FlipV")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		// ifliph
		ctrl = mVDSettings->IFLIPH;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("FlipH")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		// post flip		
		ctrl = mVDSettings->IFLIPPOSTV;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("flipPostV")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
		hue++;
		ImGui::SameLine();

		ctrl = mVDSettings->IFLIPPOSTH;
		(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
		if (ImGui::Button("flipPostH")) {
			toggleValue(ctrl);
		}
		ImGui::PopStyleColor(3);
/*
		if (ImGui::Button("blackout"))
		{
			setFloatValue(1, 0.0);
			setFloatValue(2, 0.0);
			setFloatValue(3, 0.0);
			setFloatValue(4, 0.0);
		}*/

		ImGui::TextColored(ImColor(255, 150, 0), "Mode: %d - %s", mVDSession->getMode(), mVDSession->getModeName(mVDSession->getMode()).c_str());
		ImGui::SameLine();
		ImGui::Text("fp %dx%d f %dx%d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight);
		ImGui::SameLine();
		ImGui::Text("main %dx%d", mVDSettings->mMainWindowWidth, mVDSettings->mMainWindowHeight);
		ImGui::SameLine();
		// windows
		ImGui::Text("render window %dx%d", mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);

		ImGui::SameLine();
		ImGui::Text("Position: %.1f,%.1f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		ImGui::Text("Clic %d", ImGui::GetIO().MouseDown[0]);

		// mouse
		mouseX = getFloatValue(mVDSettings->IMOUSEX);
		if (ImGui::SliderFloat("MouseX", &mouseX, 0.0f, mVDSettings->mFboWidth, "%.4f", 3.0f))
		{
			setFloatValue(mVDSettings->IMOUSEX, mouseX);
		}
		mouseY = getFloatValue(mVDSettings->IMOUSEY);
		if (ImGui::SliderFloat("MouseY", &mouseY, 0.0f, mVDSettings->mFboHeight, "%.4f", 0.3f))
		{
			setFloatValue(mVDSettings->IMOUSEY, mouseY);
		}
		mouseZ ^= ImGui::Button("mouse click");
		if (mouseZ)
		{
			setFloatValue(mVDSettings->IMOUSEZ, 1.0f);
		}
		else
		{
			setFloatValue(mVDSettings->IMOUSEZ, 0.0f);
		}
		ImGui::SameLine();
		ImGui::Text("beat %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBEAT));
		ImGui::SameLine();
		ImGui::Text("bar %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBAR));
		ImGui::SameLine();
		ImGui::Text("bb %d ", mVDSession->getIntUniformValueByIndex(mVDSettings->IBARBEAT));
		ImGui::SameLine();
		ImGui::Text("Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITIME));
		ImGui::SameLine();
		ImGui::Text("Tempo Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->ITEMPOTIME));
		ImGui::SameLine();
		ImGui::Text("Delta Time %.2f", mVDSession->getFloatUniformValueByIndex(mVDSettings->IDELTATIME));
		// LiveOSC Obsolete ImGui::Text("Trk %s %.2f", mVDSettings->mTrackName.c_str(), mVDSettings->liveMeter);
		ImGui::SameLine();
		//			ImGui::Checkbox("Playing", &mVDSettings->mIsPlaying);
		ImGui::Text("Tempo %.2f ", mVDSession->getBpm());


		// line 3
		/*ImGui::RadioButton("Warp", &currentWindowRow1, 0); ImGui::SameLine();
		ImGui::RadioButton("Anim", &currentWindowRow1, 1); ImGui::SameLine();
		ImGui::RadioButton("Mouse", &currentWindowRow1, 2);  ImGui::SameLine();
		ImGui::RadioButton("Render", &currentWindowRow1, 3); ImGui::SameLine();
		ImGui::RadioButton("AudioTempo", &currentWindowRow1, 4); ImGui::SameLine();
		ImGui::RadioButton("Color", &currentWindowRow1, 5); ImGui::SameLine();
		ImGui::RadioButton("Network", &currentWindowRow1, 6); ImGui::SameLine();
		ImGui::RadioButton("Hydra", &currentWindowRow1, 7); ImGui::SameLine();
		ImGui::RadioButton("Midi", &currentWindowRow1, 8);*/

		// modes
		/*for (int m = 0; m < mVDSession->getModesCount(); m++) {
			if (m > 0) ImGui::SameLine();
			switch (m)
			{
			case 0:
				ImGui::Image((void*)mVDSession->getMixetteTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 1:
				ImGui::Image((void*)mVDSession->getMixTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 2:
				ImGui::Image((void*)mVDSession->getRenderTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 3:
				ImGui::Image((void*)mVDSession->getHydraTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 4:
				ImGui::Image((void*)mVDSession->getFboTexture(0)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 5:
				ImGui::Image((void*)mVDSession->getFboTexture(1)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 6:
				ImGui::Image((void*)mVDSession->getFboTexture(2)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			case 7:
				ImGui::Image((void*)mVDSession->getFboTexture(3)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			default:
				ImGui::Image((void*)mVDSession->getMixetteTexture()->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				break;
			}
		}*/
		/*for (int m = 0; m < mVDSession->getModesCount(); m++) {
			if (m > 0) ImGui::SameLine();
			ctrl = mVDSettings->IWEIGHT0 + m;
			float iWeight = mVDSession->getFloatUniformValueByIndex(ctrl);
			sprintf(buf, "W%d##modew", m);
			if (ImGui::DragFloat(buf, &iWeight, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, iWeight);
			}
			if (mVDSession->getMode() == m) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(m / 16.0f, 1.0f, 0.5f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(m / 16.0f, 0.1f, 0.1f));
			}
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(m / 16.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(m / 16.0f, 0.8f, 0.8f));
			ImGui::PopStyleColor(3);
		}
		for (int m = 0; m < mVDSession->getModesCount(); m++) {
			if (m > 0) ImGui::SameLine();

			sprintf(buf, "%s##mode", mVDSession->getModeName(m).c_str());
			if (ImGui::Button(buf)) mVDSession->setMode(m);
			sprintf(buf, "Set mode to %s", mVDSession->getModeName(m).c_str());
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);
		}*/


		const float spacing = 4;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, spacing));

		ImGui::PushID("fbomixes");
		for (int m = 0; m < mVDSession->getFboListSize(); m++)//mVDSession->getModesCount()
		{
			if (m > 0) ImGui::SameLine();
			/*if (mVDSession->getMode() == m) {
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
			ImGui::SameLine();*/
			ctrl = mVDSettings->IWEIGHT0 + m;
			//ctrl = math<int>::min(mVDSettings->IWEIGHT8, mVDSettings->IWEIGHT0 + m);
			float iWeight = mVDSession->getFloatUniformValueByIndex(ctrl);
			ImGui::PushID(m);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(m / 16.0f, 0.5f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor::HSV(m / 16.0f, 0.6f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor::HSV(m / 16.0f, 0.7f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor::HSV(m / 16.0f, 0.9f, 0.9f));
			ImGui::Image((void*)mVDSession->getFboRenderedTexture(m)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			string tooltip = mVDSession->getFboName(m) + " - " + mVDSession->getFboInputTextureName(m);
			sprintf(buf, "%s", tooltip.c_str());
			if (ImGui::IsItemHovered()) ImGui::SetTooltip(buf);


			ImGui::SameLine();
			if (ImGui::VSliderFloat("##v", ImVec2(18, 60), &iWeight, 0.0f, 1.0f, ""))
			{
				setFloatValue(ctrl, iWeight);
			};
			if (ImGui::IsItemActive() || ImGui::IsItemHovered())
				ImGui::SetTooltip("%.3f", iWeight);
			ImGui::PopStyleColor(4);
			ImGui::PopID();
			//ImGui::SameLine();
			//ImGui::TextColored(ImColor(255, 150, 0), "%d - %s", mVDSession->getMode(), mVDSession->getModeName(m).c_str());
		}
		ImGui::PopID();
		ImGui::PopStyleVar();

		ImGui::Indent();

		/*
				ImGui::TextWrapped("Last error: %s", mVDSettings->mErrorMsg.c_str());
				ImGui::TextWrapped("Msg: %s", mVDSettings->mMsg.c_str());
				ImGui::TextWrapped("Midi: %s", mVDSettings->mMidiMsg.c_str());
				ImGui::TextWrapped("WS Msg: %s", mVDSettings->mWebSocketsMsg.c_str());
				ImGui::TextWrapped("OSC Msg: %s", mVDSettings->mOSCMsg.c_str());
				hue++;

				ImGui::RadioButton("Textures", &currentWindowRow2, 0); ImGui::SameLine();
				ImGui::RadioButton("Fbos", &currentWindowRow2, 1); ImGui::SameLine();
				ImGui::RadioButton("Shaders", &currentWindowRow2, 2); ImGui::SameLine();
				ImGui::RadioButton("Blend", &currentWindowRow2, 3); */

		ImGui::PopItemWidth();
	}
	ImGui::End();

	mUIAnimation->Run("Animation");
	/*switch (currentWindowRow1) {
	case 0:

		break;
	case 1:
		// Animation
		mUIAnimation->Run("Animation");
		break;
	case 2:
		// Mouse
		mUIMouse->Run("Mouse");
		break;
	case 3:
		// Render
		mUIRender->Run("Render");
		// Blend
		mUIBlend->Run("Blend");
		break;
	case 4:
		// Audio
		mUIAudio->Run("Audio");
		break;
	case 5:
		// Color
		mUIColor->Run("Color");
		break;
	case 6:
		// Osc
		mUIOsc->Run("Osc");
		break;
	case 7:
		// Websockets
		mUIWebsockets->Run("Websockets");
		break;
	case 8:
		// Midi
		mUIMidi->Run("Midi");
		break;
	}
	mVDSession->blendRenderEnable(currentWindowRow1 == 3);*/
	// Warps
	if (mShowWarps) {
		mUIWarps->Run("Warps");
	}
	// textures
	//mUITextures->Run("Textures");
	// Fbos
	if (mShowFbos) {
		mUIFbos->Run("Fbos");
	}
	// Shaders
	//mUIShaders->Run("Shaders");


}
