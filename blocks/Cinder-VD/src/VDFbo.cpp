#include "VDFbo.h"

namespace videodromm {
	VDFbo::VDFbo(VDSettingsRef aVDSettings, string aShaderFilename, string aTextureFilename)
		:mValid(false)
	{
		CI_LOG_V("VDFbo constructor");
		mShaderName = aShaderFilename;
		mTextureName = aTextureFilename;
		shaderInclude = "#version 150\n"
			"// shadertoy specific\n"
			"uniform vec2      	RENDERSIZE;\n"
			"uniform vec3 		iResolution;\n"
			"uniform float     	TIME;\n"
			"uniform float     	iZoom;\n"
			"uniform vec4      	iMouse;\n"
			"uniform bool       iFlipH;\n"
			"uniform bool       iFlipV;\n"

			"uniform sampler2D 	inputImage;\n"
			"out vec4 fragColor;\n"
			"#define IMG_NORM_PIXEL texture2D\n";

		mVDSettings = aVDSettings;
		mUseBeginEnd = false;
		isReady = false;
		mInputTextureIndex = 0;
		mSrcArea = Area(0, 0, 10, 10);
		mVDAnimation = VDAnimation::create(mVDSettings);

		if (mTextureName == "") { mTextureName = "help.jpg"; }
		fs::path texPath = getAssetPath("") / mVDSettings->mAssetsPath / mTextureName;
		if (fs::exists(texPath)) {
			mTexture = gl::Texture::create(loadImage(texPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
			mRenderedTexture = gl::Texture::create(loadImage(texPath), gl::Texture2d::Format().loadTopDown().mipmap(true).minFilter(GL_LINEAR_MIPMAP_LINEAR));
		}
		else {
			mTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
			mRenderedTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
		}

		mSrcArea = mTexture->getBounds();
		// init texture
		// init the fbo whatever happens next
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFmt);
		//mThumbFbo = gl::Fbo::create(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight, fboFmt);
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
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no mUniforms found, we add from shadertoy.inc");
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

			// try to compile a first time to get active mUniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
			// update only if success
			mFragmentShaderString = aFragmentShaderString;
			mVDSettings->mMsg = aName + " loaded and compiled";
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
		mVDSettings->mNewMsg = true;
		mVDSettings->mMsg = mError;
		return mValid;
	}

	ci::gl::Texture2dRef VDFbo::getFboTexture() {

		if (mValid) {

			gl::ScopedFramebuffer fbScp(mFbo);
			gl::clear(Color::black());

			mTexture->bind(0);
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
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
						break;
					case 1: // sampler2D
						mShader->uniform(name, mInputTextureIndex);
						break;
					case 2: // vec2
						mShader->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
						break;
					case 3: // vec3
						mShader->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
						break;
					case 4: // vec4
						mShader->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
						break;
					case 5: // int
						mShader->uniform(name, mVDAnimation->getIntUniformValueByName(name));
						break;
					case 6: // bool
						mShader->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
						break;
					default:
						break;
					}
				}
				else {
					if (name != "ciModelViewProjection") {//type 35676
						mVDSettings->mNewMsg = true;
						mError = "uniform not found " + name;
						mVDSettings->mMsg = mError;
						CI_LOG_E(mError);
					}
				}
			}

			if (!isReady) {
				mShader->uniform("RENDERSIZE", vec2(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight));
			}
			else {
				mShader->uniform("RENDERSIZE", vec2(mTexture->getWidth(), mTexture->getHeight()));
			}
			mShader->uniform("TIME", (float)getElapsedSeconds());// mVDAnimation->getFloatUniformValueByIndex(0));

			gl::ScopedGlslProg glslScope(mShader);
			// TODO: test gl::ScopedViewport sVp(0, 0, mFbo->getWidth(), mFbo->getHeight());			
			if (!isReady) {
				gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight));
			}
			else {
				gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
			}
			mRenderedTexture = mFbo->getColorTexture();
			if (!isReady) {
				string filename = mShaderName + ".jpg";
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
		json.addChild(ci::JsonTree("shadername", mShaderName));
		json.addChild(ci::JsonTree("shadertype", "fs"));
		json.addChild(ci::JsonTree("texturename", mTextureName));
		if (save) {
			string jsonFileName = mShaderName + ".json";
			fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
			json.write(jsonFile);
		}
		return json;
	}
}