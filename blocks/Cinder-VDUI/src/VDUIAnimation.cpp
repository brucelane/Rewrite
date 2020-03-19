#include "VDUIAnimation.h"

using namespace videodromm;

VDUIAnimation::VDUIAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	// zoom
	minZoom = getMinUniformValueByIndex(12);
	maxZoom = getMaxUniformValueByIndex(12);
	for (int c = 0; c < 128; c++)
	{
		localValues[c] = mVDSession->getFloatUniformValueByIndex(c);
	}
	// contour
	minContour = getMinUniformValueByIndex(mVDSettings->ICONTOUR);
	maxContour = getMaxUniformValueByIndex(mVDSettings->ICONTOUR);
	iResolutionX = (int)mVDSession->getFloatUniformValueByIndex(mVDSettings->IRESX);//(int)getFloatValueByName("iResolutionX");
	iResolutionY = (int)mVDSession->getFloatUniformValueByIndex(mVDSettings->IRESY);//(int)getFloatValueByName("iResolutionY");
	iOutW = getIntValue(mVDSettings->IOUTW);
	iOutH = getIntValue(mVDSettings->IOUTH);
	iBarBeat = getIntValue(mVDSettings->IBARBEAT);
}
VDUIAnimation::~VDUIAnimation() {

}

void VDUIAnimation::Run(const char* title) {
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH * 3.4), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow1), ImGuiSetCond_Once);
	int hue = 0;
	ImGui::Begin("Animation", NULL, ImVec2(0, 0), ImGui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		
		if (ImGui::CollapsingHeader("Animation", NULL, true, true))
		{
			// iChromatic
			ctrl = mVDSettings->ICHROMATIC;
			if (ImGui::Button("a##chromatic")) {
				toggleAuto(ctrl);
			}
			ImGui::SameLine();
			if (ImGui::Button("f##chromatic")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##chromatic")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("m##chromatic")) { toggleMid(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##chromatic")) { toggleTreble(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##chromatic")) {
				resetAutoAnimation(ctrl);
			}
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("chromatic", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				mVDSession->setFloatUniformValueByIndex(ctrl, localValues[ctrl]);
			}

			// ratio
			ctrl = mVDSettings->IRATIO;
			if (ImGui::Button("a##ratio")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##ratio")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##ratio")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("m##ratio")) { toggleMid(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##ratio")) { toggleTreble(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##ratio")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("ratio/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}

			// sobel
			ctrl = mVDSettings->ISOBEL;
			if (ImGui::Button("a##sobel")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##sobel")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##sobel")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("m##sobel")) { toggleMid(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##sobel")) { toggleTreble(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##sobel")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("sobel/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// exposure
			ctrl = mVDSettings->IEXPOSURE;
			if (ImGui::Button("a##exposure")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##exposure")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##exposure")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("m##exposure")) { toggleMid(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##exposure")) { toggleTreble(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##exposure")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("exposure", &localValues[ctrl], 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// zoom
			ctrl = mVDSettings->IZOOM;
			if (ImGui::Button("a##zoom"))
			{
				toggleAuto(ctrl);
			}
			ImGui::SameLine();
			if (ImGui::Button("f##zoom")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("b##zoom")) { toggleBass(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("m##zoom")) { toggleMid(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("t##zoom")) { toggleTreble(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##zoom")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("zoom", &localValues[ctrl], 0.1f, minZoom, maxZoom))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			/* todo crash:
			ImGui::DragFloat("minzm", &minZoom, 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
			ImGui::SameLine();
			ImGui::DragFloat("maxzm", &maxZoom, 0.1f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
			*/
			//  speed sequence
// 20190727 TODO CHECK IF NEEDED
			/*ctrl = mVDSettings->ISPEED;
			if (ImGui::Button("a##speed")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##speed")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##speed")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("speed", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			} */
			// rotation speed 
			ctrl = mVDSettings->IROTATIONSPEED;
			if (ImGui::Button("a##rotationspeed")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##rotationspeed")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##rotationspeed")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("rotationSpeed", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// badTv
			/*ctrl = mVDSettings->IBADTV;
			if (ImGui::Button("a##badtv")) { toggleAuto(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("f##badtv")) { toggleTempo(ctrl); }
			ImGui::SameLine();
			if (ImGui::Button("x##badtv")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("badTv", &localValues[ctrl], 0.01f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}*/
			// iPixelX
			ctrl = mVDSettings->IPIXELX;
			if (ImGui::Button("x##iPixelX")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("iPixelX/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// iPixelY
			ctrl = mVDSettings->IPIXELY;
			if (ImGui::Button("x##iPixelY")) { resetAutoAnimation(ctrl); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("iPixelY/min/max", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// iBarBeat
			ctrl = mVDSettings->IBARBEAT;
			if (ImGui::Button("x##iBarBeat")) { iBarBeat = 1; setIntValue(ctrl, 1); }
			ImGui::SameLine();
			if (ImGui::SliderInt("iBarBeat", &iBarBeat, 1, 50))
			{
				setIntValue(ctrl, iBarBeat);
			}
			/*sprintf(buf, "XorY");
			mVDSettings->iXorY ^= ImGui::Button(buf);*/

			// steps
			ctrl = mVDSettings->ISTEPS;
			if (ImGui::Button("x##steps")) { localValues[ctrl] = 16.0f; setFloatValue(ctrl, localValues[ctrl]); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("steps", &localValues[ctrl], 1.0f, 128.0f))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// pixelate
			ctrl = mVDSettings->IPIXELATE;
			if (ImGui::Button("x##pixelate")) { localValues[ctrl] = 1.0f; setFloatValue(ctrl, localValues[ctrl]); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("pixelate", &localValues[ctrl], 0.01f, 1.0f))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
			// trixels
			ctrl = mVDSettings->ITRIXELS;
			if (ImGui::Button("x##trixels")) { localValues[ctrl] = 0.0f; setFloatValue(ctrl, localValues[ctrl]); }
			ImGui::SameLine();
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("trixels", &localValues[ctrl], 0.00f, 1.0f))
			{
				setFloatValue(ctrl, localValues[ctrl]);
			}
		}

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
			ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Tempo", NULL, true, true))
		{
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
			// iTimeFactor
			ctrl = mVDSettings->ITIMEFACTOR;
			localValues[ctrl] = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::SliderFloat("timeFactor", &localValues[ctrl], getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl)))
			{
				mVDSession->setFloatUniformValueByIndex(ctrl, localValues[ctrl]);
			}

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
		if (ImGui::CollapsingHeader("Midi", NULL, true))
		{
			sprintf(buf, "Enable");
			if (ImGui::Button(buf)) mVDSession->midiSetup();
			if (ImGui::CollapsingHeader("MidiIn", "20", true, true))
			{
				ImGui::Columns(2, "datain", true);
				ImGui::Text("Name"); ImGui::NextColumn();
				ImGui::Text("Connect"); ImGui::NextColumn();
				ImGui::Separator();
				for (int i = 0; i < mVDSession->getMidiInPortsCount(); i++)
				{
					if (mVDSession->getMidiInPortName(i) != "Ableton Push 2 1") {
						ImGui::Text(mVDSession->getMidiInPortName(i).c_str()); ImGui::NextColumn();

						if (mVDSession->isMidiInConnected(i))
						{
							sprintf(buf, "Disconnect %d", i);
						}
						else
						{
							sprintf(buf, "Connect %d", i);
						}

						if (ImGui::Button(buf))
						{
							if (mVDSession->isMidiInConnected(i))
							{
								mVDSession->closeMidiInPort(i);
							}
							else
							{
								mVDSession->openMidiInPort(i);
							}
						}
						ImGui::NextColumn();
						ImGui::Separator();
					}
				}
				ImGui::Columns(1);
			}
			// Midi out
			if (ImGui::CollapsingHeader("MidiOut", "20", true, true))
			{
				ImGui::Columns(2, "dataout", true);
				ImGui::Text("Name"); ImGui::NextColumn();
				ImGui::Text("Connect"); ImGui::NextColumn();
				ImGui::Separator();
				for (int i = 0; i < mVDSession->getMidiOutPortsCount(); i++)
				{
					ImGui::Text(mVDSession->getMidiOutPortName(i).c_str()); ImGui::NextColumn();

					if (mVDSession->isMidiOutConnected(i))
					{
						sprintf(buf, "Disconnect  %d", i);
					}
					else
					{
						sprintf(buf, "Connect  %d", i);
					}

					if (ImGui::Button(buf))
					{
						if (mVDSession->isMidiOutConnected(i))
						{
							mVDSession->closeMidiOutPort(i);
						}
						else
						{
							mVDSession->openMidiOutPort(i);
						}
					}
					ImGui::NextColumn();
					ImGui::Separator();
				}
				ImGui::Columns(1);
			}
		}

		if (ImGui::CollapsingHeader("Websockets", NULL, true))
		{
			// websockets
			if (mVDSettings->mIsWebSocketsServer)
			{
				ImGui::Text("WS Server %s%s:%d/%s/%s", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort, mVDSettings->mWebSocketsRoom.c_str(), mVDSettings->mWebSocketsNickname.c_str());
				if (ImGui::Button("srv->clt"))
				{
					mVDSettings->mIsWebSocketsServer = false;
					mVDSession->wsConnect();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a WS client");
			}
			else
			{
				ImGui::Text("WS Client %s%s:%d/%s/%s", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort, mVDSettings->mWebSocketsRoom.c_str(), mVDSettings->mWebSocketsNickname.c_str());
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
			// host
			static char host[128] = "127.0.0.1";
			std::copy(mVDSettings->mWebSocketsHost.begin(), (mVDSettings->mWebSocketsHost.size() >= 128 ? mVDSettings->mWebSocketsHost.begin() + 128 : mVDSettings->mWebSocketsHost.end()), host);
			if (ImGui::InputText("address", host, IM_ARRAYSIZE(host)))
			{
				mVDSettings->mWebSocketsHost = host;
			}
			// port
			static int port = mVDSettings->mWebSocketsPort;
			if (ImGui::InputInt("port", &port)) mVDSettings->mWebSocketsPort = port;
			// room
			static char room[128] = "roomtest";
			std::copy(mVDSettings->mWebSocketsRoom.begin(), (mVDSettings->mWebSocketsRoom.size() >= 128 ? mVDSettings->mWebSocketsRoom.begin() + 128 : mVDSettings->mWebSocketsRoom.end()), room);

			if (ImGui::InputText("room", room, IM_ARRAYSIZE(room)))
			{
				mVDSettings->mWebSocketsRoom = room;
			}
			// nickname
			static char nick[128] = "bruce";
			std::copy(mVDSettings->mWebSocketsNickname.begin(), (mVDSettings->mWebSocketsNickname.size() >= 128 ? mVDSettings->mWebSocketsNickname.begin() + 128 : mVDSettings->mWebSocketsNickname.end()), nick);

			if (ImGui::InputText("nick", nick, IM_ARRAYSIZE(nick)))
			{
				mVDSettings->mWebSocketsNickname = nick;
			}

			//ImGui::PushItemWidth(mVDSettings->uiLargeW/3); // useless?
			ImGui::TextWrapped(">%s", mVDSettings->mWebSocketsMsg.c_str());
			//ImGui::PopItemWidth();

		}
		if (ImGui::CollapsingHeader("OSC", NULL, true))
		{
			if (mVDSettings->mOSCEnabled) {
				ImGui::Text("OSC enabled");
				if (mVDSettings->mIsOSCSender) {
					/*ImGui::Text("Sending to host %s", mVDSettings->mOSCDestinationHost.c_str());
					ImGui::SameLine();
					ImGui::Text(" on port %d", mVDSettings->mOSCDestinationPort);
					static char str0[128] = "/live/play";
					static int i0 = 0;
					static float f0 = 0.0f;
					ImGui::InputText("address", str0, IM_ARRAYSIZE(str0));
					ImGui::InputInt("track", &i0);
					ImGui::InputFloat("clip", &f0, 0.01f, 1.0f);
					if (ImGui::Button("Send")) { mVDSession->sendOSCIntMessage(str0, i0); }*/
				}
				else {
					ImGui::Text(" Receiving on port %d", mVDSettings->mOSCReceiverPort);
				}
			}
			else {
				ImGui::Text("OSC disabled");

				if (mVDSettings->mIsOSCSender) {
					if (ImGui::Button("sender->receiver"))
					{
						mVDSettings->mIsOSCSender = false;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a OSC receiver");

					static char host[128] = "127.0.0.1";
					std::copy(mVDSettings->mOSCDestinationHost.begin(), (mVDSettings->mOSCDestinationHost.size() >= 128 ? mVDSettings->mOSCDestinationHost.begin() + 128 : mVDSettings->mOSCDestinationHost.end()), host);
					static int senderPort = mVDSettings->mOSCDestinationPort;
					ImGui::InputText("destination address", host, IM_ARRAYSIZE(host));
					if (ImGui::InputInt("destination port", &senderPort)) mVDSettings->mOSCDestinationPort = senderPort;
				}
				else {
					if (ImGui::Button("receiver->sender"))
					{
						mVDSettings->mIsOSCSender = true;
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a OSC sender");

					static int receiverPort = mVDSettings->mOSCReceiverPort;
					if (ImGui::InputInt("receiver port", &receiverPort)) mVDSettings->mOSCReceiverPort = receiverPort;
				}
				if (ImGui::Button("Enable"))
				{
					mVDSettings->mOSCEnabled = true;
					/*if (mVDSettings->mIsOSCSender) {
						mVDSession->setupOSCSender();
					}
					else {
						mVDSession->setupOSCReceiver();
					}*/
				}
			}
			ImGui::Text(">%s", mVDSettings->mOSCMsg.c_str());

			/* TODO if useful ImGui::Text("Sending to 2nd host %s", mVDSettings->mOSCDestinationHost2.c_str());
			ImGui::SameLine();
			ImGui::Text(" on port %d", mVDSettings->mOSCDestinationPort2);*/
		}
		if (ImGui::CollapsingHeader("Render", NULL, true, true))
		{
		
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
			if (ImGui::Button("x##iouth")) { iOutH = 720; setIntValue(ctrl, iOutH); }
			ImGui::SameLine();
			if (ImGui::SliderInt("iOutH", &iOutH, 480, 2000))
			{
				setIntValue(ctrl, iOutH);
			}
			// iResolution
			ctrl = mVDSettings->IRESX;
			//iResolutionX = getFloatValueByName("iResolutionX");
			if (ImGui::Button("x##iresx")) { iResolutionX = 1280; setFloatValue(ctrl, 1280); }
			ImGui::SameLine();
			if (ImGui::SliderInt("iResolutionX", &iResolutionX, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, (float)iResolutionX);
			}
			ctrl = mVDSettings->IRESY;
			//iResolutionY = getFloatValueByName("iResolutionY");
			if (ImGui::Button("x##iresy")) { iResolutionY = 720; setFloatValue(ctrl, 720); }
			ImGui::SameLine();
			if (ImGui::SliderInt("iResolutionY", &iResolutionY, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
			{
				setFloatValue(ctrl, (float)iResolutionY);
			}

			hue++;
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

			// debug
			/*ctrl = mVDSettings->IDEBUG;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
			if (ImGui::Button("debug")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;*/

			// iVignette
			ctrl = mVDSettings->IVIGN;
			(getBoolValue(ctrl)) ? ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue / 16.0f, 1.0f, 0.5f)) : ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.1f, 0.1f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hue / 16.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hue / 16.0f, 0.8f, 0.8f));
			if (ImGui::Button("vignette")) {
				toggleValue(ctrl);
			}
			ImGui::PopStyleColor(3);
			hue++;
			ctrl = mVDSettings->IVAMOUNT;
			iVAmount = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("Amount", &iVAmount, 0.001f, 0.0f, 1.0f))
			{
				setFloatValue(ctrl, iVAmount);
			}
			ctrl = mVDSettings->IVFALLOFF;
			iVFallOff = mVDSession->getFloatUniformValueByIndex(ctrl);
			if (ImGui::DragFloat("FallOff", &iVFallOff, 0.001f, 0.0f, 0.99f))
			{
				setFloatValue(ctrl, iVFallOff);
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
				setFloatValue(ctrl, contour);
			}
			ImGui::DragFloat("mincr", &minContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
			ImGui::SameLine();
			ImGui::DragFloat("maxcr", &maxContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));


			/*if (ImGui::Button("Create Window")) {
				mVDSession->createWindow();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Window")) {
				mVDSession->deleteWindow();
			}*/


			//ImGui::SameLine();
			//ImGui::Text("solo %d", mVDSession->getSolo());

			ImGui::PopItemWidth();
		}

		if (ImGui::CollapsingHeader("Color", NULL, true))
		{
			ImGui::PushItemWidth(200.0f);
			// foreground color
			color[0] = getFloatValue(mVDSettings->IFR);
			color[1] = getFloatValue(mVDSettings->IFG);
			color[2] = getFloatValue(mVDSettings->IFB);
			color[3] = getFloatValue(mVDSettings->IFA);
			ImGui::ColorEdit4("f", color);
			for (int i = 0; i < 4; i++)
			{
				if (getFloatValue(i + mVDSettings->IFR) != color[i])
				{
					setFloatValue(i + mVDSettings->IFR, color[i]);
				}
			}

			// background color
			backcolor[0] = getFloatValue(mVDSettings->IBR);
			backcolor[1] = getFloatValue(mVDSettings->IBG);
			backcolor[2] = getFloatValue(mVDSettings->IBB);
			ImGui::ColorEdit3("g", backcolor);
			for (int i = 0; i < 3; i++)
			{
				if (getFloatValue(i + mVDSettings->IBR) != backcolor[i])
				{
					setFloatValue(i + mVDSettings->IBR, backcolor[i]);
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
				setFloatValue(ctrl, localValues[ctrl]);
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
				setFloatValue(ctrl, localValues[ctrl]);
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
				setFloatValue(ctrl, localValues[ctrl]);
			}

			ImGui::PopItemWidth();
		}

		
	}
	ImGui::End();
}
