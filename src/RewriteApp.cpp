/*
 Copyright (c) 2013-2020, Bruce Lane - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Using Cinder-Warping from Paul Houx.

 Cinder-Warping is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Cinder-Warping is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "cinder/Rand.h"
 // json
#include "cinder/Json.h"
//#include "Warp.h"
#include "VDSession.h"
// Spout
#include "CiSpoutOut.h"
// Video
//#include "ciWMFVideoPlayer.h"
// UI
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS 1
#include "VDUI.h"
#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))
using namespace ci;
using namespace ci::app;
using namespace videodromm;
using namespace std;

class RewriteApp : public App {
public:
	static void prepare(Settings *settings);
	RewriteApp();
	//void setup() override;
	void cleanup() override;
	void update() override;
	void draw() override;
	void resize() override;
	void mouseMove(MouseEvent event) override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;
	void fileDrop(FileDropEvent event) override;
private:
	// Settings
	VDSettingsRef					mVDSettings;
	// Session
	VDSessionRef					mVDSession;
	// UI
	VDUIRef							mVDUI;
	// video
	/*ciWMFVideoPlayer				mVideo;
	float							mVideoPos;
	float							mVideoDuration;
	bool							mIsVideoLoaded;*/


	bool							mFadeInDelay = true;
	//void							saveWarps();
	void							toggleCursorVisibility(bool visible);
	SpoutOut 						mSpoutOut;
};

void RewriteApp::prepare(Settings *settings)
{
	settings->setWindowSize(1440, 900);
}

RewriteApp::RewriteApp() : mSpoutOut("rewrite", app::getWindowSize())
{

	// Settings
	mVDSettings = VDSettings::create("Rewrite");
	// Session
	mVDSession = VDSession::create(mVDSettings);
	mVDSession->getWindowsResolution();
	toggleCursorVisibility(mVDSettings->mCursorVisible);
	mVDSession->toggleUI();
	mVDSession->setMode(1);
	// sos
	//mVDSession->setBpm(160.0f);
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IMOUSEX, 0.27710f);
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IMOUSEY, 0.5648f);
	// sos only mVDSession->setFloatUniformValueByIndex(mVDSettings->IEXPOSURE, 1.93f);
	mFadeInDelay = true;
	// UI
	mVDUI = VDUI::create(mVDSettings, mVDSession);
	/*fs::path texFileOrPath = getAssetPath("") / mVDSettings->mAssetsPath / "accueil.mp4";
	if (fs::exists(texFileOrPath)) {
		string ext = "";
		int dotIndex = texFileOrPath.filename().string().find_last_of(".");
		if (dotIndex != std::string::npos) ext = texFileOrPath.filename().string().substr(dotIndex + 1);
		if (ext == "mp4" || ext == "wmv" || ext == "avi" || ext == "mov") {
			if (!mVideo.isStopped()) {
				mVideo.stop();
			}

			mIsVideoLoaded = mVideo.loadMovie(texFileOrPath);

			mVideoDuration = mVideo.getDuration();
			mVideoPos = mVideo.getPosition();
			mVideo.play();

		}
	}*/
}

void RewriteApp::toggleCursorVisibility(bool visible)
{
	if (visible)
	{
		showCursor();
	}
	else
	{
		hideCursor();
	}
}

void RewriteApp::fileDrop(FileDropEvent event)
{
	mVDSession->fileDrop(event);
}

void RewriteApp::mouseMove(MouseEvent event)
{
	if (!mVDSession->handleMouseMove(event)) {

	}
}

void RewriteApp::mouseDown(MouseEvent event)
{

	if (!mVDSession->handleMouseDown(event)) {

	}
}

void RewriteApp::mouseDrag(MouseEvent event)
{

	if (!mVDSession->handleMouseDrag(event)) {

	}
}

void RewriteApp::mouseUp(MouseEvent event)
{

	if (!mVDSession->handleMouseUp(event)) {

	}
}

void RewriteApp::keyDown(KeyEvent event)
{

	// warp editor did not handle the key, so handle it here
	if (!mVDSession->handleKeyDown(event)) {
		switch (event.getCode()) {
		case KeyEvent::KEY_F12:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen(!isFullScreen());
			break;

		case KeyEvent::KEY_l:
			mVDSession->createWarp();
			break;
		}
	}
}

void RewriteApp::keyUp(KeyEvent event)
{

	// let your application perform its keyUp handling here
	if (!mVDSession->handleKeyUp(event)) {
		/*switch (event.getCode()) {
		default:
			CI_LOG_V("main keyup: " + toString(event.getCode()));
			break;
		}*/
	}
}
void RewriteApp::cleanup()
{
	CI_LOG_V("cleanup and save");
	ui::Shutdown();
	mVDSession->save();

	mVDSettings->save();
	CI_LOG_V("quit");
}

void RewriteApp::update()
{
	switch (mVDSession->getCmd()) {
	case 0:
		//createControlWindow();
		break;
	case 1:
		//deleteControlWindows();
		break;
	}
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IFPS, getAverageFps());
	mVDSession->update();
	/*mVideo.update();
	mVideoPos = mVideo.getPosition();
	if (mVideo.isStopped() || mVideo.isPaused()) {
		mVideo.setPosition(0.0);
		mVideo.play();
	}*/
}


void RewriteApp::resize()
{
	mVDUI->resize();


}
void RewriteApp::draw()
{
	// clear the window and set the drawing color to white
	gl::clear();
	gl::color(Color::white());
	if (mFadeInDelay) {
		mVDSettings->iAlpha = 0.0f;
		if (getElapsedFrames() > 10.0) {// mVDSession->getFadeInDelay()) {
			mFadeInDelay = false;
			timeline().apply(&mVDSettings->iAlpha, 0.0f, 1.0f, 1.5f, EaseInCubic());
		}
	}
	else {
		//gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight, false);
		gl::setMatricesWindow(mVDSession->getIntUniformValueByIndex(mVDSettings->IOUTW), mVDSession->getIntUniformValueByIndex(mVDSettings->IOUTH), true);

		// ok gl::draw(mVDSession->getPostFboTexture(), Area(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));//getWindowBounds()
		gl::draw(mVDSession->getWarpFboTexture(), Area(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));//getWindowBounds()
		/*vec2 videoSize = vec2(mVideo.getWidth(), mVideo.getHeight());
		mGlslVideoTexture->uniform("uVideoSize", videoSize);
		videoSize *= 0.25f;
		videoSize *= 0.5f;
		ciWMFVideoPlayer::ScopedVideoTextureBind scopedVideoTex(mVideo, 0);
		gl::scale(vec3(videoSize, 1.0f));*/

		//gl::draw(mPostFbo->getColorTexture());
		//gl::draw(mVDSession->getFboRenderedTexture(0));
	}
	// Spout Send
	// KO mSpoutOut.sendViewport();
	// OK mSpoutOut.sendTexture(mVDSession->getFboRenderedTexture(1));
	mSpoutOut.sendTexture(mVDSession->getPostFboTexture());
	// imgui
	if (mVDSession->showUI()) {
		mVDUI->Run("UI", (int)getAverageFps());
		if (mVDUI->isReady()) {
		}
	}
	getWindow()->setTitle(mVDSettings->sFps + " fps");
}



CINDER_APP(RewriteApp, RendererGl(RendererGl::Options().msaa(8)), &RewriteApp::prepare)
