//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace videodromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	CI_LOG_V("VDSession ctor");
	mVDSettings = aVDSettings;

	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	// TODO: needed? mVDAnimation->tapTempo();
	// Mix
	mVDMix = VDMix::create(mVDSettings, mVDAnimation);
	//createFboShaderTexture("default.fs", "0.jpg");
	//createFboShaderTexture("audio.fs", "audio");
	// allow log to file
	mVDLog = VDLog::create();
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing

	fboFmt.setColorTextureFormat(fmt);
	mWarpsFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, format.depthTexture());
	mPostFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, format.depthTexture());
	mGlslPost = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("passthrough.vs")).fragment(loadAsset("post.glsl")));
	mWarpTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
	// adjust the content size of the warps

	// TODO 20200305 was 20200302 if (getFboRenderedTexture(0)) Warp::setSize(mWarpList, getFboRenderedTexture(0)->getSize());
	Warp::setSize(mWarpList, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight)); //
	// initialize warps
	mSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarpList = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarpList.push_back(WarpPerspectiveBilinear::create());
	}
	loadFbos();
	//loadWarps();
	// init fbo format
	//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	//fmt.setBorderColor(Color::black());
	// uncomment this to enable 4x antialiasing
	//fboFmt.setSamples( 4 );
	/*fboFmt.setColorTextureFormat(fmt);
	mPosX = mPosY = 0.0f;
	mZoom = 1.0f;
	mSelectedWarp = 0;*/
	// Modes
	mModesList[0] = "Mixette";
	mModesList[1] = "Fbo0";
	mModesList[2] = "Fbo1";
	mModesList[3] = "Fbo2";
	mModesList[4] = "Fbo3";
	mModesList[5] = "Fbo4";
	mModesList[6] = "Fbo5";
	mModesList[7] = "Fbo6";
	mModesList[8] = "Fbo7";
	mModesList[9] = "Fbo8";
	mMode = 0;
	// Websocket
	mVDWebsocket = VDWebsocket::create(mVDSettings, mVDAnimation);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation, mVDWebsocket);
	// reset no matter what, so we don't miss anything
	cmd = -1;
	mFreqWSSend = false;
	reset();

	mCurrentBlend = 0;
	// check to see if VDSession.xml file exists and restore if it does
	sessionPath = getAssetPath("") / mVDSettings->mAssetsPath / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		// Create json file if it doesn't already exist.
		std::ofstream oStream(sessionPath.string());
		oStream.close();
		save();
	}
}
void VDSession::loadFbos() {

	int f = 0;
	bool found = true;
	string shaderFileName;
	string textureFileName;
	while (found) {
		string jsonFileName = "fbo" + toString(f) + ".json";
		fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
		if (fs::exists(jsonFile)) {
			JsonTree json(loadFile(jsonFile));
			fboFromJson(json);
			f++;
		}
		else {
			found = false;
		}
	} //while

}

VDSessionRef VDSession::create(VDSettingsRef aVDSettings)
{
	return shared_ptr<VDSession>(new VDSession(aVDSettings));
}

void VDSession::update(unsigned int aClassIndex) {
	if (mVDWebsocket->hasReceivedShader()) {
		string receivedShader = mVDWebsocket->getReceivedShader();
		// save file
		string mShaderFileName = mVDSettings->mWebSocketsNickname + toString((int)getElapsedSeconds()) + ".fs";

		fs::path fsFile = getAssetPath("") / mVDSettings->mAssetsPath / mShaderFileName;
		ofstream mFS(fsFile.string(), std::ofstream::binary);
		mFS << receivedShader;
		mFS.close();

		// save fbo file
		JsonTree		json;
		JsonTree shader = ci::JsonTree::makeArray("shader");
		shader.addChild(ci::JsonTree("shadername", "received.txt"));
		shader.pushBack(ci::JsonTree("shadertype", "fs"));
		shader.pushBack(ci::JsonTree("shadertext", receivedShader));
		json.addChild(shader);
		JsonTree texture = ci::JsonTree::makeArray("texture");
		texture.addChild(ci::JsonTree("texturename", "audio"));
		texture.pushBack(ci::JsonTree("texturetype", "audio"));
		texture.pushBack(ci::JsonTree("texturemode", 0));
		json.addChild(texture);

		// load fbo
		if (odd) {
			fboFromJson(json, getFboListSize() + 1);
		}
		else {
			fboFromJson(json, getFboListSize());
		}
		odd = !odd;
	}

	/*if (mVDRouter->hasFBOAChanged()) {
		setFboFragmentShaderIndex(0, mVDRouter->selectedFboA());
	}
	if (mVDRouter->hasFBOBChanged()) {
		setFboFragmentShaderIndex(1, mVDRouter->selectedFboB());
	}
	if (aClassIndex == 0) {
		if (mVDWebsocket->hasReceivedStream()) { //&& (getElapsedFrames() % 100 == 0)) {
			updateStream(mVDWebsocket->getBase64Image());
		}
		if (mVDWebsocket->hasReceivedShader()) {
			string receivedShader = mVDWebsocket->getReceivedShader();
			if (mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE) < 0.5) {
				setFragmentShaderString(2, receivedShader);
				mShaderLeft = receivedShader;
			}
			else {
				setFragmentShaderString(1, receivedShader);
				mShaderRight = receivedShader;
			}
			setFragmentShaderString(0, receivedShader);
			setHydraFragmentShaderString(receivedShader);
			// TODO timeline().apply(&mWarps[aWarpIndex]->ABCrossfade, 0.0f, 2.0f); };
		}
		if (mVDWebsocket->hasReceivedUniforms()) {
			mHydraUniformsValuesString = mVDWebsocket->getReceivedUniforms();
		}
		// TODO: CHECK index if (mVDSettings->iGreyScale)
		//{
		//	mVDWebsocket->changeFloatValue(1, mVDAnimation->getFloatUniformValueByIndex(3));
		//	mVDWebsocket->changeFloatValue(2, mVDAnimation->getFloatUniformValueByIndex(3));
		//	mVDWebsocket->changeFloatValue(5, mVDAnimation->getFloatUniformValueByIndex(7));
		//	mVDWebsocket->changeFloatValue(6, mVDAnimation->getFloatUniformValueByIndex(7));
		//}
		*/
		// fps calculated in main app

	mVDSettings->sFps = toString(floor(getFloatUniformValueByIndex(mVDSettings->IFPS)));
	mVDAnimation->update();
	/*}
	else {
		// aClassIndex == 1 (audio analysis only)
		updateAudio();
	}*/
	// all cases
	mVDWebsocket->update();
	/*if (mFreqWSSend) {
		mVDWebsocket->changeFloatValue(mVDSettings->IFREQ0, getFreq(0), true);
		mVDWebsocket->changeFloatValue(mVDSettings->IFREQ1, getFreq(1), true);
		mVDWebsocket->changeFloatValue(mVDSettings->IFREQ2, getFreq(2), true);
		mVDWebsocket->changeFloatValue(mVDSettings->IFREQ3, getFreq(3), true);
	}*/
	// check if xFade changed
	/*if (mVDSettings->xFadeChanged) {
		mVDSettings->xFadeChanged = false;
	}*/
	/*updateMixUniforms();
	renderMix();
	updateHydraUniforms();
	renderHydra();
	// blendmodes preview
	if (mVDAnimation->renderBlend()) {
		updateBlendUniforms();
		renderBlend();
	}*/
	mVDMix->getMixetteTexture(0);
	renderWarpsToFbo();
	renderPostToFbo();
}
void VDSession::renderPostToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mPostFbo);
		// clear out the FBO with black
		gl::clear(Color::black());
		//gl::clear(ColorA(0.4f, 0.8f, 0.0f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mPostFbo->getSize());

		// texture binding must be before ScopedGlslProg
		//mWarpsFbo->getColorTexture()
		mWarpTexture->bind(40);
		gl::ScopedGlslProg prog(mGlslPost);

		// not used yet mGlslPost->uniform("TIME", getFloatUniformValueByIndex(mVDSettings->ITIME) - mVDSettings->iStart);;
		mGlslPost->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		mGlslPost->uniform("iChannel0", 40); // texture 0
		mGlslPost->uniform("iSobel", getFloatUniformValueByIndex(mVDSettings->ISOBEL));
		mGlslPost->uniform("iExposure", getFloatUniformValueByIndex(mVDSettings->IEXPOSURE));
		mGlslPost->uniform("iTrixels", getFloatUniformValueByIndex(mVDSettings->ITRIXELS)); // trixels if > 0.
		mGlslPost->uniform("iZoom", getFloatUniformValueByIndex(mVDSettings->IZOOM));
		mGlslPost->uniform("iChromatic", getFloatUniformValueByIndex(mVDSettings->ICHROMATIC));
		mGlslPost->uniform("iFlipV", (int)getBoolUniformValueByIndex(mVDSettings->IFLIPPOSTV));
		mGlslPost->uniform("iFlipH", (int)getBoolUniformValueByIndex(mVDSettings->IFLIPPOSTH));
		mGlslPost->uniform("iInvert", (int)getBoolUniformValueByIndex(mVDSettings->IINVERT));
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	}
}
void VDSession::renderWarpsToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mWarpsFbo);
		// clear out the FBO with black
		//gl::clear(Color::black());
		gl::clear(ColorA(0.4f, 0.0f, 0.8f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mWarpsFbo->getSize());
		// iterate over the warps and draw their content
		int i = 0;
		int a = 0;
		int s = 0;
		for (auto &warp : mWarpList) {
			a = warp->getAFboIndex();
			if (a < 0) a = 0; // TODO 20200228 a could be negative if warps3.xml > warps01.json
			i = math<int>::min(a, getFboListSize() - 1);
			s = getFboListSize(); // TMP
			//if (isFboValid(i)) {
				//ko  warp->draw(getFboRenderedTexture(0));
				//ko warp->draw(getFboTexture(0)); bind to 0 broken
			warp->draw(mVDMix->getRenderedMixetteTexture(0));
			//}

		}
		//gl::color(0.5, 0.0, 1.0, 0.4f);
		//gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth/2, mVDSettings->mFboHeight/2));
		mWarpTexture = mWarpsFbo->getColorTexture();
	}
}
bool VDSession::save()
{
	saveFbos();
	saveWarps();
	// save uniforms settings
	//mVDAnimation->save();
	// save in sessionPath
	// TODO add shaders section
	JsonTree doc;

	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("bpm", mOriginalBpm));
	settings.addChild(ci::JsonTree("beatsperbar", mVDAnimation->getIntUniformValueByName("iBeatsPerBar")));
	//settings.addChild(ci::JsonTree("fadeindelay", mFadeInDelay));
	//settings.addChild(ci::JsonTree("fadeoutdelay", mFadeOutDelay));
	settings.addChild(ci::JsonTree("endframe", mVDAnimation->mEndFrame));
	doc.pushBack(settings);

	/*JsonTree assets = JsonTree::makeArray("assets");
	if (mWaveFileName.length() > 0) assets.addChild(ci::JsonTree("wavefile", mWaveFileName));
	assets.addChild(ci::JsonTree("waveplaybackdelay", mWavePlaybackDelay));
	if (mMovieFileName.length() > 0) assets.addChild(ci::JsonTree("moviefile", mMovieFileName));
	assets.addChild(ci::JsonTree("movieplaybackdelay", mMoviePlaybackDelay));
	if (mImageSequencePath.length() > 0) assets.addChild(ci::JsonTree("imagesequencepath", mImageSequencePath));
	if (mText.length() > 0) {
		assets.addChild(ci::JsonTree("text", mText));
		assets.addChild(ci::JsonTree("textplaybackdelay", mTextPlaybackDelay));
		assets.addChild(ci::JsonTree("textplaybackend", mTextPlaybackEnd));
	}
	doc.pushBack(assets);*/

	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());

	return true;
}

void VDSession::restore()
{
	// save load settings
	//load();

	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));
		/*if (doc.hasChild("shaders")) {
			JsonTree shaders(doc.getChild("shaders"));
			if (shaders.hasChild("0")) createShaderFbo(shaders.getValueForKey<string>("0"));
			if (shaders.hasChild("1")) createShaderFbo(shaders.getValueForKey<string>("1"));
			if (shaders.hasChild("2")) createShaderFbo(shaders.getValueForKey<string>("2"));
			if (shaders.hasChild("3")) createShaderFbo(shaders.getValueForKey<string>("3"));
		}*/
		if (doc.hasChild("settings")) {
			JsonTree settings(doc.getChild("settings"));
			if (settings.hasChild("bpm")) {
				mOriginalBpm = settings.getValueForKey<float>("bpm", 166.0f);
				CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mOriginalBpm " + toString(mOriginalBpm));
				mVDAnimation->setBpm(mOriginalBpm);
				CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()));
			};
			if (settings.hasChild("beatsperbar")) mVDAnimation->setIntUniformValueByName("iBeatsPerBar", settings.getValueForKey<int>("beatsperbar"));
			if (mVDAnimation->getIntUniformValueByName("iBeatsPerBar") < 1) mVDAnimation->setIntUniformValueByName("iBeatsPerBar", 4);
			//if (settings.hasChild("fadeindelay")) mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
			//if (settings.hasChild("fadeoutdelay")) mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
			if (settings.hasChild("endframe")) mVDAnimation->mEndFrame = settings.getValueForKey<int>("endframe");
			CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mTargetFps " + toString(mTargetFps));
			mTargetFps = mVDAnimation->getBpm() / 60.0f * mFpb;
			CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mTargetFps " + toString(mTargetFps));
		}

		/*if (doc.hasChild("assets")) {
			JsonTree assets(doc.getChild("assets"));
			if (assets.hasChild("wavefile")) mWaveFileName = assets.getValueForKey<string>("wavefile");
			if (assets.hasChild("waveplaybackdelay")) mWavePlaybackDelay = assets.getValueForKey<int>("waveplaybackdelay");
			if (assets.hasChild("moviefile")) mMovieFileName = assets.getValueForKey<string>("moviefile");
			if (assets.hasChild("movieplaybackdelay")) mMoviePlaybackDelay = assets.getValueForKey<int>("movieplaybackdelay");
			if (assets.hasChild("imagesequencepath")) mImageSequencePath = assets.getValueForKey<string>("imagesequencepath");
			if (assets.hasChild("text")) mText = assets.getValueForKey<string>("text");
			if (assets.hasChild("textplaybackdelay")) mTextPlaybackDelay = assets.getValueForKey<int>("textplaybackdelay");
			if (assets.hasChild("textplaybackend")) mTextPlaybackEnd = assets.getValueForKey<int>("textplaybackend");
		}*/

	}
	catch (const JsonTree::ExcJsonParserError& exc) {
		CI_LOG_W(exc.what());
	}
}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
	mFpb = 16;
	mVDAnimation->setBpm(mOriginalBpm);
	mTargetFps = mOriginalBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mOriginalBpm = 166;
	/* TODO 20200221
	mWaveFileName = "";
	mWavePlaybackDelay = 10;
	mMovieFileName = "";
	mImageSequencePath = "";
	mMoviePlaybackDelay = 10;
	mFadeInDelay = 5;
	mFadeOutDelay = 1;
	mText = "";
	mTextPlaybackDelay = 10;
	mTextPlaybackEnd = 2020000;*/
	mVDAnimation->mEndFrame = 20000000;

	resetSomeParams();
}

void VDSession::blendRenderEnable(bool render) {
	mVDAnimation->blendRenderEnable(render);
}

void VDSession::fileDrop(FileDropEvent event) {
	string ext = "";
	//string fileName = "";

	unsigned int index = (int)(event.getX() / (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin));
	int y = (int)(event.getY());
	//if (index < 2 || y < mVDSettings->uiYPosRow3 || y > mVDSettings->uiYPosRow3 + mVDSettings->uiPreviewH) index = 0;
	ci::fs::path mPath = event.getFile(event.getNumFiles() - 1);
	string absolutePath = mPath.string();
	// use the last of the dropped files
	int dotIndex = absolutePath.find_last_of(".");
	int slashIndex = absolutePath.find_last_of("\\");

	if (dotIndex != std::string::npos && dotIndex > slashIndex) {
		ext = absolutePath.substr(dotIndex + 1);
		//fileName = absolutePath.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		if (ext == "json") {
			JsonTree json(loadFile(absolutePath));
			fboFromJson(json);

		}
		else if (ext == "glsl" || ext == "frag" || ext == "fs") {
			loadFragmentShader(absolutePath, index);
		}
		else if (ext == "png" || ext == "jpg") {
			if (index < 1) index = 1;
			if (index > 3) index = 3;
			loadImageFile(absolutePath, index);
		}
		/*else if (ext == "wav" || ext == "mp3") {
			loadAudioFile(absolutePath);
		}
		else if (ext == "xml") {
		}
		else if (ext == "mov") {
			loadMovie(absolutePath, index);
		}
		else if (ext == "txt") {
		}
		else if (ext == "") {
			// try loading image sequence from dir
			if (!loadImageSequence(absolutePath, index)) {
				// try to load a folder of shaders
				loadShaderFolder(absolutePath);
			}
		}*/
	}
}

#pragma region events
bool VDSession::handleMouseMove(MouseEvent &event)
{
	bool handled = true;
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDown(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDrag(MouseEvent &event)
{

	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseUp(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}

	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}


bool VDSession::handleKeyDown(KeyEvent &event)
{
	bool handled = true;
	float newValue;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif
	bool isShiftDown = event.isShiftDown();
	bool isAltDown = event.isAltDown();
	CI_LOG_V("session keydown: " + toString(event.getCode()) + " ctrl: " + toString(isModDown) + " shift: " + toString(isShiftDown) + " alt: " + toString(isAltDown));
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarpList, event)) {
		// pass this event to Mix handler
		if (!mVDAnimation->handleKeyDown(event)) {
			switch (event.getCode()) {
			case KeyEvent::KEY_w:
				CI_LOG_V("wsConnect");
				if (isModDown) {
					wsConnect();
				}
				else {
					// handled in main app
					//handled = false;
					// toggle warp edit mode
					Warp::enableEditMode(!Warp::isEditModeEnabled());
				}
				break;

			case KeyEvent::KEY_F1:
				mMode = 0;
				break;
			case KeyEvent::KEY_F2:
				mMode = 1;
				break;
			case KeyEvent::KEY_F3:
				mMode = 2;
				break;
			case KeyEvent::KEY_F4:
				mMode = 3;
				break;
			case KeyEvent::KEY_F5:
				mMode = 4;
				break;
			case KeyEvent::KEY_F6:
				mMode = 5;
				break;
			case KeyEvent::KEY_F7:
				mMode = 6;
				break;
			case KeyEvent::KEY_F8:
				mMode = 7;
				break;
			case KeyEvent::KEY_F9:
				mMode = 8;
				break;
				//case KeyEvent::KEY_SPACE:
				//mVDTextures->playMovie();
				//mVDAnimation->currentScene++;
				//if (mMovie) { if (mMovie->isPlaying()) mMovie->stop(); else mMovie->play(); }
				//break;
			//case KeyEvent::KEY_0:
				//break;
			//case KeyEvent::KEY_l:
				// live params TODO mVDAnimation->load();
				//mLoopVideo = !mLoopVideo;
				//if (mMovie) mMovie->setLoop(mLoopVideo);
				//break;
			case KeyEvent::KEY_x:
				// trixels
				mVDWebsocket->changeFloatValue(mVDSettings->ITRIXELS, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ITRIXELS) + 0.05f);
				break;
			case KeyEvent::KEY_r:
				if (isAltDown) {
					mVDWebsocket->changeFloatValue(mVDSettings->IBR, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IBR), false, true, isShiftDown, isModDown);
				}
				else {
					mVDWebsocket->changeFloatValue(mVDSettings->IFR, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFR), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_g:
				if (isAltDown) {
					mVDWebsocket->changeFloatValue(mVDSettings->IBG, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IBG), false, true, isShiftDown, isModDown);
				}
				else {
					mVDWebsocket->changeFloatValue(mVDSettings->IFG, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFG), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_b:
				if (isAltDown) {
					mVDWebsocket->changeFloatValue(mVDSettings->IBB, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IBB), false, true, isShiftDown, isModDown);
				}
				else {
					mVDWebsocket->changeFloatValue(mVDSettings->IFB, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFB), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_a:
				mVDWebsocket->changeFloatValue(mVDSettings->IFA, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFA), false, true, isShiftDown, isModDown);
				break;
			case KeyEvent::KEY_u:
				// chromatic
				// TODO find why can't put value >0.9 or 0.85!
				newValue = mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ICHROMATIC) + 0.05f;
				if (newValue > 1.0f) newValue = 0.0f;
				mVDWebsocket->changeFloatValue(mVDSettings->ICHROMATIC, newValue);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				mVDWebsocket->changeFloatValue(mVDSettings->IPIXELATE, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IPIXELATE) + 0.05f);
				break;
			case KeyEvent::KEY_y:
				// glitch
				mVDWebsocket->changeBoolValue(mVDSettings->IGLITCH, true);
				break;
			case KeyEvent::KEY_i:
				// invert
				mVDWebsocket->changeBoolValue(mVDSettings->IINVERT, true);
				break;
			case KeyEvent::KEY_o:
				// toggle
				mVDWebsocket->changeBoolValue(mVDSettings->ITOGGLE, true);
				break;
			case KeyEvent::KEY_z:
				// zoom
				mVDWebsocket->changeFloatValue(mVDSettings->IZOOM, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IZOOM) - 0.05f);
				break;
				/* removed temp for Sky Project case KeyEvent::KEY_LEFT:
					//mVDTextures->rewindMovie();
					if (mVDAnimation->getFloatUniformValueByIndex(21) > 0.1f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) - 0.1f);
					break;
				case KeyEvent::KEY_RIGHT:
					//mVDTextures->fastforwardMovie();
					if (mVDAnimation->getFloatUniformValueByIndex(21) < 1.0f) mVDWebsocket->changeFloatValue(21, mVDAnimation->getFloatUniformValueByIndex(21) + 0.1f);
					break;*/
			case KeyEvent::KEY_PAGEDOWN:
			case KeyEvent::KEY_RIGHT:
				// crossfade right
				if (mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE) < 1.0f) mVDWebsocket->changeFloatValue(mVDSettings->IXFADE, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE) + 0.1f);
				break;
			case KeyEvent::KEY_PAGEUP:
			case KeyEvent::KEY_LEFT:
				// crossfade left
				if (mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE) > 0.0f) mVDWebsocket->changeFloatValue(mVDSettings->IXFADE, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE) - 0.1f);
				break;
			case KeyEvent::KEY_UP:
				// imgseq next
				//incrementSequencePosition();
				break;
			case KeyEvent::KEY_DOWN:
				// imgseq next
				//decrementSequencePosition();
				break;
			case KeyEvent::KEY_v:
				//if (isModDown) fboFlipV(0);// TODO other indexes mVDSettings->mFlipV = !mVDSettings->mFlipV; useless?
				break;
			case KeyEvent::KEY_h:
				if (isModDown) {
					//fboFlipH(0);// TODO other indexes mVDSettings->mFlipH = !mVDSettings->mFlipH; useless?
				}
				else {
					// ui visibility
					toggleUI();
				}
				break;
			case KeyEvent::KEY_d:
				/*if (isAltDown) {
					setSpeed(0, getSpeed(0) - 0.01f);
				}
				else {
					setSpeed(0, getSpeed(0) + 0.01f);
				}*/
				break;
			default:
				CI_LOG_V("session keydown: " + toString(event.getCode()));
				handled = false;
				break;
			}
		}
	}
	CI_LOG_V((handled ? "session keydown handled " : "session keydown not handled "));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyUp(KeyEvent &event) {
	bool handled = true;

	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarpList, event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
			switch (event.getCode()) {
			case KeyEvent::KEY_y:
				// glitch
				mVDWebsocket->changeBoolValue(mVDSettings->IGLITCH, false);
				break;
			case KeyEvent::KEY_t:
				// trixels
				mVDWebsocket->changeFloatValue(mVDSettings->ITRIXELS, 0.0f);
				break;
			case KeyEvent::KEY_i:
				// invert
				mVDWebsocket->changeBoolValue(mVDSettings->IINVERT, false);
				break;
			case KeyEvent::KEY_u:
				// chromatic
				mVDWebsocket->changeFloatValue(mVDSettings->ICHROMATIC, 0.0f);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				mVDWebsocket->changeFloatValue(mVDSettings->IPIXELATE, 1.0f);
				break;
			case KeyEvent::KEY_o:
				// toggle
				mVDWebsocket->changeBoolValue(mVDSettings->ITOGGLE, false);
				break;
			case KeyEvent::KEY_z:
				// zoom
				mVDWebsocket->changeFloatValue(mVDSettings->IZOOM, 1.0f);
				break;
			default:
				CI_LOG_V("session keyup: " + toString(event.getCode()));
				handled = false;
				break;
			}
		}
	}
	CI_LOG_V((handled ? "session keyup handled " : "session keyup not handled "));
	event.setHandled(handled);
	return event.isHandled();
}
#pragma endregion events
// fbos
#pragma region fbos

/*unsigned int VDSession::createShaderFboFromString(string aFragmentShaderString, string aShaderFilename) {
	unsigned int rtn = 0;
	// create new shader
	VDShaderRef s(new VDShader(mVDSettings, mVDAnimation, aShaderFilename, aFragmentShaderString));
	if (s->isValid()) {
		mShaderList.push_back(s);
		rtn = mShaderList.size() - 1;
		// each shader element has a fbo
		VDFboRef f(new VDFbo(mVDSettings, mVDAnimation));
		// create fbo xml
		XmlTree			fboXml;
		fboXml.setTag(aShaderFilename);
		fboXml.setAttribute("id", rtn);
		fboXml.setAttribute("width", "1280");
		fboXml.setAttribute("height", "720");
		fboXml.setAttribute("shadername", mShaderList[rtn]->getName());
		fboXml.setAttribute("inputtextureindex", math<int>::min(rtn, mTextureList.size() - 1));
		f->fromXml(fboXml);
		//f->setShaderIndex(rtn);
		f->setFragmentShader(rtn, mShaderList[rtn]->getFragmentString(), mShaderList[rtn]->getName());
		mFboList.push_back(f);
		setFboInputTexture(mFboList.size() - 1, math<int>::min(rtn, mTextureList.size() - 1));// TODO load tex idx from file 20200216
	}
	return rtn;
}




void VDSession::sendFragmentShader(unsigned int aShaderIndex) {
	mVDWebsocket->changeFragmentShader(getFragmentString(aShaderIndex));
}

void VDSession::setFboAIndex(unsigned int aIndex, unsigned int aFboIndex) {
	if (aFboIndex < mFboList.size()) {
		mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, aFboIndex);
	}
	else {
		mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, mFboList.size() - 1);
	}*/
	/*mVDMix->setWarpAFboIndex(aIndex, aFboIndex);
	mVDRouter->setWarpAFboIndex(aIndex, aFboIndex);
	mVDWebsocket->changeWarpFboIndex(aIndex, aFboIndex, 0);
}*/
/*
void VDSession::setFboBIndex(unsigned int aIndex, unsigned int aFboIndex) {
	if (aFboIndex < mFboList.size()) {
		mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, aFboIndex);
	}
	else {
		mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, mFboList.size() - 1);
	}*/
	/*mVDMix->setWarpBFboIndex(aIndex, aFboIndex);
	mVDRouter->setWarpBFboIndex(aIndex, aFboIndex);
	mVDWebsocket->changeWarpFboIndex(aIndex, aFboIndex, 1);
}*/
#pragma endregion fbos
// shaders
#pragma region shaders
/*bool VDSession::loadShaderFolder(string aFolder) {
	string ext = "";
	fs::path p(aFolder);
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			int dotIndex = fileName.find_last_of(".");

			if (dotIndex != std::string::npos)
			{
				ext = fileName.substr(dotIndex + 1);
				if (ext == "glsl")
				{
					loadFragmentShader(aFolder + "/" + fileName);
				}
			}
		}
	}
	return true;
}
*/
#pragma endregion shaders

// websockets
#pragma region websockets

void VDSession::wsConnect() {
	mVDWebsocket->wsConnect();
}
void VDSession::wsPing() {
	mVDWebsocket->wsPing();
}
void VDSession::wsWrite(string msg)
{
	mVDWebsocket->wsWrite(msg);
}
#pragma endregion websockets

// mix
#pragma region mix
/*
void VDSession::setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex) {
	CI_LOG_V("setFboFragmentShaderIndex, before, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
	if (aFboIndex > mFboList.size() - 1) aFboIndex = mFboList.size() - 1;
	if (aFboShaderIndex > mShaderList.size() - 1) aFboShaderIndex = mShaderList.size() - 1;
	CI_LOG_V("setFboFragmentShaderIndex, after, fboIndex: " + toString(aFboIndex) + " shaderIndex " + toString(aFboShaderIndex));
	// 20200216 bug fix mFboList[aFboIndex]->setFragmentShader(aFboShaderIndex, mShaderList[aFboShaderIndex]->getFragmentString(), mShaderList[aFboShaderIndex]->getName());
	mFboList[aFboShaderIndex]->setFragmentShader(aFboShaderIndex, mShaderList[aFboShaderIndex]->getFragmentString(), mShaderList[aFboShaderIndex]->getName());
	if (aFboIndex == 0) {
		setFboAIndex(0, aFboShaderIndex); // TODO 20200216 check 0 useless for now
	}
	else {
		setFboBIndex(1, aFboShaderIndex); // TODO 20200216 check 1 useless for now
	}
	// route message
	// LOOP! mVDWebsocket->changeFragmentShader(mShaderList[aFboShaderIndex]->getFragmentString());
}

unsigned int VDSession::getFboFragmentShaderIndex(unsigned int aFboIndex) {
	unsigned int rtn = mFboList[aFboIndex==0 ? mVDAnimation->getIntUniformValueByIndex(mVDSettings->IFBOA) : mVDAnimation->getIntUniformValueByIndex(mVDSettings->IFBOB)]->getShaderIndex();
	//CI_LOG_V("getFboFragmentShaderIndex, fboIndex: " + toString(aFboIndex)+" shaderIndex: " + toString(rtn));
	if (rtn > mShaderList.size() - 1) rtn = mShaderList.size() - 1;
	return rtn;
}
string VDSession::getShaderName(unsigned int aShaderIndex) {
	return mShaderList[math<int>::min(aShaderIndex, mShaderList.size() - 1)]->getFileNameWithExtension(); // 20200216 was getName()
}
*/
/*
ci::gl::TextureRef VDSession::getShaderThumb(unsigned int aShaderIndex) {
	unsigned int found = 0;
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) found = i;
	}
	return getFboRenderedTexture(found);
}
void VDSession::updateStream(string * aStringPtr) {
	int found = -1;
	for (int i = 0; i < mTextureList.size(); i++)
	{
		if (mTextureList[i]->getType() == mTextureList[i]->STREAM) found = i;
	}
	if (found < 0) {
		// create stream texture
		TextureStreamRef t(new TextureStream(mVDAnimation));
		// add texture xml
		XmlTree			textureXml;
		textureXml.setTag("texture");
		textureXml.setAttribute("id", "9");
		textureXml.setAttribute("texturetype", "stream");
		t->fromXml(textureXml);
		mTextureList.push_back(t);
		found = mTextureList.size() - 1;
	}
	mTextureList[found]->loadFromFullPath(*aStringPtr);
}
string VDSession::getFragmentShaderString(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	return mShaderList[aShaderIndex]->getFragmentString();
}
// shaders
void VDSession::updateShaderThumbFile(unsigned int aShaderIndex) {
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) mFboList[i]->updateThumbFile();
	}
}
void VDSession::removeShader(unsigned int aShaderIndex) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	mShaderList[aShaderIndex]->removeShader();
}
void VDSession::setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName) {
	if (aShaderIndex > mShaderList.size() - 1) aShaderIndex = mShaderList.size() - 1;
	mShaderList[aShaderIndex]->setFragmentString(aFragmentShaderString, aName);
	// if live coding shader compiles and is used by a fbo reload it
	for (int i = 0; i < mFboList.size(); i++)
	{
		if (mFboList[i]->getShaderIndex() == aShaderIndex) setFboFragmentShaderIndex(i, aShaderIndex);
	}
}
void VDSession::setHydraFragmentShaderString(string aFragmentShaderString, string aName) {

	//mShaderList[0]->setFragmentString(aFragmentShaderString, aName);
	//setFboFragmentShaderIndex(0, 0);
	// try to compile a first time to get active uniforms
	mGlslHydra = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);

}
void VDSession::updateHydraUniforms() {
	int index = 300;
	int texIndex = 0;
	int firstDigit = -1;
	auto &uniforms = mGlslHydra->getActiveUniforms();
	string name;
	string textName;
	for (const auto &uniform : uniforms) {
		name = uniform.getName();

		// if uniform is handled
		if (mVDAnimation->isExistingUniform(name)) {
			int uniformType = mVDAnimation->getUniformType(name);
			switch (uniformType)
			{
			case 0:
				// float
				firstDigit = name.find_first_of("0123456789");
				// if contains a digit
				if (firstDigit > -1) {
					index = std::stoi(name.substr(firstDigit));
					textName = name.substr(0, firstDigit);
					if (mVDAnimation->isExistingUniform(textName)) {
						mGlslHydra->uniform(name, mVDAnimation->getFloatUniformValueByName(textName));
					}
					else {
						mVDAnimation->createFloatUniform(name, 400 + index, 0.31f, 0.0f, 1000.0f);
					}
				}
				else {
					mGlslHydra->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
				}
				break;
			case 1:
				// sampler2D
				mGlslHydra->uniform(name, 0);
				break;
			case 2:
				// vec2
				mGlslHydra->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
				break;
			case 3:
				// vec3
				mGlslHydra->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
				break;
			case 4:
				// vec4
				mGlslHydra->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
				break;
			case 5:
				// int
				mGlslHydra->uniform(name, mVDAnimation->getIntUniformValueByName(name));
				break;
			case 6:
				// bool
				mGlslHydra->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
				break;
			default:
				break;
			}
		}
		else {
			if (name != "ciModelViewProjection") {
				mVDSettings->mMsg = "mHydraShader, uniform not found:" + name + " type:" + toString(uniform.getType());
				//CI_LOG_V(mVDSettings->mMsg);
				firstDigit = name.find_first_of("0123456789");
				// if contains a digit
				if (firstDigit > -1) {
					index = std::stoi(name.substr(firstDigit));
					textName = name.substr(0, firstDigit);

					// create uniform
					/* done in VDFbo
					switch (uniform.getType())
					{
					case 5126:

					} */
					/*}
				}
			}
		}
		mGlslHydra->uniform("time", mVDAnimation->getFloatUniformValueByIndex(0));
		mGlslHydra->uniform("resolution", vec2(mVDAnimation->getFloatUniformValueByName("iResolutionX"), mVDAnimation->getFloatUniformValueByName("iResolutionY")));


	}
	string VDSession::getHydraFragmentShaderString() {
		return mShaderList[0]->getFragmentString();
	}


	*/

unsigned int VDSession::fboFromJson(const JsonTree &json, unsigned int aFboIndex) {
	unsigned int rtn = 0;

	rtn = createFboShaderTexture(json, aFboIndex);
	return rtn;
}

#pragma endregion mix

#pragma region textures

/*ci::gl::TextureRef VDSession::getNextInputTexture(unsigned int aTextureIndex) {
	if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	if (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->SEQUENCE) {
		return mTextureList[aTextureIndex]->getNextTexture();
	}
	else {
		return mTextureList[aTextureIndex]->getTexture();
	}

}
ci::gl::TextureRef VDSession::getInputTexture(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getTexture();
}*/
/*


unsigned int VDSession::getInputTextureOriginalWidth(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getOriginalWidth();
}
unsigned int VDSession::getInputTextureOriginalHeight(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getOriginalHeight();
}
int VDSession::getInputTextureXLeft(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getXLeft();
}
void VDSession::setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft) {
	mTextureList[aTextureIndex]->setXLeft(aXLeft);
}
int VDSession::getInputTextureYTop(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getYTop();
}
void VDSession::setInputTextureYTop(unsigned int aTextureIndex, int aYTop) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->setYTop(aYTop);
}
int VDSession::getInputTextureXRight(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getXRight();
}
void VDSession::setInputTextureXRight(unsigned int aTextureIndex, int aXRight) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->setXRight(aXRight);
}
int VDSession::getInputTextureYBottom(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getYBottom();
}
void VDSession::setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->setYBottom(aYBottom);
}

bool VDSession::getInputTextureLockBounds(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getLockBounds();
}
void VDSession::toggleInputTextureLockBounds(unsigned int aTextureIndex) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->toggleLockBounds();
}
void VDSession::togglePlayPause(unsigned int aTextureIndex) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->togglePlayPause();
}
string VDSession::getStatus(unsigned int aTextureIndex) {
	return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getStatus();
}
bool VDSession::loadImageSequence(string aFolder, unsigned int aTextureIndex) {
	//if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
	CI_LOG_V("loadImageSequence " + aFolder + " at textureIndex " + toString(aTextureIndex));
	// add texture xml
	XmlTree			textureXml;
	textureXml.setTag("texture");
	textureXml.setAttribute("id", "0");
	textureXml.setAttribute("texturetype", "sequence");
	textureXml.setAttribute("path", aFolder);
	TextureImageSequenceRef t(new TextureImageSequence(mVDAnimation));
	if (t->fromXml(textureXml)) {
		mTextureList.push_back(t);
		return true;
	}
	else {
		return false;
	}
}
void VDSession::loadMovie(string aFile, unsigned int aTextureIndex) {

}*/

/*
void VDSession::loadAudioFile(string aFile) {
	mTextureList[0]->loadFromFullPath(aFile);
}
bool VDSession::isMovie(unsigned int aTextureIndex) {
	return false;
}
*/
// sequence
/*bool VDSession::isSequence(unsigned int aTextureIndex) {
	aTextureIndex = math<int>::min(aTextureIndex, mTextureList.size() - 1);
	return (mTextureList[aTextureIndex]->getType() == mTextureList[aTextureIndex]->SEQUENCE);
}
bool VDSession::isLoadingFromDisk(unsigned int aTextureIndex) {
	return (mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->isLoadingFromDisk());
}
void VDSession::toggleLoadingFromDisk(unsigned int aTextureIndex) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->toggleLoadingFromDisk();
}
void VDSession::syncToBeat(unsigned int aTextureIndex) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->syncToBeat();
}
void VDSession::reverse(unsigned int aTextureIndex) {
	mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->reverse();
}

*/
#pragma endregion textures
/*void VDSession::load()
{

	CI_LOG_V("VDMix load: ");
	CI_LOG_V("mMixFbos.size() < mWarps.size(), we create a new mixFbo");
	mMixFbos[0].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
	mMixFbos[0].texture = gl::Texture2d::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
	mMixFbos[0].name = "new";
}

// Render the scene into the FBO
ci::gl::Texture2dRef VDSession::getRenderTexture()
{
	gl::ScopedFramebuffer fbScp(mRenderFbo);
	gl::clear(Color::black());
	getMixetteTexture()->bind(0);
	gl::ScopedGlslProg prog(mGlslRender);
	mGlslRender->uniform("TIME", (float)getElapsedSeconds());
	mGlslRender->uniform("iResolution", vec3(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY), 1.0));
	mGlslRender->uniform("iChannel0", 0); // texture 0
	mGlslRender->uniform("iExposure", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IEXPOSURE));
	mGlslRender->uniform("iSobel", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ISOBEL));
	mGlslRender->uniform("iChromatic", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ICHROMATIC));
	gl::drawSolidRect(Rectf(0, 0, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY)));
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());
	mRenderedTexture = mRenderFbo->getColorTexture();
	return  mRenderFbo->getColorTexture();
}*/
/*ci::gl::TextureRef VDSession::getMixetteTexture() {
	gl::ScopedFramebuffer fbScp(mMixetteFbo);
	// clear out the FBO with black
	gl::clear(Color::black());

	// 20200216 TODO CHECK
	mTextureList[mTextureList.size() > 1 ? 1 : 0]->getTexture()->bind(0);
	mHydraFbo->getColorTexture()->bind(1);
	mMixFbos[0].fbo->getColorTexture()->bind(2);
	mFboList[0]->getRenderedTexture()->bind(3);
	mFboList[1]->getRenderedTexture()->bind(4);
	mFboList[2]->getRenderedTexture()->bind(5);
	mFboList[3]->getRenderedTexture()->bind(6);
	mFboList[4]->getRenderedTexture()->bind(7);

	//mImage->bind(0);
	gl::ScopedGlslProg prog(mGlslMixette);
	mGlslMixette->uniform("iResolution", vec3(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY), 1.0));
	mGlslMixette->uniform("iChannel0", 0); // texture 0
	mGlslMixette->uniform("iChannel1", 1);
	mGlslMixette->uniform("iChannel2", 2);
	mGlslMixette->uniform("iChannel3", 3);
	mGlslMixette->uniform("iChannel4", 4);
	mGlslMixette->uniform("iChannel5", 5);
	mGlslMixette->uniform("iChannel6", 6);
	mGlslMixette->uniform("iChannel7", 7);
	mGlslMixette->uniform("iWeight0", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT0));	// weight of channel 0
	mGlslMixette->uniform("iWeight1", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT1));	// weight of channel 1
	mGlslMixette->uniform("iWeight2", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT2));	// weight of channel 2
	mGlslMixette->uniform("iWeight3", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT3));
	mGlslMixette->uniform("iWeight4", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT4));
	mGlslMixette->uniform("iWeight5", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT5));
	mGlslMixette->uniform("iWeight6", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT6));
	mGlslMixette->uniform("iWeight7", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT7));
	//gl::drawSolidRect(getWindowBounds());
	gl::drawSolidRect(Rectf(0, 0, mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY)));
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mMixetteFbo->getSize());
	mMixetteTexture = mMixetteFbo->getColorTexture();
	return mMixetteFbo->getColorTexture();
}
ci::gl::TextureRef VDSession::getMixTexture(unsigned int aMixFboIndex) {
	if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = 0;
	if (!mMixFbos[aMixFboIndex].texture) {
		// should never happen
		mMixFbos[aMixFboIndex].texture = gl::Texture2d::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight);
	}
	if (!mMixFbos[aMixFboIndex].fbo) {
		// should never happen
		mMixFbos[aMixFboIndex].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
	}

	return mMixFbos[aMixFboIndex].texture;
}

ci::gl::TextureRef VDSession::getFboTexture(unsigned int aFboIndex) {
	if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
	return mFboList[aFboIndex]->getFboTexture();
}*/


/*void VDSession::renderBlend()
{
	if (mCurrentBlend > mBlendFbos.size() - 1) mCurrentBlend = 0;
	gl::ScopedFramebuffer scopedFbo(mBlendFbos[mCurrentBlend]);
	gl::clear(Color::black());
	// texture binding must be before ScopedGlslProg
	mFboList[0]->getRenderedTexture()->bind(0);
	mFboList[1]->getRenderedTexture()->bind(1);
	gl::ScopedGlslProg glslScope(mGlslBlend);
	gl::drawSolidRect(Rectf(0, 0, mBlendFbos[mCurrentBlend]->getWidth(), mBlendFbos[mCurrentBlend]->getHeight()));
}

void VDSession::renderHydra() {
	gl::ScopedFramebuffer scopedFbo(mHydraFbo);
	gl::clear(Color::black());

	gl::ScopedGlslProg glslScope(mGlslHydra);
	// useless mGlslHydra->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE));

	gl::drawSolidRect(Rectf(0, 0, mHydraFbo->getWidth(), mHydraFbo->getHeight()));
}
*/
/*void VDSession::renderMix() {
	if (mFboList.size() > 0) {
		if (!mMixFbos[0].fbo) mMixFbos[0].fbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		gl::ScopedFramebuffer scopedFbo(mMixFbos[0].fbo);
		gl::clear(Color::black());
		// render A and B fbos
		mFboList[mVDAnimation->getIntUniformValueByName("iFboA")]->getFboTexture();
		mFboList[mVDAnimation->getIntUniformValueByName("iFboB")]->getFboTexture();
		// texture binding must be before ScopedGlslProg
		mFboList[mVDAnimation->getIntUniformValueByName("iFboA")]->getRenderedTexture()->bind(0);
		mFboList[mVDAnimation->getIntUniformValueByName("iFboB")]->getRenderedTexture()->bind(1);
		gl::ScopedGlslProg glslScope(mGlslMix);
		mGlslMix->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE));

		gl::drawSolidRect(Rectf(0, 0, mMixFbos[0].fbo->getWidth(), mMixFbos[0].fbo->getHeight()));

		// save to a texture
		mMixFbos[0].texture = mMixFbos[0].fbo->getColorTexture();
	}
}

string VDSession::getMixFboName(unsigned int aMixFboIndex) {
	if (aMixFboIndex > mMixFbos.size() - 1) aMixFboIndex = mMixFbos.size() - 1;
	mMixFbos[aMixFboIndex].name = mFboList[0]->getName() + "/" + mFboList[1]->getName();
	return mMixFbos[aMixFboIndex].name;
}*/
/*
void VDSession::readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const DataSourceRef &source) {
	XmlTree			doc;

	CI_LOG_V("VDSession readSettings");
	// try to load the specified xml file
	try {
		doc = XmlTree(source);
		CI_LOG_V("VDSession xml doc ok");
	}
	catch (...) {
		CI_LOG_V("VDSession xml doc error");
	}

	// check if this is a valid file
	bool isOK = doc.hasChild("fbos");
	if (!isOK) return;

	//
	if (isOK) {
		XmlTree mixXml = doc.getChild("fbos");
		fromXml(mixXml);
	}
}
void VDSession::fromXml(const XmlTree &xml) {

	// find fbo childs in xml
	if (xml.hasChild("fbo")) {
		CI_LOG_V("VDSession got fbo childs");
		for (XmlTree::ConstIter fboChild = xml.begin("fbo"); fboChild != xml.end(); ++fboChild) {
			CI_LOG_V("VDSession create fbo ");
			createShaderFbo(fboChild->getAttributeValue<string>("shadername", ""), 0);
		}
	}
}




void VDSession::updateMixUniforms() {
	//vec4 mouse = mVDAnimation->getVec4UniformValueByName("iMouse");

	mGlslMix->uniform("iBlendmode", mVDSettings->iBlendmode);
	mGlslMix->uniform("TIME", mVDAnimation->getFloatUniformValueByIndex(0));
	// was vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0)):
	mGlslMix->uniform("iResolution", vec3(mVDAnimation->getFloatUniformValueByName("iResolutionX"), mVDAnimation->getFloatUniformValueByName("iResolutionY"), 1.0));
	//mGlslMix->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	// 20180318 mGlslMix->uniform("iMouse", mVDAnimation->getVec4UniformValueByName("iMouse"));
	mGlslMix->uniform("iMouse", vec3(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEY), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEZ)));
	mGlslMix->uniform("iDate", mVDAnimation->getVec4UniformValueByName("iDate"));
	mGlslMix->uniform("iWeight0", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT0));	// weight of channel 0
	mGlslMix->uniform("iWeight1", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT1));	// weight of channel 1
	mGlslMix->uniform("iWeight2", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT2));	// weight of channel 2
	mGlslMix->uniform("iWeight3", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT3)); // texture
	mGlslMix->uniform("iWeight4", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT4)); // texture
	mGlslMix->uniform("iChannel0", 0); // fbo shader
	mGlslMix->uniform("iChannel1", 1); // fbo shader
	mGlslMix->uniform("iChannel2", 2); // texture 1
	mGlslMix->uniform("iChannel3", 3); // texture 2
	mGlslMix->uniform("iChannel4", 4); // texture 3

	mGlslMix->uniform("iRatio", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRATIO));//check if needed: +1;
	mGlslMix->uniform("iRenderXY", mVDSettings->mRenderXY);
	mGlslMix->uniform("iZoom", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IZOOM));
	mGlslMix->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFA) * mVDSettings->iAlpha);
	mGlslMix->uniform("iChromatic", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ICHROMATIC));
	mGlslMix->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IROTATIONSPEED));
	mGlslMix->uniform("iCrossfade", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE));
	mGlslMix->uniform("iPixelate", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IPIXELATE));
	mGlslMix->uniform("iExposure", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IEXPOSURE));
	mGlslMix->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->ITOGGLE));
	mGlslMix->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	mGlslMix->uniform("iBackgroundColor", mVDAnimation->getVec3UniformValueByName("iBackgroundColor"));
	mGlslMix->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IVIGN));
	mGlslMix->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IINVERT));
	mGlslMix->uniform("iTempoTime", mVDAnimation->getFloatUniformValueByName("iTempoTime"));
	mGlslMix->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IGLITCH));
	mGlslMix->uniform("iTrixels", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ITRIXELS));
	mGlslMix->uniform("iRedMultiplier", mVDAnimation->getFloatUniformValueByName("iRedMultiplier"));
	mGlslMix->uniform("iGreenMultiplier", mVDAnimation->getFloatUniformValueByName("iGreenMultiplier"));
	mGlslMix->uniform("iBlueMultiplier", mVDAnimation->getFloatUniformValueByName("iBlueMultiplier"));
	mGlslMix->uniform("iFlipH", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));
	mGlslMix->uniform("iFlipV", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));
	mGlslMix->uniform("iPixelX", mVDAnimation->getFloatUniformValueByName("iPixelX"));
	mGlslMix->uniform("iPixelY", mVDAnimation->getFloatUniformValueByName("iPixelY"));
	mGlslMix->uniform("iXorY", mVDSettings->iXorY);
	mGlslMix->uniform("iBadTv", mVDAnimation->getFloatUniformValueByName("iBadTv"));
	mGlslMix->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFPS));
	mGlslMix->uniform("iContour", mVDAnimation->getFloatUniformValueByName("iContour"));
	mGlslMix->uniform("iSobel", mVDAnimation->getFloatUniformValueByName("iSobel"));

}
void VDSession::updateBlendUniforms() {
	mCurrentBlend = getElapsedFrames() % mVDAnimation->getBlendModesCount();
	mGlslBlend->uniform("iBlendmode", mCurrentBlend);
	mGlslBlend->uniform("TIME", mVDAnimation->getFloatUniformValueByIndex(0));
	mGlslBlend->uniform("iResolution", vec3(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, 1.0));
	//mGlslBlend->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	// 20180318 mGlslBlend->uniform("iMouse", mVDAnimation->getVec4UniformValueByName("iMouse"));
	mGlslBlend->uniform("iMouse", vec3(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEY), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IMOUSEZ)));
	mGlslBlend->uniform("iDate", mVDAnimation->getVec4UniformValueByName("iDate"));
	mGlslBlend->uniform("iWeight0", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT0));	// weight of channel 0
	mGlslBlend->uniform("iWeight1", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT1));	// weight of channel 1
	mGlslBlend->uniform("iWeight2", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT2));	// weight of channel 2
	mGlslBlend->uniform("iWeight3", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT3)); // texture
	mGlslBlend->uniform("iWeight4", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IWEIGHT4)); // texture
	mGlslBlend->uniform("iChannel0", 0); // fbo shader
	mGlslBlend->uniform("iChannel1", 1); // fbo shader
	mGlslBlend->uniform("iChannel2", 2); // texture 1
	mGlslBlend->uniform("iChannel3", 3); // texture 2
	mGlslBlend->uniform("iChannel4", 4); // texture 3
	mGlslBlend->uniform("iAudio0", 0);
	mGlslBlend->uniform("iFreq0", mVDAnimation->getFloatUniformValueByName("iFreq0"));
	mGlslBlend->uniform("iFreq1", mVDAnimation->getFloatUniformValueByName("iFreq1"));
	mGlslBlend->uniform("iFreq2", mVDAnimation->getFloatUniformValueByName("iFreq2"));
	mGlslBlend->uniform("iFreq3", mVDAnimation->getFloatUniformValueByName("iFreq3"));
	mGlslBlend->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	mGlslBlend->uniform("iColor", vec3(mVDAnimation->getFloatUniformValueByIndex(1), mVDAnimation->getFloatUniformValueByIndex(2), mVDAnimation->getFloatUniformValueByIndex(3)));
	mGlslBlend->uniform("iBackgroundColor", mVDAnimation->getVec3UniformValueByName("iBackgroundColor"));
	mGlslBlend->uniform("iSteps", (int)mVDAnimation->getFloatUniformValueByIndex(10));
	mGlslBlend->uniform("iRatio", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRATIO));
	mGlslBlend->uniform("width", 1);
	mGlslBlend->uniform("height", 1);
	mGlslBlend->uniform("iRenderXY", mVDSettings->mRenderXY);
	mGlslBlend->uniform("iZoom", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IZOOM));
	mGlslBlend->uniform("iAlpha", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFA) * mVDSettings->iAlpha);
	mGlslBlend->uniform("iChromatic", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ICHROMATIC));
	mGlslBlend->uniform("iRotationSpeed", mVDAnimation->getFloatUniformValueByIndex(9));
	mGlslBlend->uniform("iCrossfade", 0.5f);// blendmode only work if different than 0 or 1
	mGlslBlend->uniform("iPixelate", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IPIXELATE));
	mGlslBlend->uniform("iExposure", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IEXPOSURE));
	mGlslBlend->uniform("iDeltaTime", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IDELTATIME));
	mGlslBlend->uniform("iFade", (int)mVDSettings->iFade);
	mGlslBlend->uniform("iToggle", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->ITOGGLE));
	mGlslBlend->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	mGlslBlend->uniform("iTransition", mVDSettings->iTransition);
	mGlslBlend->uniform("iAnim", mVDSettings->iAnim.value());
	mGlslBlend->uniform("iRepeat", (int)mVDSettings->iRepeat);
	mGlslBlend->uniform("iVignette", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IVIGN));
	mGlslBlend->uniform("iInvert", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IINVERT));
	mGlslBlend->uniform("iDebug", (int)mVDSettings->iDebug);
	mGlslBlend->uniform("iShowFps", (int)mVDSettings->iShowFps);
	mGlslBlend->uniform("iFps", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFPS));
	mGlslBlend->uniform("iTempoTime", mVDAnimation->getFloatUniformValueByName("iTempoTime"));
	mGlslBlend->uniform("iGlitch", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IGLITCH));
	mGlslBlend->uniform("iTrixels", mVDAnimation->getFloatUniformValueByIndex(mVDSettings->ITRIXELS));
	mGlslBlend->uniform("iSeed", mVDSettings->iSeed);
	mGlslBlend->uniform("iRedMultiplier", mVDAnimation->getFloatUniformValueByName("iRedMultiplier"));
	mGlslBlend->uniform("iGreenMultiplier", mVDAnimation->getFloatUniformValueByName("iGreenMultiplier"));
	mGlslBlend->uniform("iBlueMultiplier", mVDAnimation->getFloatUniformValueByName("iBlueMultiplier"));
	mGlslBlend->uniform("iFlipH", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));
	mGlslBlend->uniform("iFlipV", (int)mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));
	mGlslBlend->uniform("iPixelX", mVDAnimation->getFloatUniformValueByName("IPIXELX"));
	mGlslBlend->uniform("iPixelY", mVDAnimation->getFloatUniformValueByName("IPIXELY"));
	mGlslBlend->uniform("iXorY", mVDSettings->iXorY);
	mGlslBlend->uniform("iBadTv", mVDAnimation->getFloatUniformValueByName("iBadTv"));
	mGlslBlend->uniform("iContour", mVDAnimation->getFloatUniformValueByName("iContour"));
	mGlslBlend->uniform("iSobel", mVDAnimation->getFloatUniformValueByName("iSobel"));

}*/
/*
// initialize the shaders list
initShaderList();
mMixesFilepath = getAssetPath("") / "mixes.xml";

//if (fs::exists(mMixesFilepath)) {
	// load textures from file if one exists
	// TODO readSettings(mVDSettings, mVDAnimation, loadFile(mMixesFilepath));
	//}
	// render fbo
mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);
mMixetteFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);

for (size_t i = 0; i < mVDAnimation->getBlendModesCount(); i++)
{
	mBlendFbos[i] = gl::Fbo::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, fboFmt);
}

try
{
	mGlslMix = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getMixFragmentShaderString());
	mGlslBlend = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getMixFragmentShaderString());
	mGlslHydra = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getHydraFragmentShaderString());
	mGlslMixette = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getMixetteFragmentShaderString());
	mGlslRender = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getPostFragmentShaderString());
*/

/*fs::path mPostFilePath = getAssetPath("") / "post.glsl";
if (!fs::exists(mPostFilePath)) {
	mError = mPostFilePath.string() + " does not exist";
	CI_LOG_V(mError);
}
mGlslRender = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), loadString(loadFile(mPostFilePath)));


fs::path mMixetteFilePath = getAssetPath("") / "mixette.glsl";
if (!fs::exists(mMixetteFilePath)) {
	mError = mMixetteFilePath.string() + " does not exist";
	CI_LOG_V(mError);
}
mGlslMixette = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), loadString(loadFile(mMixetteFilePath)));*/

/*
}
catch (gl::GlslProgCompileExc &exc)
{
	mError = "mix error:" + string(exc.what());
	CI_LOG_V("setFragmentString, unable to compile live fragment shader:" + mError);
}
catch (const std::exception &e)
{
	mError = "mix exception:" + string(e.what());
	CI_LOG_V("setFragmentString, error on live fragment shader:" + mError);
}
mVDSettings->mMsg = mError;
mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, 0);
mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, 1);
//mAFboIndex = 0;
//mBFboIndex = 1;
mMode = 0;
mShaderLeft = "";
mShaderRight = "";
// hydra
mHydraUniformsValuesString = "";
mHydraFbo = gl::Fbo::create(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY), fboFmt);
mRenderFbo = gl::Fbo::create(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESX), mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IRESY), fboFmt);
*/



