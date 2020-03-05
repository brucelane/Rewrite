#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
// json
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Utils
#include "VDUtils.h"
// Message router
#include "VDRouter.h"
// Websocket
#include "VDWebsocket.h"
// Animation
#include "VDAnimation.h"
// Fbos
#include "VDFbo.h"
// Logger
#include "VDLog.h"
// Mix
#include "VDMix.h"
// Warping
#include "Warp.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ph::warping;

namespace videodromm {

	typedef std::shared_ptr<class VDSession> VDSessionRef;

	/*struct VDMixFbo
	{
		ci::gl::FboRef					fbo;
		ci::gl::Texture2dRef			texture;
		string							name;
	};*/
	class VDSession {
	public:
		VDSession(VDSettingsRef aVDSettings);
		static VDSessionRef				create(VDSettingsRef aVDSettings);
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		void							update(unsigned int aClassIndex = 0);
		//!
		void							reset();
		void							resetSomeParams();
		void							resize(){
			//mRenderFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, fboFmt);
			// tell the fbos our window has been resized, so they properly scale up or down
			Warp::handleResize(mWarpList);
			Warp::setSize(mWarpList, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight));
		}
		unsigned int					getWarpCount() { return mWarpList.size(); };
		string							getWarpName(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getName(); };
		int								getWarpWidth(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getWidth(); };
		int								getWarpHeight(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getHeight(); };
		void							setWarpWidth(unsigned int aWarpIndex, int aWidth) {
			mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->setWidth(aWidth); 
			mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->resize();
		};
		void							setWarpHeight(unsigned int aWarpIndex, int aHeight) { 
			Warp::handleResize(mWarpList);
			Warp::setSize(mWarpList, ivec2(mVDSettings->mFboWidth, aHeight));
			mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->setHeight(aHeight); 

		};
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getAFboIndex(); };
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex) { return mWarpList[math<int>::min(aWarpIndex, mWarpList.size() - 1)]->getBFboIndex(); };
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
			if (aWarpIndex < mWarpList.size() && aWarpFboIndex < mVDMix->getFboListSize()) {
				mWarpList[aWarpIndex]->setAFboIndex(aWarpFboIndex);
				updateWarpName(aWarpIndex);
			}
		}
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex) {
			if (aWarpIndex < mWarpList.size() && aWarpFboIndex < mVDMix->getFboListSize()) {
				mWarpList[aWarpIndex]->setBFboIndex(aWarpFboIndex);
				updateWarpName(aWarpIndex);
			}
		}

		void							createWarp() {
			auto warp = WarpBilinear::create();
			warp->setName("New");
			warp->setAFboIndex(0);
			warp->setBFboIndex(0);
			warp->setAShaderIndex(0);
			warp->setBShaderIndex(0);
			warp->setAShaderFilename("inputImage.fs");
			warp->setBShaderFilename("inputImage.fs");
			warp->setATextureFilename("audio");
			warp->setBTextureFilename("audio");
			mWarpList.push_back(WarpBilinear::create());
		}
		string							getFboShaderName(unsigned int aFboIndex) {
			return mVDMix->getFboShaderName(aFboIndex);
		}
		string							getFboTextureName(unsigned int aFboIndex) {
			return mVDMix->getFboTextureName(aFboIndex);
		}
		void							saveWarps() {
			/*int i = 0;
			for (auto &warp : mWarpList) {
				// 
				warp->setAShaderFilename(getFboShaderName(warp->getAFboIndex()));
				warp->setATextureFilename(getFboTextureName(warp->getAFboIndex()));
				JsonTree		json;
				string jsonFileName = "warp" + toString(i) + ".json";
				fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
				// write file
				json.pushBack(warp->toJson());
				json.write(jsonFile);
				i++;
			}
			// save warp settings
			Warp::writeSettings(mWarpList, writeFile(mSettings));*/
		}
		ci::gl::TextureRef				getPostFboTexture() {
			return mPostFbo->getColorTexture();
		};
		ci::gl::TextureRef				getWarpFboTexture() {
			return mWarpsFbo->getColorTexture();
		};
		bool							handleMouseMove(MouseEvent &event);
		bool							handleMouseDown(MouseEvent &event);
		bool							handleMouseDrag(MouseEvent &event);
		bool							handleMouseUp(MouseEvent &event);		
		bool							save();
		void							restore();		
		
		
		/*void							fromXml(const ci::XmlTree &xml);

		//! read a xml file and pass back a vector of VDMixs
		void							readSettings(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const ci::DataSourceRef &source);
		string							getWaveFileName() { return mWaveFileName; };
		int								getWavePlaybackDelay() { return mWavePlaybackDelay; };
		string							getMovieFileName() { return mMovieFileName; };
		int								getMoviePlaybackDelay() { return mMoviePlaybackDelay; };
		bool							hasMovie() { return mMovieFileName.length() > 0; };
		unsigned int					getFadeInDelay() {
			return mFadeInDelay;
		};
		unsigned int					getFadeOutDelay() { return mFadeOutDelay; };
		string							getImageSequencePath() { return mImageSequencePath; };
		bool							hasImageSequencePath() { return mImageSequencePath.length() > 0; };
		string							getText() { return mText; };
		int								getTextStart() { return mTextPlaybackDelay; };
		int								getTextEnd() { return mTextPlaybackEnd; };
		bool							hasText() { return mText.length() > 0; };
		*/
		
		// control values
		void							toggleValue(unsigned int aCtrl) {
			mVDWebsocket->toggleValue(aCtrl);
		};
		void							toggleAuto(unsigned int aCtrl) {
			mVDWebsocket->toggleAuto(aCtrl);
		};
		void							toggleTempo(unsigned int aCtrl) {
			mVDWebsocket->toggleTempo(aCtrl);
		};
		void							resetAutoAnimation(unsigned int aIndex) {
			mVDWebsocket->resetAutoAnimation(aIndex);
		};
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDAnimation->getMinUniformValueByIndex(aIndex);
		};
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDAnimation->getMaxUniformValueByIndex(aIndex);
		};

		vec2							getVec2UniformValueByIndex(unsigned int aIndex) {
			return mVDAnimation->getVec2UniformValueByIndex(aIndex);
		};
		vec3							getVec3UniformValueByIndex(unsigned int aIndex) {
			return mVDAnimation->getVec3UniformValueByIndex(aIndex);
		};
		vec4							getVec4UniformValueByIndex(unsigned int aIndex) {
			return mVDAnimation->getVec4UniformValueByIndex(aIndex);
		};
		int								getSampler2DUniformValueByName(string aName) {
			return mVDAnimation->getSampler2DUniformValueByName(aName);
		};
		vec2							getVec2UniformValueByName(string aName) {
			return mVDAnimation->getVec2UniformValueByName(aName);
		};
		vec3							getVec3UniformValueByName(string aName) {
			return mVDAnimation->getVec3UniformValueByName(aName);
		};
		vec4							getVec4UniformValueByName(string aName) {
			return mVDAnimation->getVec4UniformValueByName(aName);
		};
		int								getIntUniformValueByName(string aName) {
			return mVDAnimation->getIntUniformValueByName(aName);
		};
		int								getIntUniformValueByIndex(unsigned int aCtrl) {
			return mVDAnimation->getIntUniformValueByIndex(aCtrl);
		};
		bool							getBoolUniformValueByName(string aName) {
			return mVDAnimation->getBoolUniformValueByName(aName);
		};
		bool							getBoolUniformValueByIndex(unsigned int aCtrl) {
			return mVDAnimation->getBoolUniformValueByIndex(aCtrl);
		}
		float							getFloatUniformValueByIndex(unsigned int aCtrl) {
			return mVDAnimation->getFloatUniformValueByIndex(aCtrl);
		};
		float							getFloatUniformValueByName(string aCtrlName) {
			return mVDAnimation->getFloatUniformValueByName(aCtrlName);
		};
		void							setFloatUniformValueByIndex(unsigned int aCtrl, float aValue) {
			// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
			mVDWebsocket->changeFloatValue(aCtrl, aValue);
		};
		void							setIntUniformValueByIndex(unsigned int aCtrl, int aValue) {
			mVDWebsocket->changeIntValue(aCtrl, aValue);
		};
		void							setBoolUniformValueByIndex(unsigned int aCtrl, float aValue) {
			// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
			mVDWebsocket->changeBoolValue(aCtrl, aValue);
		};
		// tempo
		float							getMaxVolume() { return mVDAnimation->maxVolume; };
		float							getBpm() { return mVDAnimation->getBpm(); };
		void							setBpm(float aBpm) { mVDAnimation->setBpm(aBpm); };
		void							tapTempo() { mVDAnimation->tapTempo(); };
		void							toggleUseTimeWithTempo() { mVDAnimation->toggleUseTimeWithTempo(); };
		void							useTimeWithTempo() { mVDAnimation->useTimeWithTempo(); };
		bool							getUseTimeWithTempo() { return mVDAnimation->getUseTimeWithTempo(); };
		//void							setTimeFactor(const int &aTimeFactor) { mVDAnimation->setTimeFactor(aTimeFactor); };
		// audio
		ci::gl::TextureRef				getAudioTexture() { return mVDAnimation->getAudioTexture(); };
		string							getAudioTextureName() { return mVDAnimation->getAudioTextureName(); };
		float *							getFreqs() { return mVDAnimation->iFreqs; };
		int								getFreqIndexSize() { return mVDAnimation->getFreqIndexSize(); };
		float							getFreq(unsigned int aFreqIndex) { return mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IFREQ0 + aFreqIndex); };
		int								getFreqIndex(unsigned int aFreqIndex) { return mVDAnimation->getFreqIndex(aFreqIndex); };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { mVDAnimation->setFreqIndex(aFreqIndex, aFreq); };
		int								getFFTWindowSize() { return mVDAnimation->mFFTWindowSize; };
		bool							isAudioBuffered() { return mVDAnimation->isAudioBuffered(); };
		void							toggleAudioBuffered() { mVDAnimation->toggleAudioBuffered(); };
		bool							getUseLineIn() { return mVDAnimation->getUseLineIn(); };
		void							setUseLineIn(bool useLineIn) { mVDAnimation->setUseLineIn(useLineIn); };
		void							toggleUseLineIn() { mVDAnimation->toggleUseLineIn(); };
		int								loadFragmentShader(string aFilePath, unsigned int aFboShaderIndex = 4) {
			return mVDMix->loadFragmentShader(aFilePath, aFboShaderIndex);
		};
		/*bool							getFreqWSSend() { return mFreqWSSend; };
		void							toggleFreqWSSend() { mFreqWSSend = !mFreqWSSend; };
		// uniforms
		//void							setMixCrossfade(unsigned int aWarpIndex, float aCrossfade) { mVDSettings->xFade = aCrossfade; mVDSettings->xFadeChanged = true; };
		//float							getMixCrossfade(unsigned int aWarpIndex) { return mVDSettings->xFade; };
		float							getCrossfade() {
			return mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IXFADE);
		};
		void							setCrossfade(float aCrossfade) {
			mVDAnimation->setFloatUniformValueByIndex(mVDSettings->IXFADE, aCrossfade);
		};
		void							setFboAIndex(unsigned int aIndex, unsigned int aFboIndex);
		void							setFboBIndex(unsigned int aIndex, unsigned int aFboIndex);
		unsigned int					getFboAIndex(unsigned int aIndex) { return mVDAnimation->getIntUniformValueByName("iFboA"); };
		unsigned int					getFboBIndex(unsigned int aIndex) { return mVDAnimation->getIntUniformValueByName("iFboB"); };

		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex);
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex);
		bool							loadShaderFolder(string aFolder);

		unsigned int					getShadersCount() { return mShaderList.size(); };
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::TextureRef				getShaderThumb(unsigned int aShaderIndex);
		string							getFragmentString(unsigned int aShaderIndex) { return mShaderList[aShaderIndex]->getFragmentString(); };
		void							setFragmentShaderString(unsigned int aShaderIndex, string aFragmentShaderString, string aName = "");
		//string							getVertexShaderString(unsigned int aShaderIndex) { return mVDMix->getVertexShaderString(aShaderIndex); };
		string							getFragmentShaderString(unsigned int aShaderIndex);
		//string							getVertexShaderString(unsigned int aShaderIndex) { return mVDMix->getVertexShaderString(aShaderIndex); };
		void							setHydraFragmentShaderString(string aFragmentShaderString, string aName = "");
		string							getHydraFragmentShaderString();
		void							updateShaderThumbFile(unsigned int aShaderIndex);
		void							removeShader(unsigned int aShaderIndex);

		// file operations (filedrop, etc)
		//int								loadFileFromAbsolutePath(string aAbsolutePath, int aIndex = 0);

		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex) ;
		
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0);
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex) { return mBlendFbos[aBlendIndex]->getColorTexture(); };
		unsigned int					createShaderFboFromString(string aFragmentShaderString, string aShaderFilename);*/
		int								getFboTextureWidth(unsigned int aFboIndex) { 
			return mVDMix->getFboInputTextureWidth(aFboIndex);
		};
		int								getFboTextureHeight(unsigned int aFboIndex) { 
			return mVDMix->getFboInputTextureHeight(aFboIndex);
		};
		// utils

		float							getTargetFps() { return mTargetFps; };
		void							blendRenderEnable(bool render);
		void							fileDrop(FileDropEvent event);
		void VDSession::setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex) {
			//mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->setInputTexture(mTextureList[aInputTextureIndex]->getTexture());
		}
		// utils
		int								getWindowsResolution() {
			return mVDUtils->getWindowsResolution();
		};
		// fbos
		string							getFboName(unsigned int aFboIndex) { 
			return mVDMix->getFboName(aFboIndex);
			
		};

		unsigned int					getFboListSize() { return mVDMix->getFboListSize(); };
		unsigned int 					createFboShaderTexture(string aShaderFilename, string aTextureFilename) {
			return mVDMix->createFboShaderTexture(aShaderFilename, aTextureFilename);
		};
		unsigned int					fboFromJson(const JsonTree &json);
	
		void							saveFbos() {
			mVDMix->saveFbos();
		};
		
		bool							isFboValid(unsigned int aFboIndex) {
			return mVDMix->isFboValid(aFboIndex);
		};
		
		// fbo 
		bool							getFboBoolUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mVDMix->getFboBoolUniformValueByIndex(aCtrl, aFboIndex);
		};
		
		void							toggleFboValue(unsigned int aCtrl, unsigned int aFboIndex) {
			mVDMix->toggleFboValue(aCtrl, aFboIndex);
		};
		int								getFboIntUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mVDMix->getFboIntUniformValueByIndex(aCtrl, aFboIndex);
		};

		float							getFboFloatUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mVDMix->getFboFloatUniformValueByIndex(aCtrl, aFboIndex);
		};
		bool							setFboFloatUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex, float aValue) {
			return mVDMix->setFboFloatUniformValueByIndex(aCtrl,  aFboIndex, aValue);
		};
		bool									getGlobal(unsigned int aFboIndex) {
			return mVDMix->getGlobal(aFboIndex);
		};
		void									toggleGlobal(unsigned int aFboIndex) {
			mVDMix->toggleGlobal(aFboIndex);
		};
		/*
		void							fboFlipV(unsigned int aFboIndex) {
			mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->flipV();
		};
		bool							isFboFlipV(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->isFlipV();
		};
		void							fboFlipH(unsigned int aFboIndex) {
			mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->flipH();
		};
		bool							isFboFlipH(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->isFlipH();
		};*/
		unsigned int					getFboInputTexturesCount(unsigned int aFboIndex = 0) {
			return 1; //TODO support several textures
		}
		string							getFboStatus(unsigned int aFboIndex = 0) {
			return mVDMix->getFboStatus(aFboIndex);
		}
		void							updateShaderThumbFile(unsigned int aFboIndex) {
			mVDMix->updateShaderThumbFile(aFboIndex);
		}
		string							getFboInputTextureName(unsigned int aFboIndex = 0) {
			return mVDMix->getFboInputTextureName(aFboIndex);
		}
		ci::gl::Texture2dRef							getFboInputTexture(unsigned int aFboIndex = 0) {
			return mVDMix->getFboInputTexture(aFboIndex);
		}
		std::vector<ci::gl::GlslProg::Uniform>			getUniforms(unsigned int aFboIndex = 0) {
			return mVDMix->getUniforms(aFboIndex);
		}
		int								getUniformIndexForName(string aName) {
			return mVDAnimation->getUniformIndexForName(aName);
		};
		
		// Mix
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex) {
			return mVDMix->getFboRenderedTexture(aFboIndex);
		}
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex) {
			return mVDMix->getFboTexture(aFboIndex);
		}
		ci::gl::TextureRef				getMixetteTexture(unsigned int aFboIndex) {
			return mVDMix->getMixetteTexture(aFboIndex);
		};
		// RTE in release mode? 
		//ci::gl::Texture2dRef			getRenderedTexture(bool reDraw = true) { return mVDMix->getRenderedTexture(reDraw); };
		//string							getFboFragmentShaderText(unsigned int aFboIndex);
		// feedback get/set
		/*int								getFeedbackFrames() {
			return mVDMix->getFeedbackFrames();
		};
		void							setFeedbackFrames(int aFeedbackFrames) {
			mVDMix->setFeedbackFrames(aFeedbackFrames);
		};*/
		/*string							getMixFboName(unsigned int aMixFboIndex);
		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);

		unsigned int					getMixFbosCount() { return mMixFbos.size(); };
		ci::gl::TextureRef				getRenderTexture();
		bool							isEnabledAlphaBlending() { return mEnabledAlphaBlending; };
		void							toggleEnabledAlphaBlending() { mEnabledAlphaBlending = !mEnabledAlphaBlending; }
		bool							isRenderTexture() { return mRenderTexture; };
		void							toggleRenderTexture() { mRenderTexture = !mRenderTexture; }
		bool							isFlipH() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH); };
		bool							isFlipV() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		void							flipH(){mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPH, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));};
		void							flipV(){ mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPV, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));};

		// blendmodes
		unsigned int					getFboBlendCount() { return mBlendFbos.size(); };
		void							useBlendmode(unsigned int aBlendIndex) { mVDSettings->iBlendmode = aBlendIndex; };
		*/
		bool							isAutoLayout() { return mVDSettings->mAutoLayout; };
		void							toggleAutoLayout() { mVDSettings->mAutoLayout = !mVDSettings->mAutoLayout; }
		// textures
		void							loadImageFile(string aFile, unsigned int aTextureIndex) {
			mVDMix->loadImageFile(aFile, aTextureIndex);
		};
		/*unsigned int					getInputTexturesCount() {
			return mTextureList.size();
		}
		string VDSession::getInputTextureName(unsigned int aTextureIndex) {
			return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getName();
		}*/
		
		//ci::gl::TextureRef				getInputTexture(unsigned int aTextureIndex);
		/*ci::gl::TextureRef				getCachedTexture(unsigned int aTextureIndex, string aFilename);
		//ci::gl::TextureRef				getNextInputTexture(unsigned int aTextureIndex);
		
		void							loadAudioFile(string aFile);
		void							loadMovie(string aFile, unsigned int aTextureIndex);
		bool							loadImageSequence(string aFolder, unsigned int aTextureIndex);
		//void							toggleSharedOutput(unsigned int aMixFboIndex = 0);
		//bool							isSharedOutputActive() { return mVDMix->isSharedOutputActive(); };
		//unsigned int					getSharedMixIndex() { return mVDMix->getSharedMixIndex(); };
		// move, rotate, zoom methods
		//void							setPosition(int x, int y);
		//void							setZoom(float aZoom);
		int								getInputTextureXLeft(unsigned int aTextureIndex);
		void							setInputTextureXLeft(unsigned int aTextureIndex, int aXLeft);
		int								getInputTextureYTop(unsigned int aTextureIndex);
		void							setInputTextureYTop(unsigned int aTextureIndex, int aYTop);
		int								getInputTextureXRight(unsigned int aTextureIndex);
		void							setInputTextureXRight(unsigned int aTextureIndex, int aXRight);
		int								getInputTextureYBottom(unsigned int aTextureIndex);
		void							setInputTextureYBottom(unsigned int aTextureIndex, int aYBottom);
		bool							isFlipVInputTexture(unsigned int aTextureIndex);
		bool							isFlipHInputTexture(unsigned int aTextureIndex);
		void							inputTextureFlipV(unsigned int aTextureIndex);
		void							inputTextureFlipH(unsigned int aTextureIndex);
		bool							getInputTextureLockBounds(unsigned int aTextureIndex);
		void							toggleInputTextureLockBounds(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalWidth(unsigned int aTextureIndex);
		unsigned int					getInputTextureOriginalHeight(unsigned int aTextureIndex);
		void							togglePlayPause(unsigned int aTextureIndex);
		string							getStatus(unsigned int aTextureIndex);
		// movie
		bool							isMovie(unsigned int aTextureIndex);
		// sequence
		bool							isSequence(unsigned int aTextureIndex);
		bool							isLoadingFromDisk(unsigned int aTextureIndex);
		void							toggleLoadingFromDisk(unsigned int aTextureIndex);
		void							syncToBeat(unsigned int aTextureIndex);
		void							reverse(unsigned int aTextureIndex);*/
		/*float							getSpeed(unsigned int aTextureIndex) {
			return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getSpeed();
		};
		void							setSpeed(unsigned int aTextureIndex, float aSpeed) {
			//if (aTextureIndex > mTextureList.size() - 1) aTextureIndex = mTextureList.size() - 1;
			//mTextureList[aTextureIndex]->setSpeed(aSpeed);
			for (int i = 0; i < mTextureList.size() - 1; i++)
			{
				mTextureList[i]->setSpeed(aSpeed);
			}
		};
		void							incrementSequencePosition() {
			for (unsigned int i = 0; i < mTextureList.size() - 1; i++)
			{
				setPlayheadPosition(i, getPosition(i) + 1);
			}
		}
		void							decrementSequencePosition() {
			for (unsigned int i = 0; i < mTextureList.size() - 1; i++)
			{
				setPlayheadPosition(i, getPosition(i) - 1);
			}
		}
		int								getPosition(unsigned int aTextureIndex) {
			return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getPosition();
		};
		void							setPlayheadPosition(unsigned int aTextureIndex, int aPosition) {
			mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->setPlayheadPosition(aPosition);
		};
		int								getMaxFrame(unsigned int aTextureIndex) {
			return mTextureList[math<int>::min(aTextureIndex, mTextureList.size() - 1)]->getMaxFrame();
		};*/
		// websockets
		void							wsConnect();
		void							wsPing();
		void							wsWrite(std::string msg);
		/*void							sendFragmentShader(unsigned int aShaderIndex);*/
		// midi
		void							midiSetup() { mVDRouter->midiSetup(); };
		void							midiOutSendNoteOn(int i, int channel, int pitch, int velocity) { mVDRouter->midiOutSendNoteOn(i, channel, pitch, velocity); };
		int								getMidiInPortsCount() { return mVDRouter->getMidiInPortsCount(); };
		string							getMidiInPortName(int i) { return mVDRouter->getMidiInPortName(i); };
		bool							isMidiInConnected(int i) { return mVDRouter->isMidiInConnected(i); };
		int								getMidiOutPortsCount() { return mVDRouter->getMidiOutPortsCount(); };
		string							getMidiOutPortName(int i) { return mVDRouter->getMidiOutPortName(i); };
		bool							isMidiOutConnected(int i) { return mVDRouter->isMidiOutConnected(i); };
		void							openMidiInPort(int i) { mVDRouter->openMidiInPort(i); };
		void							closeMidiInPort(int i) { mVDRouter->closeMidiInPort(i); };
		void							openMidiOutPort(int i) { mVDRouter->openMidiOutPort(i); };
		void							closeMidiOutPort(int i) { mVDRouter->closeMidiOutPort(i); };
		//! window management
		void							createWindow() { cmd = 0; };
		void							deleteWindow() { cmd = 1; };
		
		int								getCmd() { int rtn = cmd; cmd = -1; return rtn; };
		// utils
		/*float							formatFloat(float f) { return mVDUtils->formatFloat(f); };

		void							load();
		void							updateAudio() {mTextureList[0]->getTexture();}
		void							updateMixUniforms();
		void							updateBlendUniforms();
		// hydra
		string							getHydraUniformsString() { return mHydraUniformsValuesString; };
		ci::gl::TextureRef				getHydraTexture() { return mHydraFbo->getColorTexture(); };*/

		// modeint
		int								getMode() { return mMode; };
		void							setMode(int aMode) { mMode = aMode; };;
		int								getModesCount() { return mModesList.size() - 1; };
		void							toggleUI() { mShowUI = !mShowUI; };
		bool							showUI() { return mShowUI; };
		string							getModeName(unsigned int aMode) {
			if (aMode > mModesList.size() - 1) aMode = mModesList.size() - 1;
			return mModesList[aMode];
		}
	private:
		int								mMode;

		// Settings
		VDSettingsRef					mVDSettings;
		// Utils
		VDUtilsRef						mVDUtils;
		// Message router
		VDRouterRef						mVDRouter;
		// VDWebsocket
		VDWebsocketRef					mVDWebsocket;
		// Animation
		VDAnimationRef					mVDAnimation;
		// Log
		VDLogRef						mVDLog;
		// Mix
		VDMixRef						mVDMix;
		const string					sessionFileName = "session.json";
		fs::path						sessionPath;
		// tempo
		float							mFpb;
		float							mOriginalBpm;
		float							mTargetFps;
		// audio
		bool							mFreqWSSend;
		// files and paths
		/*string							mWaveFileName;
		string							mMovieFileName;
		string							mImageSequencePath;
		// delay
		int								mWavePlaybackDelay;
		int								mMoviePlaybackDelay;
		unsigned int					mFadeInDelay;
		unsigned int					mFadeOutDelay;
		// font and text
		string							mText;
		int								mTextPlaybackDelay;
		int								mTextPlaybackEnd;
		//! Fbos
		// maintain a list of fbo for right only or left/right or more fbos specific to this mix
		//VDFboList						mFboList;
		fs::path						mFbosFilepath;
		// fbo
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		bool							mEnabledAlphaBlending;
		bool							mRenderTexture;
		//! Warps
		int								mSelectedWarp;
		//! Shaders
		string							mShaderLeft;
		string							mShaderRight;
		//! hydra
		string							mHydraUniformsValuesString;
		void							updateHydraUniforms();
		void							renderHydra();
		ci::gl::FboRef					mHydraFbo;
		gl::GlslProgRef					mGlslHydra;
		gl::GlslProgRef					mGlslRender;
		//! textures
		int								mWidth;
		int								mHeight;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		void							updateStream(string * aStringPtr);*/
		//! window management
		int								cmd = -1;
		bool							mShowUI = false;
		// mix

/*
		std::string						mFbosPath;

		//! mix shader
		gl::GlslProgRef					mMixShader;
		string							mError;

		//! Fbos
		map<int, VDMixFbo>				mMixFbos;
		*/
		// maintain a list of fbos specific to this mix
		//VDFboList						mFboList;


		/*fs::path						mMixesFilepath;
		//unsigned int					mAFboIndex;
		//unsigned int					mBFboIndex;

		//! Shaders
		VDShaderList					mShaderList;
		void							initShaderList();*/
		//! Textures
		//VDTextureList					mTextureList;
		//fs::path						mTexturesFilepath;
		bool							initTextureList();
		//! Modes
		map<int, string>				mModesList;
		// blendmodes fbos
		map<int, ci::gl::FboRef>		mBlendFbos;
		int								mCurrentBlend;
		/*gl::GlslProgRef					mGlslMix, mGlslBlend, mGlslFeedback, mGlslMixette;
		// render
		void							renderMix();
		void							renderBlend();
		// warping
		gl::FboRef						mRenderFbo;
		// warp rendered texture
		ci::gl::Texture2dRef			mRenderedTexture, mMixetteTexture;
		*/
		//! fbos
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		gl::FboRef						mWarpsFbo;
		gl::FboRef						mPostFbo;
		//! shaders
		gl::GlslProgRef					mGlslPost;
		void							renderPostToFbo();
		void							renderWarpsToFbo();
		// warps

		WarpList						mWarpList;
		fs::path						mSettings;
		void							updateWarpName(unsigned int aWarpIndex) {
			if (aWarpIndex < mWarpList.size()) {
				mWarpList[aWarpIndex]->setName(mVDMix->getFboName(mWarpList[aWarpIndex]->getAFboIndex()));
			}
		}
		void							loadFbos();

		void							loadWarps() {
			/*int i = 0;
			for (auto &warp : mWarpList) {
				i = math<int>::min(i, mWarpList.size() - 1);
				string jsonFileName = "warp" + toString(i) + ".json";
				fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
				if (fs::exists(jsonFile)) {
					JsonTree json(loadFile(jsonFile));
					warp->fromJson(json);
					if (json[0].hasChild("warp")) {
						JsonTree warpJsonTree(json[0].getChild("warp"));
						string shaderFileName = (warpJsonTree.hasChild("ashaderfilename")) ? warpJsonTree.getValueForKey<string>("ashaderfilename") : "inputImage.fs";
						string textureFileName = (warpJsonTree.hasChild("atexturefilename")) ? warpJsonTree.getValueForKey<string>("atexturefilename") : "audio";
						createFboShaderTexture(shaderFileName, textureFileName);
						//mVDSession->fboFromJson(warpJsonTree);
						warp->setName(shaderFileName + "-" + textureFileName);
						warp->setAFboIndex(i);
						warp->setBFboIndex(i);
						warp->setAShaderIndex(i);
						warp->setBShaderIndex(i);
						warp->setAShaderFilename(shaderFileName);
						warp->setBShaderFilename(shaderFileName);
						warp->setATextureFilename(textureFileName);
						warp->setBTextureFilename(textureFileName);
					}
					i++;
				}
			}*/
		}
	};

}
