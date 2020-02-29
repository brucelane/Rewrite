#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// Logger
#include "VDLog.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Watchdog
#include "Watchdog.h"
// std regex
#include <regex>

#pragma warning(push)
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDShader instance
	typedef std::shared_ptr<class VDShader>	VDShaderRef;
	typedef std::vector<VDShaderRef>			VDShaderList;


	class VDShader {
	public:
		VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, gl::TextureRef aVDTexture );
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, gl::TextureRef aVDTexture)
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFileOrPath, aVDTexture));
		}
		/*VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, string aFragmentShaderString = "");
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, string aFragmentShaderString = "")
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFileOrPath, aFragmentShaderString));
		}*/
		//void fromXml(const XmlTree &xml);
		//string							getName() { return mFileNameWithExtension; };
		bool							isValid() { return mValid; };
		string							getFileNameWithExtension() {return mFileNameWithExtension;};
		string							getFragmentString() {
			//if (mFragmentShaderString.empty()) mFragmentShaderString = "void main(void){vec2 uv = gl_FragCoord.xy / iResolution.xy;fragColor = texture(iChannel0, uv);}";
			//if (mFragmentShaderString.size() < 1 || mFragmentShaderString.size() > 256000) mFragmentShaderString = "void main(void){vec2 uv = gl_FragCoord.xy / iResolution.xy;fragColor = texture(iChannel0, uv);}";
			return mFragmentShaderString;
		};
		// thumb image
		// TODO ci::gl::Texture2dRef			getThumb();
		/* 
		NO: gl::GlslProgRef					getShader();
		PRIVATE NOW bool							loadFragmentStringFromFile(string aFileName);
		PRIVATE bool							setFragmentString(string aFragmentShaderString, string aName = "");
		bool							isActive() { return mActive; };
		void							setActive(bool active) { mActive = active; };
		void							removeShader();*/
		ci::gl::Texture2dRef			getThumbTexture();
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;

		//string							mName;
		string							mText;
        bool							mValid;
		gl::GlslProgRef					mShader;
		// include shader lines
		std::string						shaderInclude;
		fs::path mFragFilePath;
		string							mError;
		bool							loadFragmentStringFromFile();// keep PRIVATE
		bool							setFragmentString(string aFragmentShaderString, string aName = "");// keep PRIVATE
		std::string						mFileNameWithExtension;
		std::string						mFragmentShaderString;
		string							ext;
		std::vector<ci::gl::GlslProg::Uniform> mUniforms;
		// fbo
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		gl::FboRef						mThumbFbo;
		ci::gl::Texture2dRef			getFboTexture();
		ci::gl::Texture2dRef			mRenderedTexture;
		string							mName;
		//! Input textures
		gl::TextureRef					mTexture;

		//ci::gl::Texture2dRef			mThumbTexture;
		/*string							mId;
		bool							mActive;
		int								mMicroSeconds;
		//! fragment shader
		fs::path						mFragFile;
		*/
	};
}
