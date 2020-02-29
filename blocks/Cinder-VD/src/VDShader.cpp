#include "VDShader.h"

using namespace videodromm;
/*

	test exists at assets path or full path
	get isf string
	compile
	run
	create thumb using fbo
	save isf in assets session subfolder
was VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, string aFragmentShaderString) {
*/
VDShader::VDShader(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, string aFileOrPath, gl::TextureRef aVDTexture) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mValid = false;
	mError = "";
	bool fileExists = true;
	if (aFileOrPath.length() > 0) {
		if (fs::exists(aFileOrPath)) {
			// it's a full path
			mFragFilePath = aFileOrPath;
		}
		else {
			// try in assets folder
			mFragFilePath = getAssetPath("") / mVDSettings->mAssetsPath / aFileOrPath;
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
		// shadertoy include
		shaderInclude = loadString(loadAsset("shadertoy.vd"));

		if (loadFragmentStringFromFile()) {
		}
		else {
			mError = "could not compile:" + aFileOrPath;
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
bool VDShader::setFragmentString(string aFragmentShaderString, string aName) {

	string mOriginalFragmentString = aFragmentShaderString;
	string mISFString = aFragmentShaderString;
	string mOFISFString = "";
	string fileName = "";
	string mCurrentUniformsString = "// active uniforms start\n";
	string mProcessedShaderString = "";
	ext = "";
	mError = "";

	// we would like a name without extension
	if (aName.length() == 0) {
		aName = toString((int)getElapsedSeconds()); // + ".frag" 
	}
	else {
		int dotIndex = aName.find_last_of(".");
		int slashIndex = aName.find_last_of("\\");

		if (dotIndex != std::string::npos && dotIndex > slashIndex) {
			ext = aName.substr(dotIndex + 1);
			aName = aName.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		}

	}

	string mNotFoundUniformsString = "/* " + aName + "\n";
	// filename to save
	mValid = false;
	// load fragment shader
	CI_LOG_V("setFragmentString, loading" + aName);
	if (ext == "fs")
	{
		std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
		if (foundUniform == std::string::npos) {
			CI_LOG_V("loadFragmentStringFromFile, no mUniforms found, we add from shadertoy.inc");
			aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
		}
		else {
			aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
		}
		// try to compile a first time to get active uniforms
		mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
		// update only if success
		mFragmentShaderString = aFragmentShaderString;
		mValid = true;
		mVDSettings->mMsg = aName + " loaded and compiled\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

	}
	else {


		try
		{
			// from Hydra
			std::regex pattern{ "time" };
			std::string replacement{ "iTime" };
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
			replacement = { "iTime" };
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
			// change void main(void) to void main(void) { mainImage(gl_FragColor, gl_FragCoord.xy); }
			mISFString = mOriginalFragmentString;
			std::regex ISFPattern{ "iResolution" };
			std::string ISFReplacement{ "RENDERSIZE" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "iTime" };
			ISFReplacement = { "TIME" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			ISFPattern = { "texture2D" };
			//ISFReplacement = { "IMG_THIS_PIXEL" };
			ISFReplacement = { "IMG_NORM_PIXEL" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "texture" };
			//ISFReplacement = { "IMG_THIS_PIXEL" }; // CHECK
			ISFReplacement = { "IMG_NORM_PIXEL" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			ISFPattern = { "iChannel0" };
			ISFReplacement = { "inputImage" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "iChannel1" };
			ISFReplacement = { "inputImage" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);

			mOFISFString = mISFString;

			//ISFPattern = { "void main" };
			//ISFReplacement = { "dirtyhack mainImage" }; //dirty hack!
			//mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "out vec4 fragColor," };
			ISFReplacement = { "void" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			ISFPattern = { "in vec2 fragCoord" };
			ISFReplacement = { "" };
			mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			//ISFPattern = { "dirtyhack mainImage" };
			//ISFReplacement = { "void mainImage" }; //dirty hack!
			//mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			//ISFPattern = { "gl_FragColor" };
			//ISFReplacement = { "fragColor" };
			//mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			//ISFPattern = { "gl_FragCoord" };
			//ISFReplacement = { "fragCoord" };
			//mISFString = std::regex_replace(mISFString, ISFPattern, ISFReplacement);
			//mISFString = mISFString + "void main(void) { mainImage(gl_FragColor, gl_FragCoord.xy); }";


			// shadertoy: 
			// change void mainImage( out vec4 fragColor, in vec2 fragCoord ) to void main(void)
			/*pattern = { "mainImage" };
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
			mOriginalFragmentString = std::regex_replace(mOriginalFragmentString, pattern, replacement);*/

			// change texture2D to texture for version > 150?	

			// change fragCoord to gl_FragCoord
			// change gl_FragColor to fragColor

			// check if uniforms were declared in the file
			std::size_t foundUniform = mOriginalFragmentString.find("uniform ");
			if (foundUniform == std::string::npos) {
				CI_LOG_V("loadFragmentStringFromFile, no uniforms found, we add from shadertoy.vd");
				//aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mOriginalFragmentString;
				aFragmentShaderString = "/* " + aName + " */\n" + shaderInclude + mISFString;
			}
			else {
				//aFragmentShaderString = "/* " + aName + " */\n" + mOriginalFragmentString;
				aFragmentShaderString = "/* " + aName + " */\n" + mISFString;
			}


			// before compilation save .frag file to inspect errors
			/*fileName = aName + ".frag";
			fs::path receivedFile = getAssetPath("") / "glsl" / "received" / fileName;
			ofstream mFragReceived(receivedFile.string(), std::ofstream::binary);
			mFragReceived << aFragmentShaderString;
			mFragReceived.close();
			CI_LOG_V("file saved:" + receivedFile.string());*/

			// try to compile a first time to get active uniforms
			mShader = gl::GlslProg::create(mVDSettings->getDefaultVextexShaderString(), aFragmentShaderString);
			// update only if success
			mFragmentShaderString = aFragmentShaderString;
			mVDSettings->mMsg = aName + " loaded and compiled\n" + mVDSettings->mMsg.substr(0, mVDSettings->mMsgLength);

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
			string name;
			for (const auto &uniform : uniforms) {
				name = uniform.getName();
				CI_LOG_V(aName + ", uniform name:" + name);
				// if uniform is handled
				if (mVDAnimation->isExistingUniform(name)) {
					int uniformType = mVDAnimation->getUniformType(name);
					switch (uniformType)
					{
					case 0:
						// float
						mShader->uniform(name, mVDAnimation->getFloatUniformValueByName(name));
						mCurrentUniformsString += "uniform float " + name + "; // " + toString(mVDAnimation->getFloatUniformValueByName(name)) + "\n";
						if (name != "iTime") {
							mISFUniforms += ",\n"
								"		{\n"
								"			\"NAME\": \"" + name + "\", \n"
								"			\"TYPE\" : \"float\", \n"
								"			\"MIN\" : " + toString(mVDAnimation->getMinUniformValueByName(name)) + ",\n"
								"			\"MAX\" : " + toString(mVDAnimation->getMaxUniformValueByName(name)) + ",\n"
								"			\"DEFAULT\" : " + toString(mVDAnimation->getFloatUniformValueByName(name)) + "\n"
								"		}\n";
						}
						break;
					case 1:
						// sampler2D
						mShader->uniform(name, mVDAnimation->getSampler2DUniformValueByName(name));
						mCurrentUniformsString += "uniform sampler2D " + name + "; // " + toString(mVDAnimation->getSampler2DUniformValueByName(name)) + "\n";
						break;
					case 2:
						// vec2
						mShader->uniform(name, mVDAnimation->getVec2UniformValueByName(name));
						mCurrentUniformsString += "uniform vec2 " + name + "; // " + toString(mVDAnimation->getVec2UniformValueByName(name)) + "\n";
						break;
					case 3:
						// vec3
						mShader->uniform(name, mVDAnimation->getVec3UniformValueByName(name));
						mCurrentUniformsString += "uniform vec3 " + name + "; // " + toString(mVDAnimation->getVec3UniformValueByName(name)) + "\n";
						break;
					case 4:
						// vec4
						mShader->uniform(name, mVDAnimation->getVec4UniformValueByName(name));
						mCurrentUniformsString += "uniform vec4 " + name + "; // " + toString(mVDAnimation->getVec4UniformValueByName(name)) + "\n";
						break;
					case 5:
						// int
						mShader->uniform(name, mVDAnimation->getIntUniformValueByName(name));
						mCurrentUniformsString += "uniform int " + name + "; // " + toString(mVDAnimation->getIntUniformValueByName(name)) + "\n";
						break;
					case 6:
						// bool
						mShader->uniform(name, mVDAnimation->getBoolUniformValueByName(name));
						mCurrentUniformsString += "uniform bool " + name + "; // " + toString(mVDAnimation->getBoolUniformValueByName(name)) + "\n";
						break;
					default:
						break;
					}
				}
				else {
					if (name != "ciModelViewProjection") {
						mNotFoundUniformsString += "not found " + name + "\n";
					}
				}
			}

			// save ISF
			string mISFHeader = "/*{\n"
				"	\"CREDIT\" : \"" + aName + " by \",\n"
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
			fileName = aName + ".fs";
			fs::path OFIsfFile = getAssetPath("") / "glsl" / "osf" / fileName;
			ofstream mOFISF(OFIsfFile.string(), std::ofstream::binary);
			mOFISF << mOFISFString;
			mOFISF.close();
			CI_LOG_V("OF ISF file saved:" + OFIsfFile.string());

			// ifs for HeavyM
			fileName = aName + ".fs";
			fs::path isfFile = getAssetPath("") / "glsl" / "isf" / fileName;
			ofstream mISF(isfFile.string(), std::ofstream::binary);
			mISF << mISFString;
			mISF.close();
			CI_LOG_V("ISF file saved:" + isfFile.string());


			// add "out vec4 fragColor" if necessary
			std::size_t foundFragColorDeclaration = mOriginalFragmentString.find("out vec4 fragColor;");
			if (foundFragColorDeclaration == std::string::npos) {
				mNotFoundUniformsString += "*/\nout vec4 fragColor;\nvec2  fragCoord = gl_FragCoord.xy; // keep the 2 spaces between vec2 and fragCoord\n";
			}
			else {
				mNotFoundUniformsString += "*/\nvec2  fragCoord = gl_FragCoord.xy; // keep the 2 spaces between vec2 and fragCoord\n";
			}
			mCurrentUniformsString += "// active uniforms end\n";
			// save .frag file to migrate old shaders
			mProcessedShaderString = mNotFoundUniformsString + mCurrentUniformsString + mOriginalFragmentString;
			fileName = aName + ".frag";
			fs::path processedFile = getAssetPath("") / "glsl" / "processed" / fileName;
			ofstream mFragProcessed(processedFile.string(), std::ofstream::binary);
			mFragProcessed << mProcessedShaderString;
			mFragProcessed.close();
			CI_LOG_V("processed file saved:" + processedFile.string());
		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = aName + string(exc.what());
			CI_LOG_V("setFragmentString, unable to compile live fragment shader:" + mError + " frag:" + aFragmentShaderString);
		}
		catch (const std::exception &e)
		{
			mError = aName + string(e.what());
			CI_LOG_V("setFragmentString, error on live fragment shader:" + mError + " frag:" + aFragmentShaderString);
		}
	}
	mVDSettings->mErrorMsg = mError + "\n" + mVDSettings->mErrorMsg.substr(0, mVDSettings->mMsgLength);
	return mValid;
}


#pragma warning(pop) // _CRT_SECURE_NO_WARNINGS
