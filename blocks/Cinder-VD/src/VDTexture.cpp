#include "VDTexture.h"


using namespace ci;
using namespace ci::app;

namespace videodromm {
	VDTexture::VDTexture(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const JsonTree &json)
	{
		CI_LOG_V("VDTexture constructor");
		mVDSettings = aVDSettings;
		mVDAnimation = aVDAnimation;

		mTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());

		mTextureName = mCurrentSeqFilename = mLastCachedFilename = (json.hasChild("texturename")) ? json.getValueForKey<string>("texturename") : "0.jpg";
		mTypestr = (json.hasChild("texturetype")) ? json.getValueForKey<string>("texturetype") : "UNKNOWN";
		mMode =  (json.hasChild("texturemode")) ? json.getValueForKey<int>("texturemode") : 0;

		mType = UNKNOWN;
		mStatus = "";
		mLastCachedFilename = mTextureName;
		if (mTextureName == "" || mTextureName == "audio") {
			mTextureName = mTypestr = "audio";
			mType = AUDIO;
			mTexture = mVDAnimation->getAudioTexture();
			
		}
		fs::path texFileOrPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName;
		if (fs::exists(texFileOrPath)) {
			if (fs::is_directory(texFileOrPath)) {
				mType = SEQUENCE;
				mTypestr = "sequence";
				mExt = "jpg";
				mCurrentSeqFilename = mTextureName + " (1)." + mExt;
				mLastCachedFilename = mTextureName + " (1)." + mExt;

				fs::path jpgPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName / mCurrentSeqFilename;
				if (!fs::exists(jpgPath)) {
					// try with png
					mExt = "png";
					mCurrentSeqFilename = mTextureName + " (1)." + mExt;
					mLastCachedFilename = mTextureName + " (1)." + mExt;
				}
				
			}
			else {
				mExt = "";
				int dotIndex = texFileOrPath.filename().string().find_last_of(".");
				if (dotIndex != std::string::npos)  mExt = texFileOrPath.filename().string().substr(dotIndex + 1);
				if (mExt == "jpg" || mExt == "png") {
					mTexture = gl::Texture::create(loadImage(texFileOrPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
					mType = IMAGE;
					mTypestr = "image";
				}
				else {
					/*if (ext == "mp4" || ext == "wmv" || ext == "avi" || ext == "mov") {
						if (!mVideo.isStopped()) {
							mVideo.stop();
						}

						mIsVideoLoaded = mVideo.loadMovie(texFileOrPath);
						mType = MOVIE;
						mVideoDuration = mVideo.getDuration();
						mVideoPos = mVideo.getPosition();
						mVideo.play();
					}*/
				}
			}
		}
		else {
			mTextureName = mTypestr = "audio";
			mType = AUDIO;
			
			mTexture = mVDAnimation->getAudioTexture(); // init with audio texture
		}
		mStatus = mTextureName;
	}
	ci::gl::Texture2dRef VDTexture::getTexture(int aPosition) {
		switch (mType)
		{
		case AUDIO:
			mTexture = mVDAnimation->getAudioTexture();
			break;
			/*case MOVIE:
				mVideo.update();
				mVideoPos = mVideo.getPosition();
				if (mVideo.isStopped() || mVideo.isPaused()) {
					mVideo.setPosition(0.0);
					mVideo.play();
				}
				//mTexture = mVideo.mPlayer->mSources-> mEVRPresenter->;
				break;*/
		case IMAGE:
			break;
		case SEQUENCE:
			if (mMode = 0) {
				// use IBARBEAT else direct access
				if (mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBARBEAT) > 0) {
					// 20200306 if (mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBARBEAT) > 19) {					
					mCurrentSeqFilename = mTextureName + " (" + toString(mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBARBEAT)) + ")." + mExt;
				}
			}
			else {
				mCurrentSeqFilename = mTextureName + " (" + toString(aPosition) + ")." + mExt;
			}
			if (mCachedTextures[mCurrentSeqFilename]) {
				//CI_LOG_V(mCurrentSeqFilename + " in cache");
				mLastCachedFilename = mCurrentSeqFilename;
				mTexture = mCachedTextures[mCurrentSeqFilename];
			}
			else {
				// mTextureName is the folder name
				fs::path fullPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName / mCurrentSeqFilename;
				if (fs::exists(fullPath)) {
					// start profiling
					auto start = Clock::now();
					mCachedTextures[mCurrentSeqFilename] = ci::gl::Texture::create(loadImage(fullPath), gl::Texture::Format().loadTopDown(false));
					auto end = Clock::now();
					auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					int milli = msdur.count();
					mLastCachedFilename = mCurrentSeqFilename;
					mTexture = mCachedTextures[mCurrentSeqFilename];
					mStatus = mCurrentSeqFilename + " " + toString(milli) + "ms";
					CI_LOG_V(mStatus);
					mVDSettings->mMsg = mStatus + "\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);
				}
				else {
					// we want the last texture repeating
					//CI_LOG_V(mCurrentSeqFile + " does not exist");
					mTexture = mCachedTextures[mLastCachedFilename];
				}
			}
			break;
		default:
			break;
		}
		/*if (mType == MOVIE)
		{
			vec2 videoSize = vec2(mVideo.getWidth(), mVideo.getHeight());
			mShader->uniform("uVideoSize", videoSize);
			videoSize *= 0.25f;
			videoSize *= 0.5f;
			ciWMFVideoPlayer::ScopedVideoTextureBind scopedVideoTex(mVideo, 0);
			gl::scale(vec3(videoSize, 1.0f));
		}
		else {
			mTexture->bind(0);

		}*/
		return mTexture;
	}
}