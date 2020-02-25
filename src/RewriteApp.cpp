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


	bool							mFadeInDelay = true;
	void							saveWarps();
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
	mVDSession->setBpm(160.0f);
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IMOUSEX, 0.27710f);
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IMOUSEY, 0.5648f);
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IEXPOSURE, 1.93f);
	mFadeInDelay = true;
	// UI
	mVDUI = VDUI::create(mVDSettings, mVDSession);
}
/*void RewriteApp::createWarp() {
	auto warp = WarpBilinear::create();
	warp->setAFboIndex(0);
	warp->setBFboIndex(0);
	warp->setAShaderIndex(0);
	warp->setBShaderIndex(0);
	warp->setAShaderFilename("inputImage.fs");
	warp->setBShaderFilename("inputImage.fs");
	warp->setATextureFilename("audio");
	warp->setBTextureFilename("audio");
	mWarpList.push_back(WarpBilinear::create());
}*/

void RewriteApp::saveWarps()
{

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
		// let your application perform its mouseMove handling here
	}
}

void RewriteApp::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!mVDSession->handleMouseDown(event)) {
		// let your application perform its mouseDown handling here
	}
}

void RewriteApp::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!mVDSession->handleMouseDrag(event)) {
		// let your application perform its mouseDrag handling here
	}
}

void RewriteApp::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!mVDSession->handleMouseUp(event)) {
		// let your application perform its mouseUp handling here
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

				//case KeyEvent::KEY_v:
			//	// toggle vertical sync
			//	gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
			//	break;

			/*case KeyEvent::KEY_a:
				// toggle drawing a random region of the image
				if( mSrcArea.getWidth() != mImage->getWidth() || mSrcArea.getHeight() != mImage->getHeight() )
					mSrcArea = mImage->getBounds();
				else {
					int x1 = Rand::randInt( 0, mImage->getWidth() - 150 );
					int y1 = Rand::randInt( 0, mImage->getHeight() - 150 );
					int x2 = Rand::randInt( x1 + 150, mImage->getWidth() );
					int y2 = Rand::randInt( y1 + 150, mImage->getHeight() );
					mSrcArea = Area( x1, y1, x2, y2 );
				}
				break;
			case KeyEvent::KEY_SPACE:
				// toggle drawing mode
				mUseBeginEnd = !mUseBeginEnd;

				break;*/
			}
		}
	
}

void RewriteApp::keyUp(KeyEvent event)
{
	
		// let your application perform its keyUp handling here
		if (!mVDSession->handleKeyUp(event)) {
			switch (event.getCode()) {
			default:
				CI_LOG_V("main keyup: " + toString(event.getCode()));
				break;
			}
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
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IBARBEAT, getElapsedSeconds()); // TODO 20200225 remove
	mVDSession->update();
	
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
		gl::setMatricesWindow(mVDSession->getIntUniformValueByIndex(mVDSettings->IOUTW), mVDSession->getIntUniformValueByIndex(mVDSettings->IOUTH), false);

		gl::draw(mVDSession->getPostFboTexture(), Area(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));//getWindowBounds()

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
