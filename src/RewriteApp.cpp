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
#include "Warp.h"
#include "VDSession.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace videodromm;
using namespace std;

class RewriteApp : public App {
public:
	static void prepare(Settings *settings);

	void setup() override;
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

private:
	// Settings
	VDSettingsRef					mVDSettings;
	// Session
	VDSessionRef					mVDSession;
	fs::path						mSettings;
	WarpList						mWarpList;
	//map<int, VDFboRef>				fbos;
	bool							mFadeInDelay = true;
	void							loadWarps();
	void							saveWarps();
};

void RewriteApp::prepare(Settings *settings)
{
	settings->setWindowSize(1440, 900);
}

void RewriteApp::setup()
{
	disableFrameRate();
	// Settings
	mVDSettings = VDSettings::create("Rewrite");
	// Session
	mVDSession = VDSession::create(mVDSettings);
	// initialize warps
	mSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarpList = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarpList.push_back(WarpBilinear::create());
		mWarpList.push_back(WarpPerspective::create());
		mWarpList.push_back(WarpPerspectiveBilinear::create());
	}
	
	loadWarps();
	// adjust the content size of the warps
	Warp::setSize(mWarpList, mVDSession->getFboRenderedTexture(0)->getSize());
}
void RewriteApp::loadWarps() {
	int i = 0;
	for (auto &warp : mWarpList) {
		i = math<int>::min(i, mWarpList.size() - 1);
		string jsonFileName = "warp" + toString(i) + ".json";
		fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
		if (fs::exists(jsonFile)) {
			JsonTree json(loadFile(jsonFile));
			warp->fromJson(json);
			if (json[0].hasChild("warp")) {
				JsonTree warp(json[0].getChild("warp"));
				string shaderFileName = (warp.hasChild("ashaderfilename")) ? warp.getValueForKey<string>("ashaderfilename") : "inputImage.fs";
				mVDSession->createShaderFbo(shaderFileName, i);
			}
			i++;
		}
	}
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
		// iterate over the fbos and draw their content
		int i = 0;
		for (auto &warp : mWarpList) {
			i = math<int>::min(i, mVDSession->getFboListSize() - 1);
			if (mVDSession->isFboValid(i)) {
				warp->draw(mVDSession->getFboRenderedTexture(i), mVDSession->getFboSrcArea(i));
			}
			i++;
		}

	}
	getWindow()->setTitle(mVDSettings->sFps + " fps");
}
void RewriteApp::cleanup()
{

	// save warp settings
	Warp::writeSettings(mWarpList, writeFile(mSettings));
}
void RewriteApp::saveWarps()
{
	int i = 0;
	for (auto &warp : mWarpList) {
		JsonTree		json;
		string jsonFileName = "warpfbo" + toString(i) + ".json";
		fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
		// write file
		json.pushBack(warp->toJson());
		json.write(jsonFile);
		i++;
	}
}
void RewriteApp::update()
{

}
void RewriteApp::resize()
{
	// tell the fbos our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarpList);
}

void RewriteApp::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarpList, event)) {
		// let your application perform its mouseMove handling here
	}
}

void RewriteApp::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarpList, event)) {
		// let your application perform its mouseDown handling here
	}
}

void RewriteApp::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarpList, event)) {
		// let your application perform its mouseDrag handling here
	}
}

void RewriteApp::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarpList, event)) {
		// let your application perform its mouseUp handling here
	}
}

void RewriteApp::keyDown(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarpList, event)) {
		// warp editor did not handle the key, so handle it here
		switch (event.getCode()) {
		case KeyEvent::KEY_F12:
			// quit the application
			quit();
			break;
		case KeyEvent::KEY_f:
			// toggle full screen
			setFullScreen(!isFullScreen());
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode(!Warp::isEditModeEnabled());
			break;
		case KeyEvent::KEY_v:
			mVDSession->fboFlipV(0);// TODO other indexes	fbos[0]->flipV();
		//mImage = gl::Texture::create(loadImage(loadAsset("help.png")),
		//	gl::Texture2d::Format().loadTopDown(mFlipV).mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));

			break;
		case KeyEvent::KEY_h:
			mVDSession->fboFlipH(0);
			break;
		}
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

void RewriteApp::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarpList, event)) {
		// let your application perform its keyUp handling here
	}
}



CINDER_APP(RewriteApp, RendererGl(RendererGl::Options().msaa(8)), &RewriteApp::prepare)
