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
	// stores the pointer to the VDUIRender instance
	typedef std::shared_ptr<class VDUIRender> VDUIRenderRef;

	class VDUIRender
	{
	public:
		VDUIRender(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIRenderRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIRender>(new VDUIRender(aVDSettings, aVDSession));
		}
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Session
		VDSessionRef					mVDSession;
		int								getIntValue(unsigned int aCtrl) {
			return mVDSession->getIntUniformValueByIndex(aCtrl);
		}
		float							getFloatValueByName(const string& aCtrlName) {
			return mVDSession->getFloatUniformValueByName(aCtrlName);
		}
		float							getFloatValue(unsigned int aCtrl) {
			return mVDSession->getFloatUniformValueByIndex(aCtrl);
		}
		void							setFloatValue(unsigned int aCtrl, float aValue) {
			mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
		}
		void							setIntValue(unsigned int aCtrl, int aValue) {
			mVDSession->setIntUniformValueByIndex(aCtrl, aValue);
		}
		void							toggleAuto(unsigned int aCtrl) {
			mVDSession->toggleAuto(aCtrl);
		}
		void							toggleTempo(unsigned int aCtrl) {
			mVDSession->toggleTempo(aCtrl);
		}
		void							toggleSpoutSender() {
			//mVDSettings->mSpoutSender = !mVDSettings->mSpoutSender;
		}
		bool							getBoolValue(unsigned int aCtrl) {
			return mVDSession->getBoolUniformValueByIndex(aCtrl);
		}
		void							toggleValue(unsigned int aCtrl) {
			mVDSession->toggleValue(aCtrl);
		}
		void							resetAutoAnimation(unsigned int aCtrl) {
			mVDSession->resetAutoAnimation(aCtrl);
		}
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMinUniformValueByIndex(aIndex);
		}
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMaxUniformValueByIndex(aIndex);
		}
		// imgui
		int								ctrl;
		float							contour, iVAmount, iVFallOff;
		int								iResolutionX, iResolutionY;
		int								iOutW, iOutH;
		// contour
		float							minContour;
		float							maxContour;
		// imgui
		char						buf[64];
		int							xPos, yPos;
	};
}