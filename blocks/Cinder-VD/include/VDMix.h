/*
	VDMix
	Handles mixing shaders.
	Maintains a Fbo list
	Outputs severals Fbos depending on the context: mix 2 shaders or several with weights
*/

#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Fbos
#include "VDFbo.h"


// Syphon
#if defined( CINDER_MAC )
#include "cinderSyphon.h"
#endif

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace videodromm;

namespace videodromm
{
	// stores the pointer to the VDMix instance
	typedef std::shared_ptr<class VDMix> 	VDMixRef;
	/*struct VDMixFbo
	{
		ci::gl::FboRef					fbo;
		ci::gl::Texture2dRef			texture;
		string							name;
	};*/
	class VDMix {
	public:
		VDMix(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDMixRef					create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDMix>(new VDMix(aVDSettings, aVDAnimation));
		}
		// RTE in release mode 
		//ci::gl::Texture2dRef			getRenderedTexture(bool reDraw = true);
		// fbolist
		unsigned int					getFboListSize() { 
			return mFboList.size(); 
		};
		bool							isFboValid(unsigned int aFboIndex) {
			bool valid = false;
			if (mFboList.size() > 0) {
				valid = mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->isValid();
			}
			return valid;
			
		};
		bool							getFboBoolUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getBoolUniformValueByIndex(aCtrl);
		};

		void							toggleFboValue(unsigned int aCtrl, unsigned int aFboIndex) {
			mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->toggleValue(aCtrl);
		};
		int								getFboIntUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getIntUniformValueByIndex(aCtrl);
		};

		float							getFboFloatUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getFloatUniformValueByIndex(aCtrl);
		};
		bool							setFboFloatUniformValueByIndex(unsigned int aCtrl, unsigned int aFboIndex, float aValue) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->setFloatUniformValueByIndex(aCtrl, aValue);
		};
		bool									getGlobal(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getGlobal();
		};
		void									toggleGlobal(unsigned int aFboIndex) {
			mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->toggleGlobal();
		};
		string							getFboStatus(unsigned int aFboIndex = 0) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getStatus();
		}
		void							updateShaderThumbFile(unsigned int aFboIndex) {
			mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->updateThumbFile();
		}
		string							getFboInputTextureName(unsigned int aFboIndex = 0) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getTextureName();
		}
		ci::gl::Texture2dRef							getFboInputTexture(unsigned int aFboIndex = 0) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getInputTexture();
		}
		int								getFboInputTextureWidth(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getInputTexture() ? mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getInputTexture()->getWidth() : mVDSettings->mFboWidth;
		};
		int								getFboInputTextureHeight(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getInputTexture() ? mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getInputTexture()->getHeight() : mVDSettings->mFboHeight;
		};
		string							getFboName(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getName();
		};
		string							getFboShaderName(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getShaderName();
		};
		string							getFboTextureName(unsigned int aFboIndex) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getTextureName();
		};
		void							saveFbos() {
			unsigned int i = 0;
			for (auto &fbo : mFboList) {
				JsonTree		json = fbo->toJson(true, i);
				i++;
			}
		};
		std::vector<ci::gl::GlslProg::Uniform>			getUniforms(unsigned int aFboIndex = 0) {
			return mFboList[math<int>::min(aFboIndex, mFboList.size() - 1)]->getUniforms();
		}
		int												loadFragmentShader(const string& aFilePath, unsigned int aFboShaderIndex) {
			int rtn = -1;
			mVDSettings->mMsg = "load " + aFilePath + " at index " + toString(aFboShaderIndex) + "\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);
			bool loaded = false;
			for (auto &fbo : mFboList) {
				rtn++;
				if (!loaded) {
					if (!fbo->isValid()) {
						fbo->loadFragmentStringFromFile(aFilePath);
						loaded = true;
						break;
					}
				}
			}
			if (!loaded) {
				rtn = math<int>::min(aFboShaderIndex, mFboList.size() - 1);
				loaded = mFboList[rtn]->loadFragmentStringFromFile(aFilePath);
			}
			mVDSettings->mMsg = "loaded " + toString(loaded) + " at index " + toString(rtn) + "\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

			return rtn;
		}
		void											loadImageFile(const string& aFile, unsigned int aTextureIndex) {
			int rtn = math<int>::min(aTextureIndex, mFboList.size() - 1);
			fs::path texFileOrPath = aFile;
			if (fs::exists(texFileOrPath)) {

				string ext = "";
				int dotIndex = texFileOrPath.filename().string().find_last_of(".");
				if (dotIndex != std::string::npos)  ext = texFileOrPath.filename().string().substr(dotIndex + 1);
				if (ext == "jpg" || ext == "png") {
					ci::gl::Texture2dRef mTexture = gl::Texture::create(loadImage(texFileOrPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
					mFboList[rtn]->setImageInputTexture(mTexture, texFileOrPath.filename().string());
				}
			}
			
		}
		unsigned int									createFboShaderTexture(const JsonTree &json, unsigned int aFboIndex = 0) {
			unsigned int rtn = 0;
			VDFboRef fboRef = VDFbo::create(mVDSettings, mVDAnimation, json); 
			if (aFboIndex == 0) {
				mFboList.push_back(fboRef);
				rtn = mFboList.size() - 1;
			}
			else {
				rtn = math<int>::min(aFboIndex, mFboList.size() - 1);
				mFboList[rtn] = fboRef;			
			}

			return rtn;
		}
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex) {
			if (mFboList.size() == 0) return mDefaultTexture;
			if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
			return mFboList[aFboIndex]->getRenderedTexture();

		}
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex) {
			if (mFboList.size() == 0) return mDefaultTexture;
			if (aFboIndex > mFboList.size() - 1) aFboIndex = 0;
			return mFboList[aFboIndex]->getTexture();

		}
		ci::gl::TextureRef				getMixetteTexture(unsigned int aFboIndex);
		ci::gl::TextureRef				getRenderedMixetteTexture(unsigned int aFboIndex) { return mMixetteTexture; };
		/*void							update();
		void							updateAudio();
		void							resize();
		bool							handleMouseMove(MouseEvent &event);
		bool							handleMouseDown(MouseEvent &event);
		bool							handleMouseDrag(MouseEvent &event);
		bool							handleMouseUp(MouseEvent &event);
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };

		unsigned int					getMixFbosCount() { return mMixFbos.size(); };
		string							getMixFboName(unsigned int aMixFboIndex);

		// blendmodes
		unsigned int					getFboBlendCount();
		ci::gl::TextureRef				getFboThumb(unsigned int aBlendIndex);
		void							useBlendmode(unsigned int aBlendIndex);


		ci::gl::TextureRef				getMixTexture(unsigned int aMixFboIndex = 0);
		ci::gl::TextureRef				getFboTexture(unsigned int aFboIndex = 0);
		ci::gl::TextureRef				getFboRenderedTexture(unsigned int aFboIndex);
		unsigned int					getBlendFbosCount() { return mBlendFbos.size(); }*/
		// warps

		/*string							getWarpName(unsigned int aWarpIndex) { return mWarpList[aWarpIndex]->getName(); };
		unsigned int					getWarpAFboIndex(unsigned int aWarpIndex) { return mWarpList[aWarpIndex]->getAFboIndex(); };
		unsigned int					getWarpBFboIndex(unsigned int aWarpIndex) { return mWarpList[aWarpIndex]->getBFboIndex(); };
		unsigned int					getWarpAShaderIndex(unsigned int aWarpIndex);
		unsigned int					getWarpBShaderIndex(unsigned int aWarpIndex);
		void							createWarp(string wName = "warp", unsigned int aFboIndex = 0, unsigned int aShaderIndex = 0, unsigned int bFboIndex = 0, unsigned int bShaderIndex = 0, float xFade = 1.0f);
		void							setWarpAFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpBFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex);
		void							setWarpAShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		void							setWarpBShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex);
		float							getWarpCrossfade(unsigned int aWarpIndex) {
			//if (aWarpIndex > mWarpList.size() - 1) aWarpIndex = 0;
			//return mWarpList[aWarpIndex]->ABCrossfade;
			return mVDAnimation->getFloatUniformValueByIndex( mVDSettings->IXFADE);
		};
		void							setWarpCrossfade(unsigned int aWarpIndex, float aCrossfade) {
			//if (aWarpIndex < mWarpList.size()) mWarpList[aWarpIndex]->ABCrossfade = aCrossfade;
			mVDAnimation->setFloatUniformValueByIndex(mVDSettings->IXFADE, aCrossfade);
		};
		void							updateWarpName(unsigned int aWarpIndex);
		//void							crossfadeWarp(unsigned int aWarpIndex, float aValue) { timeline().apply(&mWarpList[aWarpIndex]->ABCrossfade, aValue, 2.0f); };
		bool							isWarpActive(unsigned int aWarpIndex) { return mWarpList[aWarpIndex]->isActive(); };
		void							toggleWarpActive(unsigned int aWarpIndex) { mWarpList[aWarpIndex]->toggleWarpActive(); };
		bool							isWarpSolo(unsigned int aWarpIndex) { return (mSolo == aWarpIndex); };
		void							toggleWarpSolo(unsigned int aWarpIndex) { mSolo = (aWarpIndex == mSolo) ? -1 : aWarpIndex; };
		bool							isWarpDeleted(unsigned int aWarpIndex) { return mWarpList[aWarpIndex]->isDeleted(); };
		void							toggleDeleteWarp(unsigned int aWarpIndex) { mWarpList[aWarpIndex]->toggleDeleteWarp(); };
		bool							isWarpAnimationActive() { return mWarpAnimationActive; };
		void							toggleWarpAnimationActive();
		// common to warps and triangles
		int								getSolo() { return mSolo; };
		unsigned int					getSoloOrActiveIndex();
		unsigned int					getCurrentEditIndex() { return mCurrentEditIndex; };
		void							setCurrentEditIndex(unsigned int aIndex);


		ci::gl::Texture2dRef			getRenderTexture();
		void							save();
		void							load();
		// fbos
		unsigned int 					createShaderFbo(const string& aShaderFilename, unsigned int aFboShaderIndex = 4);
		unsigned int					createShaderFboFromString(const string& aFragmentShaderString, const string& aShaderFilename, const string& aName);
		string							getFboName(unsigned int aFboIndex) { return mFboList[aFboIndex]->getName(); };
		void							setFboInputTexture(unsigned int aFboIndex, unsigned int aInputTextureIndex);
		unsigned int					getFboInputTextureIndex(unsigned int aFboIndex);
		void							fboFlipV(unsigned int aFboIndex);
		bool							isFboFlipV(unsigned int aFboIndex);
		void							setFboFragmentShaderIndex(unsigned int aFboIndex, unsigned int aFboShaderIndex);
		unsigned int					getFboFragmentShaderIndex(unsigned int aFboIndex);

		// textures
		ci::gl::TextureRef				getInputTexture(unsigned int aTextureIndex);
		string							getInputTextureName(unsigned int aTextureIndex);
		unsigned int					getInputTexturesCount();

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
		void							loadImageFile(const string& aFile, unsigned int aTextureIndex);
		void							loadAudioFile(const string& aFile);
		void							loadMovie(const string& aFile, unsigned int aTextureIndex);
		bool							loadImageSequence(const string& aFolder, unsigned int aTextureIndex);
		void							updateStream(string * aStringPtr);

		// movie
		bool							isMovie(unsigned int aTextureIndex);

		// sequence
		bool							isSequence(unsigned int aTextureIndex);
		bool							isLoadingFromDisk(unsigned int aTextureIndex);
		void							toggleLoadingFromDisk(unsigned int aTextureIndex);
		void							syncToBeat(unsigned int aTextureIndex);
		void							reverse(unsigned int aTextureIndex);
		float							getSpeed(unsigned int aTextureIndex);
		void							setSpeed(unsigned int aTextureIndex, float aSpeed);
		int								getPosition(unsigned int aTextureIndex);
		void							setPlayheadPosition(unsigned int aTextureIndex, int aPosition);
		int								getMaxFrame(unsigned int aTextureIndex);
		// shaders
		void							updateShaderThumbFile(unsigned int aShaderIndex);
		void							removeShader(unsigned int aShaderIndex);
		void							setFragmentShaderString(unsigned int aShaderIndex, const string& aFragmentShaderString, const string& aName = "");
		//string							getVertexShaderString(unsigned int aShaderIndex);
		string							getFragmentShaderString(unsigned int aShaderIndex);
		unsigned int					getShadersCount() { return mShaderList.size(); };
		string							getShaderName(unsigned int aShaderIndex);
		ci::gl::TextureRef				getShaderThumb(unsigned int aShaderIndex);
		string							getFragmentString(unsigned int aShaderIndex) { return mShaderList[aShaderIndex]->getFragmentString(); };
		// spout output
		void							toggleSharedOutput(unsigned int aMixFboIndex = 0);
		bool							isSharedOutputActive() { return mSharedOutputActive; };
		unsigned int					getSharedMixIndex() { return mSharedFboIndex; };*/

	private:
		/*bool							mFlipV;
		bool							mFlipH;
		std::string						mFbosPath;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;

		//! mix shader
		gl::GlslProgRef					mMixShader;*/

		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;

		//! Fbos
		//map<int, VDMixFbo>				mMixFbos;
		// maintain a list of fbos specific to this mix
		VDFboList						mFboList;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		gl::TextureRef					mDefaultTexture;
		//! mixette
		gl::FboRef						mMixetteFbo;
		gl::GlslProgRef					mGlslMixette;
		ci::gl::Texture2dRef			mMixetteTexture;
		string							mError;
		//fs::path						mMixesFilepath;
		/*
		//! Shaders
		VDShaderList					mShaderList;
		void							initShaderList();
		//! Textures
		VDTextureList					mTextureList;
		fs::path						mTexturesFilepath;
		bool							initTextureList();
		// blendmodes fbos
		map<int, ci::gl::FboRef>		mBlendFbos;
		int								mCurrentBlend;
		gl::GlslProgRef					mGlslMix, mGlslBlend, mGlslFeedback;
		// render
		void							renderMix();
		void							renderBlend();
		// warping
		string							fileWarpsName;
		//fs::path						mWarpSettings;
		fs::path						mWarpJson;*/

		/*gl::FboRef						mRenderFbo;
		int								warpMixToRender;
		int								mSolo;
		bool							mWarpAnimationActive;
		unsigned int					mWarpActiveIndex;
		// warp rendered texture
		ci::gl::Texture2dRef			mRenderedTexture;

		// common to warps and triangles
		unsigned int					mCurrentEditIndex;
		// shared texture output
		bool							mSharedOutputActive;
		unsigned int					mSharedFboIndex;
		bool							mSpoutInitialized;
		char							mSenderName[256];
#if defined( CINDER_MSW )
		// spout output
		SpoutSender						mSpoutSender;
#endif
		// syphon output
#if defined( CINDER_MAC )
		syphonServer                    mSyphonServer;
#endif
		*/
	};
}
