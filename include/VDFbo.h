#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/ImageIo.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
// useless? using namespace videodromm;

namespace videodromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> 			VDFboRef;
	typedef std::vector<VDFboRef>					VDFboList;

	class VDFbo  { // TODO : public VDTexture ?
	public:
		VDFbo(VDSettingsRef aVDSettings, string aShaderFilename);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, string aShaderFilename) {
			return std::make_shared<VDFbo>(aVDSettings, aShaderFilename);
		}		
		ci::gl::Texture2dRef getRenderedTexture();
		Area getSrcArea() { 
			return mSrcArea; 
		};
		bool							setFragmentString(string aFragmentShaderString, string aName = "");
		bool							loadFragmentStringFromFile(string aFileName);		
		bool							isValid() {
			return mValid; 
		};
		void							flipV() {
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPV, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));
		};
		void							flipH() { 
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPH, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));
		};
		bool							isFlipH() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		bool							isFlipV() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH); };
		
	private:
		// Settings
		VDSettingsRef					mVDSettings;

		gl::TextureRef					mTexture;
		int								mInputTextureIndex;
		//! shader
		gl::GlslProgRef					mShader;
		string							mShaderName = "";
		string							mTextureName = "";
		std::string						mFragmentShaderString;
		std::string						mFileNameWithExtension;
		fs::path						mFragFile;
		bool							mActive;
		int								mMicroSeconds;
		// include shader lines
		std::string						shaderInclude;
		string							mError;
		bool							mValid = false;
		//! Fbo
		gl::FboRef						mFbo;
		//gl::FboRef						mThumbFbo;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		Area							mSrcArea;
		//bool							mFlipV;
		//bool							mFlipH;
		bool							mUseBeginEnd;
		bool							isReady;
		ci::gl::Texture2dRef			mRenderedTexture;
		ci::gl::Texture2dRef			getFboTexture();
		VDAnimationRef					mVDAnimation;
	};
}