#pragma once

#include "cinder/app/App.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDUIAnimation instance
	typedef std::shared_ptr<class VDUIAnimation> VDUIAnimationRef;
	typedef std::shared_ptr<class VDUIOsc> VDUIOscRef;
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))
	class VDUIAnimation
	{
	public:
		VDUIAnimation(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIAnimationRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIAnimation>(new VDUIAnimation(aVDSettings, aVDSession));
		}
		~VDUIAnimation();
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Session
		VDSessionRef					mVDSession;

		// imgui
		float							getFloatValue(unsigned int aCtrl) {
			return mVDSession->getFloatUniformValueByIndex(aCtrl);
		};
		void							setFloatValue(unsigned int aCtrl, float aValue) {
			mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
		};
		float							getFloatValueByName(const string& aCtrlName) {
			return mVDSession->getFloatUniformValueByName(aCtrlName);
		}

		int								getIntValue(unsigned int aCtrl) {
			return mVDSession->getIntUniformValueByIndex(aCtrl);
		}

		void							toggleValue(unsigned int aCtrl) {
			mVDSession->toggleValue(aCtrl);
		};
		void							toggleAuto(unsigned int aCtrl) {
			mVDSession->toggleAuto(aCtrl);
		};
		void							toggleTempo(unsigned int aCtrl) {
			mVDSession->toggleTempo(aCtrl);
		};
		void							toggleBass(unsigned int aCtrl) {
			//mVDSession->toggleBass(aCtrl);
		};
		void							toggleMid(unsigned int aCtrl) {
			//mVDSession->toggleMid(aCtrl);
		};
		void							toggleTreble(unsigned int aCtrl) {
			//mVDSession->toggleTreble(aCtrl);
		};
		bool							getBoolValue(unsigned int aCtrl) {
			return mVDSession->getBoolUniformValueByIndex(aCtrl);
		};
		void							resetAutoAnimation(unsigned int aCtrl) {
			mVDSession->resetAutoAnimation(aCtrl);
		};
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMinUniformValueByIndex(aIndex);
		};
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMaxUniformValueByIndex(aIndex);
		};

		void							setIntValue(unsigned int aCtrl, int aValue) {
			mVDSession->setIntUniformValueByIndex(aCtrl, aValue);
		}

		unsigned int					ctrl;
		map<int, float>					localValues;
		float							f = 0.0f;
		char							buf[64];
		// zoom
		float							minZoom;
		float							maxZoom;
		// tempo
		void							toggleSpinalTap() { spinalTap = !spinalTap; };
		bool							spinalTap = false;

		// render
		float							contour, iVAmount, iVFallOff;
		int								iResolutionX, iResolutionY;
		int								iOutW, iOutH, iBarBeat;
		// contour
		float							minContour;
		float							maxContour;
		// color
		float							color[4];
		float							backcolor[4];
	};
}