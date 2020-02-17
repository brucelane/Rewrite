#include "VDWarp.h"

namespace videodromm {
	VDWarp::VDWarp(VDSettingsRef aVDSettings)
		:mValid(false)
	{
		CI_LOG_V("VDWarp constructor");
		shaderInclude = "#version 150\n"
			"// shadertoy specific\n"
			"uniform vec3      	RENDERSIZE;\n"
			"uniform vec3 	iResolution;\n"
			"uniform float     	TIME;\n"
			"uniform vec4      	iMouse;\n"
			"uniform bool       iFlipH;\n"
			"uniform bool       iFlipV;\n"

			"uniform sampler2D 	inputImage;\n"
			"out vec4 fragColor;\n"
			"#define IMG_NORM_PIXEL texture2D\n";
		//"#define RENDERSIZE iResolution\n";
		//"vec2  fragCoord = gl_FragCoord.xy; // keep the 2 spaces between vec2 and fragCoord\n";
		mVDSettings = aVDSettings;
		mUseBeginEnd = false;
		isReady = false;
		mInputTextureIndex = 0;
		mSrcArea = Area(0, 0, 10, 10);
		mVDAnimation = VDAnimation::create(mVDSettings);
		fs::path texPath = getAssetPath("") / mVDSettings->mAssetsPath / "0.jpg";
		if (fs::exists(texPath)) {
			mTexture = gl::Texture::create(loadImage(texPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
			mRenderedTexture = gl::Texture::create(loadImage(texPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
		}
		else {
			fs::path helpPath = getAssetPath("") / mVDSettings->mAssetsPath / "help.jpg";
			if (fs::exists(helpPath)) {
				mTexture = gl::Texture::create(loadImage(helpPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
				mRenderedTexture = gl::Texture::create(loadImage(helpPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
			}
			else {
				mTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
				mRenderedTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
			}
		}

		mSrcArea = mTexture->getBounds();
		// init texture
		// init the fbo whatever happens next
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		//mThumbFbo = gl::Fbo::create(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight, fboFmt);
		mError = "";
		// init with passthru shader
		//mShaderName = "0";
		mFboName = "default";
		//mValid = false;
		mActive = true;

		mValid = loadFragmentStringFromFile("inputImage.fs");

		if (mValid) {
			CI_LOG_V("VDShaders constructor success");
		}
		else {
			CI_LOG_V("VDShaders constructor failed, do not use");
		}
	}
	VDWarp::~VDWarp(void) {
	}

	bool VDWarp::loadFragmentStringFromFile(string aFileName) {
		mValid = false;
		// load fragment shader
		CI_LOG_V("loadFragmentStringFromFile, loading " + aFileName);
		mFragFile = getAssetPath("") / mVDSettings->mAssetsPath / aFileName;
		if (aFileName.length() > 0 && fs::exists(mFragFile)) {
			mFileNameWithExtension = mFragFile.filename().string();
			mFragmentShaderString = loadString(loadFile(mFragFile));
			mValid = setFragmentString(mFragmentShaderString, mFragFile.filename().string());

			CI_LOG_V(mFragFile.string() + " loaded and compiled");
		}
		else {
			mError = mFragFile.string() + " does not exist";
			CI_LOG_V(mError);
			// load default fragment shader
			try {
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
		if (mError.length() > 0) mVDSettings->mMsg = mError;
		return mValid;
	}
	// aName = fullpath
	bool VDWarp::setFragmentString(string aFragmentShaderString, string aName) {

		string mOriginalFragmentString = aFragmentShaderString;
		string fileName = "";
		string mCurrentUniformsString = "// active uniforms start\n";
		string mProcessedShaderString = "";
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

		string mNotFoundUniformsString = "/* " + aName + "\n";
		// filename to save
		mValid = false;
		// load fragment shader
		CI_LOG_V("setFragmentString, loading" + aName);
		try
		{
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no uniforms found, we add from shadertoy.inc");
				aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
			}
			else {
				aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
			}

			// before compilation save .frag file to inspect errors
			fileName = aName + ".fs";
			fs::path receivedFile = getAssetPath("") / "glsl" / "received" / fileName;
			ofstream mFragReceived(receivedFile.string(), std::ofstream::binary);
			mFragReceived << aFragmentShaderString;
			mFragReceived.close();
			CI_LOG_V("file saved:" + receivedFile.string());

			// try to compile a first time to get active uniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
			// update only if success
			mFragmentShaderString = aFragmentShaderString;
			mVDSettings->mMsg = aName + " loaded and compiled";
			// name of the shader
			mName = aName;
			mValid = true;

			auto &uniforms = mShader->getActiveUniforms();
			string uniformName;
			for (const auto &uniform : uniforms) {
				uniformName = uniform.getName();
				CI_LOG_V(aName + ", uniform name:" + uniformName);
				// if uniform is handled
				if (mVDAnimation->isExistingUniform(uniformName)) {
					int uniformType = mVDAnimation->getUniformType(uniformName);
					switch (uniformType)
					{
					case 0:
						// float
						mShader->uniform(uniformName, mVDAnimation->getFloatUniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform float " + uniformName + "; // " + toString(mVDAnimation->getFloatUniformValueByName(uniformName)) + "\n";
						break;
					case 1:
						// sampler2D
						mShader->uniform(uniformName, mVDAnimation->getSampler2DUniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform sampler2D " + uniformName + "; // " + toString(mVDAnimation->getSampler2DUniformValueByName(uniformName)) + "\n";
						break;
					case 2:
						// vec2
						mShader->uniform(uniformName, mVDAnimation->getVec2UniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform vec2 " + uniformName + "; // " + toString(mVDAnimation->getVec2UniformValueByName(uniformName)) + "\n";
						break;
					case 3:
						// vec3
						mShader->uniform(uniformName, mVDAnimation->getVec3UniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform vec3 " + uniformName + "; // " + toString(mVDAnimation->getVec3UniformValueByName(uniformName)) + "\n";
						break;
					case 4:
						// vec4
						mShader->uniform(uniformName, mVDAnimation->getVec4UniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform vec4 " + uniformName + "; // " + toString(mVDAnimation->getVec4UniformValueByName(uniformName)) + "\n";
						break;
					case 5:
						// int
						mShader->uniform(uniformName, mVDAnimation->getIntUniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform int " + uniformName + "; // " + toString(mVDAnimation->getIntUniformValueByName(uniformName)) + "\n";
						break;
					case 6:
						// bool
						mShader->uniform(uniformName, mVDAnimation->getBoolUniformValueByName(uniformName));
						//mCurrentUniformsString += "uniform bool " + uniformName + "; // " + toString(mVDAnimation->getBoolUniformValueByName(uniformName)) + "\n";
						break;
					default:
						break;
					}
				}
				else {
					if (uniformName != "ciModelViewProjection") {
						mNotFoundUniformsString += "not found " + uniformName + "\n";
					}
				}
			}



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
		mVDSettings->mNewMsg = true;
		mVDSettings->mMsg = mError;
		return mValid;
	}

	gl::GlslProgRef VDWarp::getShader() {
		/*int index = 300;
		int texIndex = 0;
		string name;
		auto &uniforms = mShader->getActiveUniforms();
		for (const auto &uniform : uniforms) {
			name = uniform.getName();
			//CI_LOG_V(mShader->getLabel() + ", getShader uniform name:" + uniform.getName());
			if (mVDAnimation->isExistingUniform(name)) {
				int uniformType = mVDAnimation->getUniformType(name);
				switch (uniformType)
				{
				case 0:
					// float
					mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
					break;
				case 1:
					// sampler2D
					mShader->uniform(name, mInputTextureIndex);// cinder::gl::GlslProg::logUniformWrongType[1021] Uniform type mismatch for "iChannel0", expected SAMPLER_2D and received uint32_t
					break;
				case 2:
					// vec2
					mShader->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
					break;
				case 3:
					// vec3
					mShader->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
					break;
				case 4:
					// vec4
					mShader->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
					break;
				case 5:
					// int
					mShader->uniform(name, mVDAnimation->getIntUniformValueByName(name));
					break;
				case 6:
					// bool
					mShader->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
					break;
				default:
					break;
				}
			}
			else {
				if (name != "ciModelViewProjection") {
					mVDSettings->mMsg = mFboName + ", uniform not found:" + name + " type:" + toString(uniform.getType());
					CI_LOG_V(mVDSettings->mMsg);
					int firstDigit = name.find_first_of("0123456789");
					// if valid image sequence (contains a digit)
					if (firstDigit > -1) {
						index = std::stoi(name.substr(firstDigit));
					}
					switch (uniform.getType())
					{
					case 5126:
						mVDAnimation->createFloatUniform(name, 400 + index, 0.31f, 0.0f, 1000.0f);
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
						break;
					case 35664:
						//mVDAnimation->createvec2(uniform.getName(), 310 + , 0);
						//++;
						break;
					case 35678:
						mVDAnimation->createSampler2DUniform(uniform.getName(), 310 + texIndex, 0);
						texIndex++;
						break;
					default:
						break;
					}
				}
			}
		}*/
		return mShader;
	}

	ci::gl::Texture2dRef VDWarp::getFboTexture() {
		// TODO move this:
		if (mValid) {
			mVDAnimation->update();
			//getShader();
			gl::ScopedFramebuffer fbScp(mFbo);
			gl::clear(Color::black());

			mTexture->bind(0);
			string name;
			auto &uniforms = mShader->getActiveUniforms();
			for (const auto &uniform : uniforms) {
				name = uniform.getName();
				//CI_LOG_V(mShader->getLabel() + ", getShader uniform name:" + uniform.getName());
				if (mVDAnimation->isExistingUniform(name)) {
					int uniformType = mVDAnimation->getUniformType(name);
					switch (uniformType)
					{
					case 0:
						// float
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
						break;
					case 1:
						// sampler2D
						mShader->uniform(name, mInputTextureIndex);
						break;
					case 2:
						// vec2
						mShader->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
						break;
					case 3:
						// vec3
						mShader->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
						break;
					case 4:
						// vec4
						mShader->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
						break;
					case 5:
						// int
						mShader->uniform(name, mVDAnimation->getIntUniformValueByName(name));
						break;
					case 6:
						// bool
						mShader->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
						break;
					default:
						break;
					}
				}
			}
			
			if (!isReady) {
				mShader->uniform("RENDERSIZE", vec3(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight, 1.0));
			}
			else {
				mShader->uniform("RENDERSIZE", vec3(mTexture->getWidth(), mTexture->getHeight(), 1.0));
			}
			mShader->uniform("TIME", (float)getElapsedSeconds());// mVDAnimation->getFloatUniformValueByIndex(0));
			//mShader->uniform("iFlipH", mFlipH);
			//mShader->uniform("iFlipV", mFlipV);
			gl::ScopedGlslProg glslScope(mShader);
			// TODO: test gl::ScopedViewport sVp(0, 0, mFbo->getWidth(), mFbo->getHeight());

			//gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight));
			gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
			mRenderedTexture = mFbo->getColorTexture();
			if (!isReady) {
				string filename = mFboName + ".jpg";
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
	ci::gl::Texture2dRef VDWarp::getRenderedTexture() {
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
}