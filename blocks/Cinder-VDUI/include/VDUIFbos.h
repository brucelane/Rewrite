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
	// stores the pointer to the VDUIFbos instance
	typedef std::shared_ptr<class VDUIFbos> VDUIFbosRef;

	class VDUIFbos
	{
	public:
		VDUIFbos(VDSettingsRef aVDSettings, VDSessionRef aVDSession);
		static VDUIFbosRef	create(VDSettingsRef aVDSettings, VDSessionRef aVDSession)
		{
			return shared_ptr<VDUIFbos>(new VDUIFbos(aVDSettings, aVDSession));
		}
		void    Run(const char* title);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Session
		VDSessionRef					mVDSession;
		// imgui
		char							buf[64];
		int								xPos, yPos;
		int								xStep = 1;
		int								yStep = 1;
		// sequence
		int								playheadPositions[12];
		float							speeds[12];
		// uniforms
		unsigned int					ctrl;
		map<int, float>					localValues;
		float							getValue(unsigned int aCtrl, unsigned int aFboIndex) {
			float rtn;
			if (mVDSession->getGlobal(aFboIndex) || aCtrl == 0) {
				rtn = mVDSession->getFloatUniformValueByIndex(aCtrl);
			}
			else {
				rtn = mVDSession->getFboFloatUniformValueByIndex(aCtrl, aFboIndex);
			}
			return rtn;
		};
		void							setValue(unsigned int aCtrl, unsigned int aFboIndex, float aValue) {
			if (mVDSession->getGlobal(aFboIndex)) {
				mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
			}
			else {
				mVDSession->setFboFloatUniformValueByIndex(aCtrl, aFboIndex, aValue);
			}
			
		};
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMinUniformValueByIndex(aIndex);
		};
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDSession->getMaxUniformValueByIndex(aIndex);
		};
		bool							getBoolValue(unsigned int aCtrl, unsigned int aFboIndex) {
			return (mVDSession->getGlobal(aFboIndex)) ? mVDSession->getBoolUniformValueByIndex(aCtrl) : mVDSession->getFboBoolUniformValueByIndex(aCtrl, aFboIndex);
		};
		int								getIntValue(unsigned int aCtrl, unsigned int aFboIndex) {
			return mVDSession->getFboIntUniformValueByIndex(aCtrl, aFboIndex);
		};
		void							toggleValue(unsigned int aCtrl, unsigned int aFboIndex) {
			mVDSession->toggleFboValue(aCtrl, aFboIndex);
		};
		// mouse
		float						mouseX, mouseY;
		bool						mouseZ;
		bool						mShowRenderedTexture = true;
		bool						mShowInputTexture = true;
	};
}