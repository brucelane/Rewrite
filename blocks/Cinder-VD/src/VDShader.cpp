#include "VDShader.h"

using namespace videodromm;
/*

	test exists at assets path or full path
	get isf string
	compile
	run
	create thumb using fbo
	save isf in assets session subfolder
was VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aFragmentShaderString) {
*/
VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const string& aFileOrPath, const string& aShaderFragmentString, gl::TextureRef aTexture) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mFragmentShaderString = aShaderFragmentString;
	mTexture = aTexture;
	mValid = false;
	mError = "";
	bool fileExists = true;
	// shadertoy include
	shaderInclude = loadString(loadAsset("shadertoy.vd"));
	mFragFilePath = getAssetPath("") / mVDSettings->mAssetsPath / aFileOrPath;
	// priority to string 
	if (mFragmentShaderString.length() > 0) {
		mFileNameWithExtension = mFragFilePath.filename().string();
		mValid = setFragmentString(mFragmentShaderString, mFileNameWithExtension);
	}
	else {
		if (aFileOrPath.length() > 0) {
			if (fs::exists(aFileOrPath)) {
				// it's a full path
				mFragFilePath = aFileOrPath;
			}
			else {
				// try in assets folder			
				if (!fs::exists(mFragFilePath)) {
					mFragFilePath = getAssetPath("") / aFileOrPath;
					if (!fs::exists(mFragFilePath)) {
						fileExists = false;
						mError = "shader file does not exist in assets root or current subfolder:" + aFileOrPath;
					}
				}

			}
		}
		else {
			mError = "shader file empty";
		}
		if (fileExists) {
			// file exists
			if (loadFragmentStringFromFile()) {
				fboFmt.setColorTextureFormat(fmt);
				mRenderedTexture = ci::gl::Texture::create(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, ci::gl::Texture::Format().loadTopDown());
				mThumbFbo = gl::Fbo::create(mVDSettings->mPreviewWidth, mVDSettings->mPreviewHeight, fboFmt);
				getThumbTexture();
			}
			else {
				mError = "could not compile:" + aFileOrPath;
			}

		}
	}

	mVDSettings->mErrorMsg = mError + "\n" + mVDSettings->mErrorMsg.substr(0, mVDSettings->mMsgLength);
	CI_LOG_V("VDShaders constructor " + mError);

}

bool VDShader::loadFragmentStringFromFile() {
	mValid = false;
	// load fragment shader
	mFileNameWithExtension = mFragFilePath.filename().string();
	CI_LOG_V("loadFragmentStringFromFile, loading " + mFileNameWithExtension);
	//mName = mFragFile.filename().string();
	// get filename without extension
	//int dotIndex = fileName.find_last_of(".");
	//
	//if (dotIndex != std::string::npos) {
	//	mName = fileName.substr(0, dotIndex);
	//}
	//else {
	//	mName = fileName;
	//}

	mFragmentShaderString = loadString(loadFile(mFragFilePath));
	mValid = setFragmentString(mFragmentShaderString, mFileNameWithExtension);

	CI_LOG_V(mFragFilePath.string() + " loaded and compiled");
	return mValid;
}// aName = fullpath
bool VDShader::setFragmentString(const string& aFragmentShaderString, const string& aName) {

	string mOriginalFragmentString = aFragmentShaderString;
	string mOutputFragmentString = aFragmentShaderString;
	string mISFString = aFragmentShaderString;
	string mOFISFString = "";
	mName = aName;
	//string fileName = "";
	string mCurrentUniformsString = "// active uniforms start\n";
	string mProcessedShaderString = "";
	ext = "";
	mError = "";

	// we would like a name without extension
	if (mName.length() == 0) {
		mName = toString((int)getElapsedSeconds()); // + ".frag" 
	}
	else {
		int dotIndex = mName.find_last_of(".");
		int slashIndex = mName.find_last_of("\\");

		if (dotIndex != std::string::npos && dotIndex > slashIndex) {
			ext = mName.substr(dotIndex + 1);
			mName = mName.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		}

	}
	
	string mNotFoundUniformsString = "/* " + mName + "\n";
	// filename to save
	mValid = false;
	// load fragment shader
	CI_LOG_V("setFragmentString, loading" + mName);
	try
	{
		if (ext == "fs")
		{
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no mUniforms found, we add from shadertoy.vd");
				mOutputFragmentString = "/* " + mName + " */\n" + shaderInclude + mOriginalFragmentString;
			}
			else {
				mOutputFragmentString = "/* " + mName + " */\n" + mOriginalFragmentString;
			}
			// try to compile a first time to get active uniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mOutputFragmentString);
			// update only if success
			mFragmentShaderString = mOutputFragmentString;
			mValid = true;
			mVDSettings->mMsg = mName + " compiled(fs)\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

		}
		else {
			// from Bonzomatic
			std::regex pattern{ "fGlobalTime" };
			std::string replacement{ "TIME" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "uniform" };
			replacement = { "//" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "v2Resolution" };
			replacement = { "RENDERSIZE" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "#version 410 core" };
			replacement = { "// from Bonzomatic" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "layout" };// ( don't work
			replacement = { "// from Bonzomatic 1" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "out_color" };
			replacement = { "fragColor" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			// save in assetspath
			/*fileName = aName + ".fs";
			fs::path fsFile = getAssetPath("") / mVDSettings->mAssetsPath / fileName;

			ofstream mFS(fsFile.string(), std::ofstream::binary);
			mFS << mOriginalFragmentString;
			mFS.close();
			CI_LOG_V("ISF file saved:" + fsFile.string());*/
			// from Hydra
			pattern = { "time" };
			replacement = { "TIME" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "uniform vec2 resolution;" };
			replacement = { "uniform vec3 iResolution ;" }; //keep the space
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "resolution" };
			replacement = { "iResolution" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "varying vec2 uv;" };
			replacement = { "// from Hydra" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			/*uniform float time;
			uniform vec2 resolution;
			varying vec2 uv;*/

			//CI_LOG_V("before regex " + mOriginalFragmentString);
					// shadertoy: 
			// change void mainImage( out vec4 fragColor, in vec2 fragCoord ) to void main(void)
			pattern = { "mainImage" };
			replacement = { "main" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "out vec4 fragColor," };
			replacement = { "void" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "in vec2 fragCoord" };
			replacement = { "" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { " vec2 fragCoord" };
			replacement = { "" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			// html glslEditor:
			// change vec2 u_resolution to vec3 iResolution
			pattern = { "2 u_r" };
			replacement = { "3 iR" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "u_r" };
			replacement = { "iR" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "u_tex" };
			replacement = { "iChannel" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "2 u_mouse" };
			replacement = { "4 iMouse" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "u_m" };
			replacement = { "iM" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "u_time" };
			replacement = { "TIME" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "u_" };
			replacement = { "i" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "gl_TexCoord[0].st" };
			replacement = { "gl_FragCoord.xy/iResolution.xy" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "iAudio0" };
			replacement = { "iChannel0" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			// 20190727 TODO CHECK
			/*pattern = { "iFreq0" };
			replacement = { "iChannel0.x" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "iFreq1" };
			replacement = { "iChannel0.y" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "iFreq2" };
			replacement = { "iChannel0.x" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement); */
			pattern = { "iRenderXY.x" };
			replacement = { "0.0" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			pattern = { "iRenderXY.y" };
			replacement = { "0.0" };
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);
			// ISF file format
			mISFString = mOriginalFragmentString;
			std::regex ISFPattern{ "iResolution" };
			std::string ISFReplacement{ "RENDERSIZE" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			ISFPattern = { "texture2D" };
			//ISFReplacement = { "IMG_THIS_PIXEL" };
			ISFReplacement = { "IMG_NORM_PIXEL" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "texture" };
			//ISFReplacement = { "IMG_THIS_PIXEL" }; // CHECK
			ISFReplacement = { "IMG_NORM_PIXEL" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			/* 20200312
			ISFPattern = { "iChannel0" };
			ISFReplacement = { "inputImage" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			*/

			mOFISFString = mISFString;

			ISFPattern = { "out vec4 fragColor," };
			ISFReplacement = { "void" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "in vec2 fragCoord" };
			ISFReplacement = { "" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			// 20200228
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "gl_FragColor" };
			ISFReplacement = { "fragColor" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			// check if uniforms were declared in the file
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no uniforms found, we add from shadertoy.vd");
				//aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
				mOutputFragmentString = "/* " + mName + " */\n" + shaderInclude + mISFString;
			}
			else {
				//aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
				mOutputFragmentString = "/* " + mName + " */\n" + mISFString;
			}

			// try to compile a first time to get active uniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), mOutputFragmentString);
			// update only if success
			mFragmentShaderString = mOutputFragmentString;
			mVDSettings->mMsg = mName + " compiled(shader)\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

			// name of the shader
			//mName = aName;
			mValid = true;
			string mISFUniforms = ",\n"
				"		{\n"
				"			\"NAME\": \"iColor\", \n"
				"			\"TYPE\" : \"color\", \n"
				"			\"DEFAULT\" : [\n"
				"				0.9, \n"
				"				0.6, \n"
				"				0.0, \n"
				"				1.0\n"
				"			]\n"
				"		}\n";
			auto &uniforms = mShader->getActiveUniforms();
			string uniformnName;
			for (const auto &uniform : uniforms) {
				uniformnName = uniform.getName();
				CI_LOG_V(mName + ", uniform name:" + uniformnName);
				// if uniform is handled
				if (mVDAnimation->isExistingUniform(uniformnName)) {
					int uniformType = mVDAnimation->getUniformType(uniformnName);
					switch (uniformType)
					{
					case 0:
						// float
						mShader->uniform(uniformnName, mVDAnimation->getFloatUniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform float " + uniformnName + "; // " + toString(mVDAnimation->getFloatUniformValueByName(uniformnName)) + "\n";
						if (uniformnName != "TIME") {
							mISFUniforms += ",\n"
								"		{\n"
								"			\"NAME\": \"" + uniformnName + "\", \n"
								"			\"TYPE\" : \"float\", \n"
								"			\"MIN\" : " + toString(mVDAnimation->getMinUniformValueByName(uniformnName)) + ",\n"
								"			\"MAX\" : " + toString(mVDAnimation->getMaxUniformValueByName(uniformnName)) + ",\n"
								"			\"DEFAULT\" : " + toString(mVDAnimation->getFloatUniformValueByName(uniformnName)) + "\n"
								"		}\n";
						}
						break;
					case 1:
						// sampler2D
						mShader->uniform(uniformnName, mVDAnimation->getSampler2DUniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform sampler2D " + uniformnName + "; // " + toString(mVDAnimation->getSampler2DUniformValueByName(uniformnName)) + "\n";
						break;
					case 2:
						// vec2
						mShader->uniform(uniformnName, mVDAnimation->getVec2UniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform vec2 " + uniformnName + "; // " + toString(mVDAnimation->getVec2UniformValueByName(uniformnName)) + "\n";
						break;
					case 3:
						// vec3
						mShader->uniform(uniformnName, mVDAnimation->getVec3UniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform vec3 " + uniformnName + "; // " + toString(mVDAnimation->getVec3UniformValueByName(uniformnName)) + "\n";
						break;
					case 4:
						// vec4
						mShader->uniform(uniformnName, mVDAnimation->getVec4UniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform vec4 " + uniformnName + "; // " + toString(mVDAnimation->getVec4UniformValueByName(uniformnName)) + "\n";
						break;
					case 5:
						// int
						mShader->uniform(uniformnName, mVDAnimation->getIntUniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform int " + uniformnName + "; // " + toString(mVDAnimation->getIntUniformValueByName(uniformnName)) + "\n";
						break;
					case 6:
						// bool
						mShader->uniform(uniformnName, mVDAnimation->getBoolUniformValueByName(uniformnName));
						mCurrentUniformsString += "uniform bool " + uniformnName + "; // " + toString(mVDAnimation->getBoolUniformValueByName(uniformnName)) + "\n";
						break;
					default:
						break;
					}
				}
				else {
					if (uniformnName != "ciModelViewProjection") {
						mNotFoundUniformsString += "not found " + uniformnName + "\n";
					}
				}
			}

			// save ISF
			string mISFHeader = "/*{\n"
				"	\"CREDIT\" : \"" + mName + " by \",\n"
				"	\"CATEGORIES\" : [\n"
				"		\"ci\"\n"
				"	],\n"
				"	\"DESCRIPTION\": \"\",\n"
				"	\"INPUTS\": [\n"
				"		{\n"
				"			\"NAME\": \"inputImage\",\n"
				"			\"TYPE\" : \"image\"\n"
				"		},\n"
				/*"		{\n"
				"			\"NAME\": \"iZoom\",\n"
				"			\"TYPE\" : \"float\",\n"
				"			\"MIN\" : 0.0,\n"
				"			\"MAX\" : 1.0,\n"
				"			\"DEFAULT\" : 1.0\n"
				"		},\n"*/
				"		{\n"
				"			\"NAME\": \"iSteps\",\n"
				"			\"TYPE\" : \"float\",\n"
				"			\"MIN\" : 2.0,\n"
				"			\"MAX\" : 75.0,\n"
				"			\"DEFAULT\" : 19.0\n"
				"		},\n"
				"		{\n"
				"			\"NAME\" :\"iMouse\",\n"
				"			\"TYPE\" : \"point2D\",\n"
				"			\"DEFAULT\" : [0.0, 0.0],\n"
				"			\"MAX\" : [640.0, 480.0],\n"
				"			\"MIN\" : [0.0, 0.0]\n"
				"		}\n";


			string mISFFooter = "	],\n"
				"}\n"
				"*/\n";


			mISFString = mISFHeader + mISFUniforms + mISFFooter + mISFString;
			mOFISFString = mISFHeader + mOFISFString;

			// ifs for openFrameworks ISFGif project
			//fileName = aName + ".fs";
			mFileNameWithExtension = mName + ".fs";
			mFragFilePath = getAssetPath("") / mVDSettings->mAssetsPath / mFileNameWithExtension;
			/*fs::path OFIsfFile = getAssetPath("") / "glsl" / "osf" / mFileNameWithExtension;
			ofstream mOFISF(OFIsfFile.string(), std::ofstream::binary);
			mOFISF << mOFISFString;
			mOFISF.close();
			CI_LOG_V("OF ISF file saved:" + OFIsfFile.string());*/

			// ifs for HeavyM, save in assetspath if not exists already
			//fileName = aName + ".fs";
			//fs::path isfFile = getAssetPath("") / "glsl" / "isf" / fileName;
			//fs::path isfFile = getAssetPath("") / mVDSettings->mAssetsPath / mFileNameWithExtension;
			/*if (fs::exists(isfFile)) {
				isfFile = getAssetPath("") / "glsl" / "isf" / fileName;
				ofstream mISF(isfFile.string(), std::ofstream::binary);
				mISF << mISFString;
				mISF.close();
				CI_LOG_V("ISF file saved:" + isfFile.string());
			}
			else {*/
			ofstream mISF(mFragFilePath.string(), std::ofstream::binary);
			mISF << mISFString;
			mISF.close();
			CI_LOG_V("ISF file saved:" + mFragFilePath.string());
			//}
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = mName + string(exc.what());
		CI_LOG_V("setFragmentString, unable to compile live fragment shader:" + mError + " frag:" + mOutputFragmentString);
	}
	catch (const std::exception &e)
	{
		mError = mName + string(e.what());
		CI_LOG_V("setFragmentString, error on live fragment shader:" + mError + " frag:" + mOutputFragmentString);
	}
	if (mError.length() > 0) mVDSettings->mShaderMsg = mError + "\n" + mVDSettings->mShaderMsg.substr(0, mVDSettings->mMsgLength);
	return mValid;
}
ci::gl::Texture2dRef VDShader::getFboTexture() {

	if (mValid) {

		gl::ScopedFramebuffer fbScp(mThumbFbo);
		gl::clear(Color::black());

		if (mTexture) mTexture->bind(254);
		string name;

		mUniforms = mShader->getActiveUniforms();
		for (const auto &uniform : mUniforms) {
			name = uniform.getName();
			if (mVDAnimation->isExistingUniform(name)) {
				int uniformType = mVDAnimation->getUniformType(name);
				switch (uniformType)
				{
				case 0: // float
					mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
					if (name == "TIME") {
						// globally
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName("TIME"));
					}
					break;
				case 1: // sampler2D
					mShader->uniform(name, 254);
					break;
				case 2: // vec2
					if (name == "RENDERSIZE") {
						mShader->uniform(name, vec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
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
					//mVDSettings->mNewMsg = true;
					mError = "uniform not found " + name;
					mVDSettings->mErrorMsg = mError + "\n" + mVDSettings->mErrorMsg.substr(0, mVDSettings->mMsgLength);
					CI_LOG_E(mError);
				}
			}
		}
		mShader->uniform("RENDERSIZE", vec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		mShader->uniform("TIME", (float)getElapsedSeconds());// mVDAnimation->getFloatUniformValueByIndex(0));

		gl::ScopedGlslProg glslScope(mShader);
		// TODO: test gl::ScopedViewport sVp(0, 0, mFbo->getWidth(), mFbo->getHeight());	

		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
		mRenderedTexture = mThumbFbo->getColorTexture();

		string filename = mName + ".jpg";
		fs::path fr = getAssetPath("") / "thumbs" / filename;

		//if (!fs::exists(fr)) {
		CI_LOG_V(fr.string() + " does not exist, creating");
		Surface s8(mRenderedTexture->createSource());
		writeImage(writeFile(fr), s8);
		//}

	}
	return mRenderedTexture;
}
ci::gl::Texture2dRef VDShader::getThumbTexture() {
	if (mValid) {
		getFboTexture();
	}
	return mRenderedTexture;
}

#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
