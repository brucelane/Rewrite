/*
	VDShader
	Handles shaders loading and sanitizing
	Input from different sources: filesystem, hydra, bonzorezo, glsleditor, websockets
	Output ISF format (Interactive Shader Format)
*/
// TODO store in database though api

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
		VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aShaderFragmentString, gl::TextureRef aTexture );
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aShaderFragmentString, gl::TextureRef aTexture)
		{
			return shared_ptr<VDShader>(new VDShader(aVDSettings, aVDAnimation, aFileOrPath, aShaderFragmentString, aTexture));
		}
		/*VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aFragmentShaderString = "");
		//void update();
		static VDShaderRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aFragmentShaderString = "")
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
		PRIVATE NOW bool							loadFragmentStringFromFile(const string& aFileName);
		PRIVATE bool							setFragmentString(const string& aFragmentShaderString, const string& aName = "");
		bool							isActive() { return mActive; };
		void							setActive(bool active) { mActive = active; };
		void							removeShader();*/
		ci::gl::Texture2dRef			getThumbTexture();
		void							setInputTexture(ci::gl::Texture2dRef aTextureRef) { mTexture = aTextureRef; };
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
		bool							setFragmentString(const string& aFragmentShaderString, const string& aName = "");// keep PRIVATE
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
