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
// shader
#include "VDShader.h"
// textures
//#include "VDTexture.h"
// video
//#include "ciWMFVideoPlayer.h"

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
	// for profiling
	typedef std::chrono::high_resolution_clock		Clock;

	class VDFbo { // TODO : public VDTexture ?
	public:
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aShaderFilename, string aTextureFilename);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aShaderFilename, string aTextureFilename) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation, aShaderFilename, aTextureFilename);
		}
		typedef enum { UNKNOWN, IMAGE, SEQUENCE, MOVIE, CAMERA, SHARED, AUDIO, STREAM } TextureType;
		ci::gl::Texture2dRef getRenderedTexture();

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
		string									getStatus() { return mStatus; };
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
		// bool
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
		// int
		int										getIntUniformValueByName(string aName) {
			return shaderUniforms[aName].intValue;
		};
		int										getIntUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].intValue;
		};
		float									getFloatUniformValueByName(string aName) {
			if (aName.length() > 0) {
				return shaderUniforms[aName].floatValue;
			}
			else {
				CI_LOG_V("getFloatUniformValueByName name empty");
				return 1.0f;
			}
		}
		int										getFloatUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].floatValue;
		};
		bool									setFloatUniformValueByIndex(unsigned int aIndex, float aValue) {
			bool rtn = false;
			// we can't change iTime at index 0
			if (aIndex > 0) {
				/*if (aIndex == 31) {
					CI_LOG_V("old value " + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue) + " newvalue " + toString(aValue));
				}*/
				float f = shaderUniforms[getUniformNameForIndex(aIndex)].floatValue;
				if (aIndex == 41 && aValue > 0.0F) {
					CI_LOG_V(f);
				}
				string uniformName = getUniformNameForIndex(aIndex);
				if (shaderUniforms[uniformName].floatValue != aValue) {
					if ((aValue >= shaderUniforms[uniformName].minValue && aValue <= shaderUniforms[uniformName].maxValue) || shaderUniforms[uniformName].autobass || shaderUniforms[uniformName].automid || shaderUniforms[uniformName].autotreble) {
						shaderUniforms[uniformName].floatValue = aValue;
						rtn = true;
					}
				}
				// not all controls are from 0.0 to 1.0
				/* not working float lerpValue = lerp<float, float>(shaderUniforms[getUniformNameForIndex(aIndex)].minValue, shaderUniforms[getUniformNameForIndex(aIndex)].maxValue, aValue);
				if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != lerpValue) {
					shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = lerpValue;
					rtn = true;
				}*/
			}
			else {
				// no max 
				if (aIndex == 0) shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = aValue;
			}
			return rtn;
		}
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;
		//! Input textures
		gl::TextureRef					mTexture;
		map<string, ci::gl::TextureRef>	mCachedTextures;
		string							mLastCachedFilename;
		string							mCurrentSeqFilename;
		string							mStatus;
		TextureType						mType;
		// video
		/*ciWMFVideoPlayer				mVideo;
		float							mVideoPos;
		float							mVideoDuration;
		bool							mIsVideoLoaded;
	*/
	//! shader
		gl::GlslProgRef					mShader;
		std::vector<ci::gl::GlslProg::Uniform> mUniforms;
		string							mShaderName = "";
		string							mShaderFileName = "";
		string							mTextureName = "";
		std::string						mFragmentShaderString;
		std::string						mFileNameWithExtension;
		//fs::path						mFragFile;
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
		void							createIntUniform(string aName, int aCtrlIndex, int aValue = 1) {
			controlIndexes[aCtrlIndex] = aName;
			shaderUniforms[aName].index = aCtrlIndex;
			shaderUniforms[aName].uniformType = 5;
			shaderUniforms[aName].isValid = true;
			shaderUniforms[aName].intValue = aValue;
		};
		void							createFloatUniform(string aName, int aCtrlIndex, float aValue, float aMin, float aMax) {
			controlIndexes[aCtrlIndex] = aName;
			shaderUniforms[aName].minValue = aMin;
			shaderUniforms[aName].maxValue = aMax;
			shaderUniforms[aName].defaultValue = aValue;
			shaderUniforms[aName].boolValue = false;
			shaderUniforms[aName].autotime = false;
			shaderUniforms[aName].automatic = false;
			shaderUniforms[aName].autobass = false;
			shaderUniforms[aName].automid = false;
			shaderUniforms[aName].autotreble = false;
			shaderUniforms[aName].index = aCtrlIndex;
			shaderUniforms[aName].floatValue = aValue;
			shaderUniforms[aName].uniformType = 0;
			shaderUniforms[aName].isValid = true;
		}
	};
}