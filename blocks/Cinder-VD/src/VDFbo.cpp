#include "VDFbo.h"

namespace videodromm {
	VDFbo::VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aShaderFilename, string aTextureFilename)
		:mValid(false)
	{
		CI_LOG_V("VDFbo constructor");
		mShaderName = aShaderFilename;
		mShaderFileName = aShaderFilename;
		mTextureName = aTextureFilename;
		mCurrentSeqFilename = aTextureFilename;
		mLastCachedFilename = aTextureFilename;
		shaderInclude = loadString(loadAsset("shadertoy.vd"));
		shaderInclude = "#version 150\n"
			"// shadertoy specific\n"
			"uniform vec2      	RENDERSIZE;\n"
			"uniform vec3 		iResolution;\n"
			"uniform float     	TIME;\n"
			"uniform float     	iZoom;\n"
			"uniform vec4      	iMouse;\n"
			"uniform bool       iFlipV;\n"
			"uniform bool       iFlipH;\n"
			"uniform float     	iBarBeat; \n"
			"uniform float     	iExposure; \n"
			"uniform float     	iBeat; \n"
			"uniform float     	iBpm; \n"
			"uniform float     	iBar; \n"
			"uniform float     	iTimeFactor; \n"
			"uniform float     	iRotationSpeed; \n"
			"uniform bool		iDebug; \n"
			"uniform sampler2D 	inputImage;\n"
			"out vec4 fragColor;\n"
			"#define IMG_NORM_PIXEL texture2D\n";

		mVDSettings = aVDSettings;
		mVDAnimation = aVDAnimation;
		// init texture
		mTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
		//twice mVDAnimation = VDAnimation::create(mVDSettings);
		mUseBeginEnd = false;
		isReady = false;
		//mInputTextureIndex = 0;
		//mSrcArea = Area(0, 0, 10, 10);
		mType = UNKNOWN;
		mStatus = "";
		mLastCachedFilename = mTextureName;
		if (mTextureName == "" || mTextureName == "audio") {
			mTextureName = "audio";
			mType = AUDIO;
			mTexture = mVDAnimation->getAudioTexture();

		}
		fs::path texFileOrPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName;
		if (fs::exists(texFileOrPath)) {
			if (fs::is_directory(texFileOrPath)) {
				mType = SEQUENCE;
				mCurrentSeqFilename = mTextureName + " (1).jpg";
				mLastCachedFilename = mTextureName + " (1).jpg";
			}
			else {
				string ext = "";
				int dotIndex = texFileOrPath.filename().string().find_last_of(".");
				if (dotIndex != std::string::npos)  ext = texFileOrPath.filename().string().substr(dotIndex + 1);
				if (ext == "jpg" || ext == "png") {
					mTexture = gl::Texture::create(loadImage(texFileOrPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
					mType = IMAGE;
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
			//mRenderedTexture = gl::Texture::create(loadImage(texPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
		}
		else {
			mTextureName = "audio";
			mType = AUDIO;
			mTexture = mVDAnimation->getAudioTexture(); // init with audio texture
			//
			//mRenderedTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
		}
		mStatus = mTextureName;
		//mSrcArea = mTexture->getBounds();
		// init texture
		// init the fbo whatever happens next
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		mError = "";
		mActive = true;
		mValid = loadFragmentStringFromFile(mShaderName);

		if (mValid) {
			CI_LOG_V("VDShaders constructor success");
		}
		else {
			CI_LOG_V("VDShaders constructor failed, do not use");
		}
	}
	VDFbo::~VDFbo(void) {
	}

	bool VDFbo::loadFragmentStringFromFile(string aFileName) {
		/*
		load from VDShader
		*/
		mValid = false;
		if (aFileName.length() > 0) {
			if (mType == MOVIE) {
				try {
					mShaderName = mShaderFileName = "video_texture.fs.glsl";
					mShader = gl::GlslProg::create(gl::GlslProg::Format()
						.vertex(loadAsset("video_texture.vs.glsl"))
						.fragment(loadAsset("video_texture.fs.glsl")));
					mValid = true;
					CI_LOG_V("fbo video_texture vtx-frag compiled");
				}
				catch (gl::GlslProgCompileExc &exc) {
					mError = string(exc.what());
					CI_LOG_V("fbo unable to load/compile vtx-frag video_texture shader:" + string(exc.what()));
				}
				catch (const std::exception &e) {
					mError = string(e.what());
					CI_LOG_V("fbo unable to load vtx-frag video_texture shader:" + string(e.what()));
				}
			}
			else {
				// load fragment shader
				VDShaderRef shaderToLoad = VDShader::create(mVDSettings, mVDAnimation, aFileName, mTexture);
				if (shaderToLoad->isValid()) {
					mFileNameWithExtension = shaderToLoad->getFileNameWithExtension();//was mFragFile.filename().string();
					mFragmentShaderString = shaderToLoad->getFragmentString();//was loadString(loadFile(mFragFile));
					mValid = setFragmentString(mFragmentShaderString, shaderToLoad->getFileNameWithExtension());// was mFragFile.filename().string());
				}
				else {
					mError = "Invalid shader file " + aFileName;
					// load default fragment shader
					try {
						mShaderName = mShaderFileName = "default.fs";
						mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getDefaultFragmentShaderString());
						mValid = true;
						CI_LOG_V("fbo default vtx-frag compiled");
					}
					catch (gl::GlslProgCompileExc &exc) {
						mError = string(exc.what());
						CI_LOG_V("fbo unable to load/compile vtx-frag shader:" + string(exc.what()));
					}
					catch (const std::exception &e) {
						mError = string(e.what());
						CI_LOG_V("fbo unable to load vtx-frag shader:" + string(e.what()));
					}
				}
				/*
				CI_LOG_V("loadFragmentStringFromFile, loading " + aFileName);
				
				mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / aFileName;
				if (aFileName.length() > 0 && fs::exists(mFragFile)) {
					mFileNameWithExtension = mFragFile.filename().string();
					mFragmentShaderString = loadString(loadFile(mFragFile));
					mValid = setFragmentString(mFragmentShaderString, mFragFile.filename().string());

					CI_LOG_V(mFragFile.string() + " loaded and compiled");
				}
				else {
					// file does not exist, try with parent folder
					mFragFile = getAssetPath("") / aFileName;
					if (fs::exists(mFragFile)) {
						mShaderName = mShaderFileName = aFileName;
						mFileNameWithExtension = mFragFile.filename().string();
						mFragmentShaderString = loadString(loadFile(mFragFile));
						mValid = setFragmentString(mFragmentShaderString, mFragFile.filename().string());

						CI_LOG_V(mFragFile.string() + " loaded and compiled(parent)");
					}
					else {
						mError = mFragFile.string() + " does not exist";
						CI_LOG_V(mError);
						// load default fragment shader
						try {
							mShaderName = mShaderFileName = "default.fs";
							mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mVDSettings->getDefaultFragmentShaderString());
							mValid = true;
							CI_LOG_V("fbo default vtx-frag compiled");
						}
						catch (gl::GlslProgCompileExc &exc) {
							mError = string(exc.what());
							CI_LOG_V("fbo unable to load/compile vtx-frag shader:" + string(exc.what()));
						}
						catch (const std::exception &e) {
							mError = string(e.what());
							CI_LOG_V("fbo unable to load vtx-frag shader:" + string(e.what()));
						}
					}
				}*/
			}
		}
		else {
			mError = "aFileName empty";
		}
		if (mError.length() > 0) mVDSettings->mErrorMsg = mError + "\n" + mVDSettings->mErrorMsg.substr(0, mVDSettings->mMsgLength);
		return mValid;
	}
	// aName = fullpath
	bool VDFbo::setFragmentString(string aFragmentShaderString, string aName) {

		string mOriginalFragmentString = aFragmentShaderString;
		string fileName = "";
		mError = "";

		// we would like a name without extension
		if (aName.length() == 0) {
			aName = toString((int)getElapsedSeconds());
		}
		else {
			int dotIndex = aName.find_last_of(".");
			int slashIndex = aName.find_last_of("\\");

			if (dotIndex != std::string::npos && dotIndex > slashIndex) {
				aName = aName.substr(slashIndex + 1, dotIndex - slashIndex - 1);
			}
		}

		string mNotFoundmUniformsString = "/* " + aName + "\n";
		// filename to save
		mValid = false;
		// load fragment shader
		CI_LOG_V("setFragmentString, loading" + aName);
		try
		{
			// before compilation save .fs file to inspect errors
			fileName = aName + ".fs";
			fs::path receivedFile = getAssetPath("") / "glsl" / "received" / fileName;
			ofstream mFragReceived(receivedFile.string(), std::ofstream::binary);
			mFragReceived << aFragmentShaderString;
			mFragReceived.close();
			CI_LOG_V("file saved:" + receivedFile.string());			
			
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no mUniforms found, we add from shadertoy.inc");
				aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
			}
			else {
				aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
			}



			// try to compile a first time to get active mUniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
			// update only if success
			mFragmentShaderString = aFragmentShaderString;
			
			mVDSettings->mMsg = aName + " loaded and compiled\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

			// name of the shader
			mShaderName = aName;
			mValid = true;

		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = aName + string(exc.what());
			CI_LOG_V("setFragmentString, unable to compile live fragment shader:" + mError + " frag:" + aName);
		}
		catch (const std::exception &e)
		{
			mError = aName + string(e.what());
			CI_LOG_V("setFragmentString, error on live fragment shader:" + mError + " frag:" + aName);
		}
		//mVDSettings->mNewMsg = true;
		mVDSettings->mErrorMsg += "\n" + mError;
		return mValid;
	}

	ci::gl::Texture2dRef VDFbo::getFboTexture() {

		if (mValid) {

			gl::ScopedFramebuffer fbScp(mFbo);
			//gl::clear(Color::black());
			gl::clear(ColorA(0.0f, 0.4f, 0.8f, 0.3f));
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
				if (mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBARBEAT) > 19) {
					// TODO IBARBEAT 
					mCurrentSeqFilename = mTextureName + " (" + toString(mVDAnimation->getIntUniformValueByIndex(mVDSettings->IBARBEAT)) + ").jpg";
				}
				if (mCachedTextures[mCurrentSeqFilename]) {
					//CI_LOG_V(mCurrentSeqFilename + " in cache");
					mLastCachedFilename = mCurrentSeqFilename;
					mTexture = mCachedTextures[mCurrentSeqFilename];
				}
				else {
					// mTextureName is the path
					fs::path fullPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName / mCurrentSeqFilename;
					if (fs::exists(fullPath)) {
						// start profiling
						auto start = Clock::now();
						mCachedTextures[mCurrentSeqFilename] = ci::gl::Texture::create(loadImage(fullPath), gl::Texture::Format().loadTopDown());
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
			if (mTexture) mTexture->bind(0);
			string name;

			mUniforms = mShader->getActiveUniforms();
			for (const auto &uniform : mUniforms) {
				name = uniform.getName();
				//CI_LOG_V(mShader->getLabel() + ", getShader uniform name:" + uniform.getName() + ", type:" + toString( uniform.getType() ));
				if (mVDAnimation->isExistingUniform(name)) {
					int uniformType = mVDAnimation->getUniformType(name);
					switch (uniformType)
					{
					case 0: // float
						createFloatUniform(name, mVDAnimation->getUniformIndexForName(name), getIntUniformValueByName(name), mVDAnimation->getMinUniformValueByName(name), mVDAnimation->getMaxUniformValueByName(name));
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
						if (name == "TIME") {
							// globally
							mShader->uniform(name, mVDAnimation->getFloatUniformValueByName("iTime"));
						}
						break;
					case 1: // sampler2D
						mShader->uniform(name, 0);
						break;
					case 2: // vec2
						if (name == "RENDERSIZE") {
							//mShader->uniform(name, vec2(mTexture->getWidth(), mTexture->getHeight()));
							mShader->uniform(name, vec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight));
						}
						else {
							mShader->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
						}
						break;
					case 3: // vec3
						mShader->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
						break;
					case 4: // vec4
						mShader->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
						break;
					case 5: // int
						// IBEAT 51
						// IBAR 52
						// IBARBEAT 53
						createIntUniform(name, mVDAnimation->getUniformIndexForName(name), getIntUniformValueByName(name));
						mShader->uniform(name, mVDAnimation->getIntUniformValueByName(name));
						break;
					case 6: // bool
						//IFLIPH 101
						//IFLIPV 102
						createBoolUniform(name, mVDAnimation->getUniformIndexForName(name), getBoolUniformValueByName(name)); // get same index as vdanimation
						mShader->uniform(name, getBoolUniformValueByName(name));
						if (name == "iDebug") {
							// globally
							mShader->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
						}
						break;
					default:
						break;
					}
				}
				else {
					if (name != "ciModelViewProjection") {//type 35676
						//mVDSettings->mNewMsg = true;
						mError = "uniform not found " + name;
						mVDSettings->mErrorMsg = mError + "\n" + mVDSettings->mErrorMsg.substr(0, mVDSettings->mMsgLength);
						CI_LOG_E(mError);
					}
				}
			}

			/*if (!isReady) {
				mShader->uniform("RENDERSIZE", vec2(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight));
			}
			else {
				mShader->uniform("RENDERSIZE", vec2(mTexture->getWidth(), mTexture->getHeight()));
			}*/
			mShader->uniform("RENDERSIZE", vec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight));
			mShader->uniform("TIME", (float)getElapsedSeconds());// mVDAnimation->getFloatUniformValueByIndex(0));

			gl::ScopedGlslProg glslScope(mShader);
			// TODO: test gl::ScopedViewport sVp(0, 0, mFbo->getWidth(), mFbo->getHeight());	
			// for thumb TODO 202020222 create thumb with correct size
			/*if (!isReady) {
				gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight));
			}
			else {
				gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
			}*/
			gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
			mRenderedTexture = mFbo->getColorTexture();
			if (!isReady) {
				string filename = mShaderName + "-" + mTextureName + ".jpg";
				fs::path fr = getAssetPath("") / "thumbs" / filename;

				if (!fs::exists(fr)) {
					CI_LOG_V(fr.string() + " does not exist, creating");
					Surface s8(mRenderedTexture->createSource());
					writeImage(writeFile(fr), s8);
				}
			}
		}
		return mRenderedTexture;
	}
	ci::gl::Texture2dRef VDFbo::getRenderedTexture() {
		if (mValid) {
			if (!isReady) {
				// render once for thumb
				getFboTexture();
				isReady = true;
			}
			getFboTexture();
		}
		return mRenderedTexture;
	}
	//! to json
	JsonTree	VDFbo::toJson(bool save) const
	{
		JsonTree		json;
		JsonTree shader = ci::JsonTree::makeArray("shader");
		shader.addChild(ci::JsonTree("shadername", mShaderFileName));
		shader.pushBack(ci::JsonTree("shadertype", "fs"));
		json.addChild(shader);
		JsonTree texture = ci::JsonTree::makeArray("texture");
		texture.addChild(ci::JsonTree("texturename", mTextureName));
		texture.pushBack(ci::JsonTree("texturetype", "image"));
		json.addChild(texture);
		/*json.addChild(ci::JsonTree("shadername", mShaderName));
		json.addChild(ci::JsonTree("shadertype", "fs"));
		json.addChild(ci::JsonTree("texturename", mTextureName));*/
		if (save) {
			string jsonFileName = mShaderName + ".json";
			fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
			json.write(jsonFile);
		}
		return json;
	}
}