#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"

// UITextures
//#include "VDUITextures.h"
// UIFbos
#include "VDUIFbos.h"
// Animation
#include "VDUIAnimation.h"
// Midi
//#include "VDUIMidi.h"
// Audio
//#include "VDUIAudio.h"
// Color
//#include "VDUIColor.h"
// Tempo
//#include "VDUITempo.h"
// Blend
//#include "VDUIBlend.h"
// Websockets
//#include "VDUIWebsockets.h"
// Osc
//#include "VDUIOsc.h"
// Mouse
//#include "VDUIMouse.h"
// Shaders
//#include "VDUIShaders.h"
// Render
//#include "VDUIRender.h"
// Warps
#include "VDUIWarps.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDConsole instance
	typedef std::shared_ptr<class VDUI> VDUIRef;

	class VDUI
	{
	public:
		VDUI(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUI>(new VDUI(aVDSettings, aVDSession));
		}

		void    Run(const char* title, unsigned int fps);
		void resize() {
			mIsResizing = true;
			// disconnect ui window and io events callbacks
			ImGui::disconnectWindow(getWindow());
		}
		bool	isReady() { return !mIsResizing; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Session
		VDSessionRef				mVDSession;

		// UITextures
		//VDUITexturesRef				mUITextures;
		//bool						showUITextures;
		// UIFbos
		VDUIFbosRef					mUIFbos;
		bool						showUIFbos;
		bool						mShowFbos;
		// UIAnimation
		VDUIAnimationRef			mUIAnimation;
		bool						showUIAnimation;
		// UIMidi
		/*VDUIMidiRef					mUIMidi;
		bool						showUIMidi;
		// UIAudio
		VDUIAudioRef				mUIAudio;
		bool						showUIAudio;
		// UIColor
		VDUIColorRef				mUIColor;
		bool						showUIColor;
		// UITempo
		VDUITempoRef				mUITempo;
		bool						showUITempo;
		// UIBlend
		VDUIBlendRef				mUIBlend;
		bool						showUIBlend;
		// UIMouse
		VDUIMouseRef				mUIMouse;
		bool						showUIMouse;
		// UIOsc
		VDUIOscRef					mUIOsc;
		bool						showUIOsc;
		// UIWebsockets
		VDUIWebsocketsRef			mUIWebsockets;
		bool						showUIWebsockets;
		// UIShaders
		VDUIShadersRef				mUIShaders;
		bool						showUIShaders;
		// UIRender
		VDUIRenderRef				mUIRender;
		bool						showUIRender;*/
		// UIWarps
		VDUIWarpsRef				mUIWarps;
		bool						showUIWarps;
		bool						mShowWarps;

		// imgui
		char						buf[64];
		bool						mIsResizing;
		float						color[4];
		float						backcolor[4];
		float						multx;
		bool						mouseGlobal;
		int							ctrl;
		float						contour, iVAmount, iVFallOff, iWeight0, iWeight1, iWeight2, iWeight3, iWeight4, iWeight5, iWeight6, iWeight7;

		bool getBoolValue(unsigned int aCtrl) {
			return mVDSession->getBoolUniformValueByIndex(aCtrl);
		}
		void toggleValue(unsigned int aCtrl) {
			mVDSession->toggleValue(aCtrl);
		}
		void mToggleShowWarps() {
			mShowWarps = !mShowWarps;
		}
		void mToggleShowFbos() {
			mShowFbos = !mShowFbos;
		}
		void setFloatValue(unsigned int aCtrl, float aValue) {
			mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
		}
		float getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMinUniformValueByIndex(aIndex);
		}
		float getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMaxUniformValueByIndex(aIndex);
		}
		float							getFloatValue(unsigned int aCtrl) {
			return mVDSession->getFloatUniformValueByIndex(aCtrl);
		};
		// mouse
		float						mouseX, mouseY;
		bool						mouseZ;
	};
}