#include "VDUIColor.h"

using namespace videodromm;

VDUIColor::VDUIColor(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	for (int c = 0; c < 28; c++)
	{
		localValues[c] = 1.0f;
	}

}
VDUIColor::~VDUIColor() {

}
float VDUIColor::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIColor::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIColor::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIColor::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIColor::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIColor::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIColor::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIColor::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH * 1.76), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);

	ImGui::Begin("Color Ws", NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
	{
		if (ImGui::CollapsingHeader("Color", NULL, true, true))
		{
			ImGui::PushItemWidth(200.0f);
			// foreground color
			color[0] = getValue(mVDSettings->IFR);
			color[1] = getValue(mVDSettings->IFG);
			color[2] = getValue(mVDSettings->IFB);
			color[3] = getValue(mVDSettings->IFA);
			ImGui::ColorEdit4("f", color);
			for (int i = 0; i < 4; i++)
			{
				if (getValue(i + mVDSettings->IFR) != color[i])
				{
					setValue(i + mVDSettings->IFR, color[i]);
				}
			}

			// background color
			backcolor[0] = getValue(mVDSettings->IBR);
			backcolor[1] = getValue(mVDSettings->IBG);
			backcolor[2] = getValue(mVDSettings->IBB);
			ImGui::ColorEdit3("g", backcolor);
			for (int i = 0; i < 3; i++)
			{
				if (getValue(i + mVDSettings->IBR) != backcolor[i])
				{
					setValue(i + mVDSettings->IBR, backcolor[i]);
				}

			}
			ImGui::PopItemWidth();
			ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			// color multipliers
			// red x
			ctrl = mVDSettings->IFRX;
			if (ImGui::Button("a##redx")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##redx")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##redx")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("red x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// green x
			ctrl = mVDSettings->IFGX;
			if (ImGui::Button("a##greenx")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##greenx")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##greenx")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("green x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}
			// blue x
			ctrl = mVDSettings->IFBX;
			if (ImGui::Button("a##bluex")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##bluex")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##bluex")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("blue x", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setValue(ctrl, localValues[ctrl]);
			}

			ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Websockets", NULL, true, true))
		{
			// websockets
			if (mVDSettings->mIsWebSocketsServer)
			{
				ImGui::Text("WS Server %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
				if (ImGui::Button("srv->clt"))
				{
					mVDSettings->mIsWebSocketsServer = false;
					mVDSession->wsConnect();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a WS client");
			}
			else
			{
				ImGui::Text("WS Client %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
				if (ImGui::Button("clt->srv"))
				{
					mVDSettings->mIsWebSocketsServer = true;
					mVDSession->wsConnect();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a WS server");
			}
			ImGui::SameLine();
			// toggle secure protocol
			sprintf(buf, "%s", mVDSettings->mWebSocketsProtocol.c_str());
			if (ImGui::Button(buf))
			{
				if (mVDSettings->mWebSocketsProtocol == "ws://") {
					mVDSettings->mWebSocketsProtocol = "wss://";
				}
				else {
					mVDSettings->mWebSocketsProtocol = "ws://";
				}
				mVDSession->wsConnect();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change WS protocol");
			ImGui::SameLine();
			if (ImGui::Button("Connect")) { mVDSession->wsConnect(); }
			ImGui::SameLine();
			if (ImGui::Button("Ping")) { mVDSession->wsPing(); }
			static char host[128] = "127.0.0.1";
			std::copy(mVDSettings->mWebSocketsHost.begin(), (mVDSettings->mWebSocketsHost.size() >= 128 ? mVDSettings->mWebSocketsHost.begin() + 128 : mVDSettings->mWebSocketsHost.end()), host);

			static int port = mVDSettings->mWebSocketsPort;
			if (ImGui::InputText("address", host, IM_ARRAYSIZE(host)))
			{
				mVDSettings->mWebSocketsHost = host; // CHECK if ok
			}
			if (ImGui::InputInt("port", &port)) mVDSettings->mWebSocketsPort = port;
			//ImGui::PushItemWidth(mVDSettings->uiLargeW/3); // useless?
			ImGui::TextWrapped(">%s", mVDSettings->mWebSocketsMsg.c_str());
			//ImGui::PopItemWidth();

		}
	}
	ImGui::End();
}
