/*
	VDUtil
	Global utilities
*/

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

// parameters
#include "VDSettings.h"
// logger
#include "VDLog.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDUtils instance
	typedef std::shared_ptr<class VDUtils> VDUtilsRef;

	class VDUtils {
	public:
		VDUtils(VDSettingsRef aVDSettings);
		static VDUtilsRef	create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDUtils>(new VDUtils(aVDSettings));
		}
		// TODO move to private when shadamixa upgraded
		void						splitWarp(int fboWidth, int fboHeight);
		int							getWindowsResolution();
		float						formatFloat(float f);


		Area						getSrcAreaLeftOrTop() {
			return mSrcAreaLeftOrTop;
		};
		Area						getSrcAreaRightOrBottom() {
			return mSrcAreaRightOrBottom;
		};
		void						moveX1LeftOrTop(int x1) {
			x1LeftOrTop = x1;
			mSrcAreaLeftOrTop = Area(x1LeftOrTop, y1LeftOrTop, x2LeftOrTop, y2LeftOrTop);
		};
		void						moveY1LeftOrTop(int y1) {
			y1LeftOrTop = y1;
			mSrcAreaLeftOrTop = Area(x1LeftOrTop, y1LeftOrTop, x2LeftOrTop, y2LeftOrTop);
		};
		// file system
		fs::path					getPath(string path) {
			fs::path p = app::getAssetPath("");
			if (path.length() > 0) { p += fs::path("/" + path); }
			return p;
		};
		string						getFileNameFromFullPath(string path) {
			fs::path fullPath = path;
			return fullPath.filename().string();
		};
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		Area						mSrcAreaLeftOrTop; // left or top
		Area						mSrcAreaRightOrBottom; // right or bottom
		int							x1LeftOrTop;
		int							y1LeftOrTop;
		int							x2LeftOrTop;
		int							y2LeftOrTop;
		int							x1RightOrBottom;
		int							y1RightOrBottom;
		int							x2RightOrBottom;
		int							y2RightOrBottom;

	};
}

