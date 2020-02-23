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
// textures
#include "VDTexture.h"

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
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aShaderFilename, string aTextureFilename);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aShaderFilename, string aTextureFilename) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation, aShaderFilename,  aTextureFilename);
		}		
		ci::gl::Texture2dRef getRenderedTexture();
		/*Area getSrcArea() { 
			return mSrcArea; 
		};*/
		bool									setFragmentString(string aFragmentShaderString, string aName = "");
		bool									loadFragmentStringFromFile(string aFileName);		
		bool									isValid() {
			return mValid; 
		};
		/*void									flipV() {
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPV, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));
		};
		void									flipH() { 
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPH, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));
		};
		bool									isFlipH() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		bool									isFlipV() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH); };*/
		std::string								getName() { return mShaderName; };
		std::string								getTextureName() { return mTextureName; };
		ci::gl::Texture2dRef					getInputTexture() { return mTexture; };
		void									setInputTexture(ci::gl::Texture2dRef aTextureRef) { mTexture = aTextureRef; };
		std::vector<ci::gl::GlslProg::Uniform>	getUniforms() { return mUniforms; };
		ci::JsonTree							toJson(bool save = false) const;
		// uniforms
		string									getUniformNameForIndex(int aIndex) {
			return controlIndexes[aIndex];
		};
		int										getUniformIndexForName(string aName) {
			return shaderUniforms[aName].index;
		};
		bool									getBoolUniformValueByIndex(unsigned int aIndex) {
			//101 mVDSettings->IFLIPH
			//102 mVDSettings->IFLIPV
			return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
		};
		bool									getBoolUniformValueByName(string aName) {
			return shaderUniforms[aName].boolValue;
		}
		bool									toggleValue(unsigned int aIndex) {
			shaderUniforms[getUniformNameForIndex(aIndex)].boolValue = !shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
			return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
		};
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;
		//! Input textures
		//VDTextureList					mTextureList;
		gl::TextureRef					mTexture;
		//int								mInputTextureIndex;
		//! shader
		gl::GlslProgRef					mShader;
		std::vector<ci::gl::GlslProg::Uniform> mUniforms;
		string							mShaderName = "";
		string							mShaderFileName = "";
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
		//Area							mSrcArea;
		//bool							mFlipV;
		//bool							mFlipH;
		bool							mUseBeginEnd;
		bool							isReady;
		ci::gl::Texture2dRef			mRenderedTexture;
		ci::gl::Texture2dRef			getFboTexture();
		// uniforms
		map<int, string>				controlIndexes;
		map<string, VDUniform>			shaderUniforms;
		void							createBoolUniform(string aName, int aCtrlIndex, bool aValue = false) {
			controlIndexes[aCtrlIndex] = aName;
			shaderUniforms[aName].boolValue = aValue;
			shaderUniforms[aName].index = aCtrlIndex;
			shaderUniforms[aName].uniformType = 6;
		}
	};
}