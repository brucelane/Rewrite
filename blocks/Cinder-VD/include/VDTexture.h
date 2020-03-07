#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
// Settings
#include "VDAnimation.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;


namespace videodromm
{
	/*
	** ---- Texture parent class ------------------------------------------------
	*/
	// stores the pointer to the VDTexture instance
	typedef std::shared_ptr<class VDTexture> 	VDTextureRef;
	typedef std::vector<VDTextureRef>			VDTextureList;
	//typedef map<string, ci::gl::TextureRef>		VDCachedTextures;

	// for profiling
	typedef std::chrono::high_resolution_clock Clock;

	class VDTexture  {
	public:
		VDTexture(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const JsonTree &json);
		
		static VDTextureRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const JsonTree &json) {
			return std::make_shared<VDTexture>(aVDSettings, aVDAnimation, json);
		}
		
		typedef enum { UNKNOWN, IMAGE, SEQUENCE, CAMERA, SHARED, AUDIO, STREAM } TextureType;
	
		VDTexture(TextureType aType = UNKNOWN);
		~VDTexture(void) { };
		ci::gl::Texture2dRef					getTexture();
		std::string								getTextureName() { return mTextureName; };
		ci::gl::Texture2dRef					getCachedTexture(string aFilename);
		string									getStatus() { return mStatus; };
		string									getType() { return mTypestr; };

		void									setImageInputTexture(ci::gl::Texture2dRef aTextureRef, string aTextureFilename) {
			mType = IMAGE;
			mTexture = aTextureRef;
			mTextureName = aTextureFilename;
			/*if (shaderToLoad) {
				shaderToLoad->setInputTexture(aTextureRef);
				shaderToLoad->getThumbTexture();
			}*/
		};
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;
		gl::TextureRef					mTexture;
		map<string, ci::gl::TextureRef>	mCachedTextures;
		string							mTextureName = "";
		string							mLastCachedFilename = "";
		string							mCurrentSeqFilename = "";
		string							mStatus = "";
		TextureType						mType = UNKNOWN;
		string							mTypestr = "";

	};
}
