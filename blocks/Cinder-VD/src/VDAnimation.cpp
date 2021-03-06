#include "VDAnimation.h"

using namespace videodromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	mBlendRender = false;
	//audio
	mAudioBuffered = false;	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	mAudioTexture = ci::gl::Texture::create(64, 2, mAudioFormat);
	mLineInInitialized = false;
	mWaveInitialized = false;
	mAudioName = "not initialized";
	//setUseLineIn(true);
	maxVolume = 0.0f;
	for (int i = 0; i < 7; i++)
	{
		freqIndexes[i] = i * 7;
	}
	for (int i = 0; i < mFFTWindowSize; i++)
	{
		iFreqs[i] = 0.0f;
	}
	// live json params
	/*mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "live_params.json";
	JsonBag::add(&mBackgroundColor, "background_color");
	JsonBag::add(&mExposure, "exposure", []() {
		app::console() << "Updated exposure" << endl;

	});
	JsonBag::add(&mText, "text", []() {
		app::console() << "Updated text" << endl;
	});
	mAutoBeatAnimation = true;
	JsonBag::add(&mAutoBeatAnimation, "autobeatanimation");*/
	currentScene = 0;

	previousTime = 0.0f;
	counter = 0;
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	mTimer.start();
	startTime = currentTime = mTimer.getSeconds();

	mUniformsJson = getAssetPath("") / mVDSettings->mAssetsPath / "uniforms.json";
	if (fs::exists(mUniformsJson)) {
		loadUniforms(loadFile(mUniformsJson));
	}
	else {
		// global time in seconds
		// TODO 20200301 get rid of iTime createFloatUniform("iTime", mVDSettings->ITIME, 0.0f); // 0
		createFloatUniform("TIME", mVDSettings->ITIME, 0.0f); // 0
		// sliders
		// red
		createFloatUniform("r", mVDSettings->IFR, 0.45f); // 1
		// green
		createFloatUniform("g", mVDSettings->IFG, 0.0f); // 2
		// blue
		createFloatUniform("b", mVDSettings->IFB, 1.0f); // 3
		// Alpha 
		createFloatUniform("iAlpha", mVDSettings->IFA, 1.0f); // 4
		// red multiplier 
		createFloatUniform("iRedMultiplier", mVDSettings->IFRX, 1.0f, 0.0f, 3.0f); // 5
		// green multiplier 
		createFloatUniform("iGreenMultiplier", mVDSettings->IFGX, 1.0f, 0.0f, 3.0f); // 6
		// blue multiplier 
		createFloatUniform("iBlueMultiplier", mVDSettings->IFBX, 1.0f, 0.0f, 3.0f); // 7
		// gstnsmk
		createFloatUniform("iSobel", mVDSettings->ISOBEL, 0.02f, 0.02f, 1.0f); // 8
		// bad tv
		//createFloatUniform("iBadTv", mVDSettings->IBADTV, 0.0f, 0.0f, 5.0f); // 9
		// Steps
		createFloatUniform("iSteps", mVDSettings->ISTEPS, 16.0f, 1.0f, 128.0f); // 10

		// rotary
		// ratio
		//createFloatUniform("iRatio", mVDSettings->IRATIO, 1.0f, 0.01f, 1.0f); // 11
		createFloatUniform("iRatio", mVDSettings->IRATIO, 20.0f, 0.00000000001f, 20.0f); // 11
		// zoom
		createFloatUniform("iZoom", mVDSettings->IZOOM, 1.0f, 0.95f, 1.1f); // 12
		// Audio multfactor 
		createFloatUniform("iAudioMult", mVDSettings->IAUDIOX, 1.0f, 0.01f, 12.0f); // 13
		// exposure
		createFloatUniform("iExposure", mVDSettings->IEXPOSURE, 1.0f, 0.0f, 3.0f); // 14
		// Pixelate
		createFloatUniform("iPixelate", mVDSettings->IPIXELATE, 1.0f, 0.01f); // 15
		// Trixels
		createFloatUniform("iTrixels", mVDSettings->ITRIXELS, 0.0f); // 16
		// iChromatic
		createFloatUniform("iChromatic", mVDSettings->ICHROMATIC, 0.0f, 0.000000001f); // 17
		// iCrossfade
		createFloatUniform("iCrossfade", mVDSettings->IXFADE, 1.0f); // 18
		// tempo time
		createFloatUniform("iTempoTime", mVDSettings->ITEMPOTIME, 0.1f); // 19
		// fps
		createFloatUniform("iFps", mVDSettings->IFPS, 60.0f, 0.0f, 500.0f); // 20	
		// iBpm 
		createFloatUniform("iBpm", mVDSettings->IBPM, 165.0f, 0.000000001f, 400.0f); // 21
		// Speed 
		createFloatUniform("speed", mVDSettings->ISPEED, 0.01f, 0.01f, 12.0f); // 22
		// slitscan / matrix (or other) Param1 
		createFloatUniform("iPixelX", mVDSettings->IPIXELX, 0.01f, -1.5f, 1.5f); // 23
		// slitscan / matrix(or other) Param2 
		createFloatUniform("iPixelY", mVDSettings->IPIXELY, 0.01f, -1.5f, 1.5f); // 24
		// delta time in seconds
		createFloatUniform("iDeltaTime", mVDSettings->IDELTATIME, 60.0f / 160.0f); // 25

		 // background red
		createFloatUniform("iBR", mVDSettings->IBR, 0.56f); // 26
		// background green
		createFloatUniform("iBG", mVDSettings->IBG, 0.0f); // 27
		// background blue
		createFloatUniform("iBB", mVDSettings->IBB, 1.0f); // 28

		// iResolutionX (should be fbowidth?) 
		createFloatUniform("iResolutionX", mVDSettings->IRESX, mVDSettings->mRenderWidth, 320.01f, 4280.0f); // 29
		// iResolutionY (should be fboheight?)  
		createFloatUniform("iResolutionY", mVDSettings->IRESY, mVDSettings->mRenderHeight, 240.01f, 2160.0f); // 30

		// weight mix fbo texture 0
		createFloatUniform("iWeight0", mVDSettings->IWEIGHT0, 1.0f); // 31
		// weight texture 1
		createFloatUniform("iWeight1", mVDSettings->IWEIGHT1, 0.0f); // 32
		// weight texture 2
		createFloatUniform("iWeight2", mVDSettings->IWEIGHT2, 0.0f); // 33
		// weight texture 3
		createFloatUniform("iWeight3", mVDSettings->IWEIGHT3, 0.0f); // 34
		// weight texture 4
		createFloatUniform("iWeight4", mVDSettings->IWEIGHT4, 0.0f); // 35
		// weight texture 5
		createFloatUniform("iWeight5", mVDSettings->IWEIGHT5, 0.0f); // 36
		// weight texture 6
		createFloatUniform("iWeight6", mVDSettings->IWEIGHT6, 0.0f); // 37
		// weight texture 7
		createFloatUniform("iWeight7", mVDSettings->IWEIGHT7, 0.0f); // 38
		// weight texture 8 
		createFloatUniform("iWeight8", mVDSettings->IWEIGHT8, 0.0f); // 39
		// elapsed in bar 
		//createFloatUniform("iElapsed", mVDSettings->IELAPSED, 0.0f); // 39

		// contour
		createFloatUniform("iContour", mVDSettings->ICONTOUR, 0.0f, 0.0f, 0.5f); // 40
		// RotationSpeed
		createFloatUniform("iRotationSpeed", mVDSettings->IROTATIONSPEED, 0.02f, -0.1f, 0.1f); // 41

		// iMouseX  
		createFloatUniform("iMouseX", mVDSettings->IMOUSEX, 320.0f, 0.0f, 1280.0f); // 42
		// iMouseY  
		createFloatUniform("iMouseY", mVDSettings->IMOUSEY, 240.0f, 0.0f, 800.0f); // 43
		// iMouseZ  
		createFloatUniform("iMouseZ", mVDSettings->IMOUSEZ, 0.0f, 0.0f, 1.0f); // 44
		// vignette amount
		createFloatUniform("iVAmount", mVDSettings->IVAMOUNT, 0.91f, 0.0f, 1.0f); // 45
		// vignette falloff
		createFloatUniform("iVFallOff", mVDSettings->IVFALLOFF, 0.31f, 0.0f, 1.0f); // 46
		// hydra time
		//createFloatUniform("time", mVDSettings->TIME, 0.0f); // 47
		// current beat
		//createFloatUniform("iPhase", mVDSettings->IPHASE, 0.0f); // 48
		// iTimeFactor
		createFloatUniform("iTimeFactor", mVDSettings->ITIMEFACTOR, 1.0f); // 49
		// int
		// blend mode 
		createIntUniform("iBlendmode", mVDSettings->IBLENDMODE, 0); // 50
		// beat 
		createFloatUniform("iBeat", mVDSettings->IBEAT, 0.0f, 0.0f, 300.0f); // 51
		// bar 
		createFloatUniform("iBar", mVDSettings->IBAR, 0.0f, 0.0f, 8.0f); // 52
		// bar 
		createFloatUniform("iBarBeat", mVDSettings->IBARBEAT, 1.0f, 1.0f, 1200.0f); // 53		
		// fbo A
		createIntUniform("iFboA", mVDSettings->IFBOA, 0); // 54
		// fbo B
		createIntUniform("iFboB", mVDSettings->IFBOB, 1); // 55
		// iOutW
		createIntUniform("iOutW", mVDSettings->IOUTW, mVDSettings->mRenderWidth); // 56
		// iOutH  
		createIntUniform("iOutH", mVDSettings->IOUTH, mVDSettings->mRenderHeight); // 57
		// beats per bar 
		createIntUniform("iBeatsPerBar", mVDSettings->IBEATSPERBAR, 4); // 59

		// vec3
		createVec3Uniform("iResolution", mVDSettings->IRESOLUTION, vec3(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"), 1.0)); // 60
		createVec3Uniform("iColor", mVDSettings->ICOLOR, vec3(0.45, 0.0, 1.0)); // 61
		createVec3Uniform("iBackgroundColor", mVDSettings->IBACKGROUNDCOLOR); // 62
		//createVec3Uniform("iChannelResolution[0]", 63, vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));

		// vec4
		createVec4Uniform("iMouse", mVDSettings->IMOUSE, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("iDate", 71, vec4(2019.0f, 12.0f, 1.0f, 5.0f));

		// boolean
		// invert
		// glitch
		createBoolUniform("iGlitch", mVDSettings->IGLITCH); // 81
		// vignette
		createBoolUniform("iVignette", mVDSettings->IVIGN); // 82 toggle
		// toggle
		createBoolUniform("iToggle", mVDSettings->ITOGGLE); // 83
		// invert
		createBoolUniform("iInvert", mVDSettings->IINVERT); // 86
		// greyscale 
		createBoolUniform("iGreyScale", mVDSettings->IGREYSCALE); //87

		createBoolUniform("iClear", mVDSettings->ICLEAR, true); // 88
		createBoolUniform("iDebug", mVDSettings->IDEBUG); // 129
		createBoolUniform("iXorY", mVDSettings->IXORY); // 130
		createBoolUniform("iFlipH", mVDSettings->IFLIPH); // 131
		createBoolUniform("iFlipV", mVDSettings->IFLIPV); // 132
		createBoolUniform("iFlipPostH", mVDSettings->IFLIPPOSTH); // 133
		createBoolUniform("iFlipPostV", mVDSettings->IFLIPPOSTV); // 134

		// 119 to 124 timefactor from midithor sos
		// floats for warps
		// srcArea 
		createFloatUniform("srcXLeft", mVDSettings->SRCXLEFT, 0.0f, 0.0f, 4280.0f); // 160
		createFloatUniform("srcXRight", mVDSettings->SRCXRIGHT, mVDSettings->mRenderWidth, 320.01f, 4280.0f); // 161
		createFloatUniform("srcYLeft", mVDSettings->SRCYLEFT, 0.0f, 0.0f, 1024.0f); // 162
		createFloatUniform("srcYRight", mVDSettings->SRCYRIGHT, mVDSettings->mRenderHeight, 0.0f, 1024.0f); // 163
		// iFreq0  
		createFloatUniform("iFreq0", mVDSettings->IFREQ0, 0.0f, 0.01f, 256.0f); // 140	
		// iFreq1  
		createFloatUniform("iFreq1", mVDSettings->IFREQ1, 0.0f, 0.01f, 256.0f); // 141
		// iFreq2  
		createFloatUniform("iFreq2", mVDSettings->IFREQ2, 0.0f, 0.01f, 256.0f); // 142
		// iFreq3  
		createFloatUniform("iFreq3", mVDSettings->IFREQ3, 0.0f, 0.01f, 256.0f); // 143

		// vec2
		createVec2Uniform("resolution", mVDSettings->RESOLUTION, vec2(1280.0f, 720.0f)); // hydra 150
		createVec2Uniform("RENDERSIZE", mVDSettings->RENDERSIZE, vec2(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"))); // isf 151

		// vec4 kinect2
		createVec4Uniform("iSpineBase", 200, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("SpineMid", 201, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("Neck", 202, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("Head", 203, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ShldrL", 204, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ElbowL", 205, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("WristL", 206, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HandL", 207, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ShldrR", 208, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ElbowR", 209, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("WristR", 210, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HandR", 211, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HipL", 212, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("KneeL", 213, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("AnkleL", 214, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("FootL", 215, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HipR", 216, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("KneeR", 217, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("AnkleR", 218, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("FootR", 219, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("SpineShldr", 220, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HandTipL", 221, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ThumbL", 222, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("HandTipR", 223, vec4(320.0f, 240.0f, 0.0f, 0.0f));
		createVec4Uniform("ThumbR", 224, vec4(320.0f, 240.0f, 0.0f, 0.0f));


	}
	// textures
	for (size_t i = 0; i < 30; i++)
	{
		createSampler2DUniform("iChannel" + toString(i), 300 + i, i);// TODO verify doesn't mess up type (uint!)
	}
	createSampler2DUniform("inputImage", 399, 0);// TODO verify doesn't mess up type (uint!)

	// iRHandX  
	//createFloatUniform("iRHandX", mVDSettings->IRHANDX, 320.0f, 0.0f, 1280.0f);
	//// iRHandY  
	//createFloatUniform("iRHandY", mVDSettings->IRHANDY, 240.0f, 0.0f, 800.0f);
	//// iLHandX  
	//createFloatUniform("iLHandX", mVDSettings->ILHANDX, 320.0f, 0.0f, 1280.0f);
	//// iLHandY  
	//createFloatUniform("iLHandY", mVDSettings->ILHANDY, 240.0f, 0.0f, 800.0f);

	load();
	loadAnimation();

	setVec3UniformValueByIndex(mVDSettings->IRESOLUTION, vec3(getFloatUniformValueByIndex(mVDSettings->IRESX), getFloatUniformValueByIndex(mVDSettings->IRESY), 1.0));
}
void VDAnimation::loadUniforms(const ci::DataSourceRef &source) {

	JsonTree json(source);

	// try to load the specified json file
	if (json.hasChild("uniforms")) {
		JsonTree u(json.getChild("uniforms"));

		// iterate uniforms
		for (size_t i = 0; i < u.getNumChildren(); i++) {
			JsonTree child(u.getChild(i));

			if (child.hasChild("uniform")) {
				JsonTree w(child.getChild("uniform"));
				// create uniform of the correct type
				int uniformType = (w.hasChild("type")) ? w.getValueForKey<int>("type") : 0;
				switch (uniformType) {
				case 0:
					//float
					floatFromJson(child);
					break;
				case 1:
					// sampler2d
					sampler2dFromJson(child);
					break;
				case 2:
					// vec2
					vec2FromJson(child);
					break;
				case 3:
					// vec3
					vec3FromJson(child);
					break;
				case 4:
					// vec4
					vec4FromJson(child);
					break;
				case 5:
					// int
					intFromJson(child);
					break;
				case 6:
					// boolean
					boolFromJson(child);
					break;
				}
			}
		}
	}
}
void VDAnimation::floatFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	float jValue, jMin, jMax;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 249;
		jValue = (u.hasChild("value")) ? u.getValueForKey<float>("value") : 0.01f;
		jMin = (u.hasChild("min")) ? u.getValueForKey<float>("min") : 0.0f;
		jMax = (u.hasChild("max")) ? u.getValueForKey<float>("max") : 1.0f;
		createFloatUniform(jName, jCtrlIndex, jValue, jMin, jMax);
	}
}
void VDAnimation::sampler2dFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	int jTextureIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 250;
		jTextureIndex = (u.hasChild("textureindex")) ? u.getValueForKey<int>("textureindex") : 0;;
		createSampler2DUniform(jName, jTextureIndex);
	}
}
void VDAnimation::vec2FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 251;
		createVec2Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::vec3FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 252;
		createVec3Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::vec4FromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 253;
		createVec4Uniform(jName, jCtrlIndex);
	}
}
void VDAnimation::intFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex, jValue;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 254;
		jValue = (u.hasChild("value")) ? u.getValueForKey<int>("value") : 1;
		createIntUniform(jName, jCtrlIndex, jValue);
	}

}
void VDAnimation::boolFromJson(const ci::JsonTree &json) {
	string jName;
	int jCtrlIndex;
	bool jValue;
	if (json.hasChild("uniform")) {
		JsonTree u(json.getChild("uniform"));
		jName = (u.hasChild("name")) ? u.getValueForKey<string>("name") : "unknown";
		jCtrlIndex = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 255;
		jValue = (u.hasChild("value")) ? u.getValueForKey<bool>("value") : false;
		createBoolUniform(jName, jCtrlIndex, jValue);
	}
}
//! uniform to json
JsonTree VDAnimation::uniformToJson(int i)
{
	stringstream svec4;
	JsonTree		json;
	string s = controlIndexes[i];

	JsonTree u = JsonTree::makeArray("uniform");
	// common
	int uniformType = shaderUniforms[s].uniformType;
	u.addChild(ci::JsonTree("type", uniformType));
	u.addChild(ci::JsonTree("name", s));
	u.addChild(ci::JsonTree("index", i));
	// type specific 
	switch (uniformType) {
	case 0:
		//float
		u.addChild(ci::JsonTree("value", shaderUniforms[s].defaultValue));
		u.addChild(ci::JsonTree("min", shaderUniforms[s].minValue));
		u.addChild(ci::JsonTree("max", shaderUniforms[s].maxValue));
		break;
	case 1:
		// sampler2d
		u.addChild(ci::JsonTree("textureindex", shaderUniforms[s].textureIndex));
		break;
	case 4:
		// vec4
		svec4 << toString(shaderUniforms[s].vec4Value.x) << "," << toString(shaderUniforms[s].vec4Value.y);
		svec4 << "," << toString(shaderUniforms[s].vec4Value.z) << "," << toString(shaderUniforms[s].vec4Value.w);
		u.addChild(ci::JsonTree("value", svec4.str()));
		break;
	case 5:
		// int
		u.addChild(ci::JsonTree("value", shaderUniforms[s].intValue));
		break;
	case 6:
		// boolean
		u.addChild(ci::JsonTree("value", shaderUniforms[s].boolValue));
		break;
	default:
		break;
	}

	json.pushBack(u);
	return json;
}
void VDAnimation::saveUniforms()
{
	string jName;
	int ctrlSize = math<int>::min(310, controlIndexes.size());
	float jMin, jMax;
	JsonTree		json;
	// create uniforms json
	JsonTree uniformsJson = JsonTree::makeArray("uniforms");

	for (unsigned i = 0; i < ctrlSize; ++i) {
		JsonTree		u(uniformToJson(i));
		// create <uniform>
		uniformsJson.pushBack(u);
	}
	// write file
	json.pushBack(uniformsJson);
	json.write(mUniformsJson);
}

void VDAnimation::createFloatUniform(const string& aName, int aCtrlIndex, float aValue, float aMin, float aMax) {
	if (aName != "") {
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
}
void VDAnimation::createSampler2DUniform(const string& aName, int aCtrlIndex, int aTextureIndex) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	shaderUniforms[aName].textureIndex = aTextureIndex;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 1;
	shaderUniforms[aName].isValid = true;
}
void VDAnimation::createVec2Uniform(const string& aName, int aCtrlIndex, vec2 aValue) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 2;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec2Value = aValue;
}
void VDAnimation::createVec3Uniform(const string& aName, int aCtrlIndex, vec3 aValue) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 3;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec3Value = aValue;
}
void VDAnimation::createVec4Uniform(const string& aName, int aCtrlIndex, vec4 aValue) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 4;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].vec4Value = aValue;
}
void VDAnimation::createIntUniform(const string& aName, int aCtrlIndex, int aValue) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].uniformType = 5;
	shaderUniforms[aName].isValid = true;
	shaderUniforms[aName].intValue = aValue;
}
void VDAnimation::createBoolUniform(const string& aName, int aCtrlIndex, bool aValue) {
	if (aName == "") {
		controlIndexes[aCtrlIndex] = aName;
	}
	controlIndexes[aCtrlIndex] = aName;
	shaderUniforms[aName].minValue = 0;
	shaderUniforms[aName].maxValue = 1;
	shaderUniforms[aName].defaultValue = aValue;
	shaderUniforms[aName].boolValue = aValue;
	shaderUniforms[aName].autotime = false;
	shaderUniforms[aName].automatic = false;
	shaderUniforms[aName].autobass = false;
	shaderUniforms[aName].automid = false;
	shaderUniforms[aName].autotreble = false;
	shaderUniforms[aName].index = aCtrlIndex;
	shaderUniforms[aName].floatValue = aValue;
	shaderUniforms[aName].uniformType = 6;
	shaderUniforms[aName].isValid = true;
}

/*bool VDAnimation::hasFloatChanged(int aIndex) {
	if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]) {
	//CI_LOG_V("hasFloatChanged, aIndex:" + toString(aIndex));
	CI_LOG_V("hasFloatChanged, shaderUniforms[getUniformNameForIndex(aIndex)].floatValue:" + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue));
	CI_LOG_V("hasFloatChanged, controlValues[aIndex]:" + toString(controlValues[aIndex]));
	//CI_LOG_W("hasFloatChanged, getUniformNameForIndex(aIndex):" + toString(getUniformNameForIndex(aIndex)));
	}
	return (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]);
	}*/
bool VDAnimation::toggleValue(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].boolValue = !shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
	return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
}
bool VDAnimation::toggleAuto(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automatic = !shaderUniforms[getUniformNameForIndex(aIndex)].automatic;
	return shaderUniforms[getUniformNameForIndex(aIndex)].automatic;
}
bool VDAnimation::toggleTempo(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autotime = !shaderUniforms[getUniformNameForIndex(aIndex)].autotime;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autotime;
}
bool VDAnimation::toggleBass(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autobass = !shaderUniforms[getUniformNameForIndex(aIndex)].autobass;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autobass;
}
bool VDAnimation::toggleMid(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automid = !shaderUniforms[getUniformNameForIndex(aIndex)].automid;
	return shaderUniforms[getUniformNameForIndex(aIndex)].automid;
}
bool VDAnimation::toggleTreble(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].autotreble = !shaderUniforms[getUniformNameForIndex(aIndex)].autotreble;
	return shaderUniforms[getUniformNameForIndex(aIndex)].autotreble;
}
void VDAnimation::resetAutoAnimation(unsigned int aIndex) {
	shaderUniforms[getUniformNameForIndex(aIndex)].automatic = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autotime = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autobass = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].automid = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].autotreble = false;
	shaderUniforms[getUniformNameForIndex(aIndex)].floatValue = shaderUniforms[getUniformNameForIndex(aIndex)].defaultValue;
}

bool VDAnimation::setFloatUniformValueByIndex(unsigned int aIndex, float aValue) {
	bool rtn = false;
	// we can't change TIME at index 0
	if (aIndex > 0) {
		/*if (aIndex == 31) {
			CI_LOG_V("old value " + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue) + " newvalue " + toString(aValue));
		}*/
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

bool VDAnimation::isExistingUniform(const string& aName) {
	return shaderUniforms[aName].isValid;
}
int VDAnimation::getUniformType(const string& aName) {
	return shaderUniforms[aName].uniformType;
}
void VDAnimation::load() {
	// Create json file if it doesn't already exist.
/*#if defined( CINDER_MSW )
	if (fs::exists(mJsonFilePath)) {
		bag()->load(mJsonFilePath);
	}
	else {
		bag()->save(mJsonFilePath);
		bag()->load(mJsonFilePath);
	}
#endif*/
}
void VDAnimation::save() {
	saveAnimation();
	saveUniforms();
}
void VDAnimation::saveAnimation() {
	// save 
	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	JsonTree doc;
	JsonTree badtv = JsonTree::makeArray("badtv");

	for (const auto& item : mBadTV) {
		if (item.second > 0.0001) badtv.addChild(ci::JsonTree(ci::toString(item.first), ci::toString(item.second)));
	}

	doc.pushBack(badtv);
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions()); */
	// backup save
	/*string fileName = "animation" + toString(getElapsedFrames()) + ".json";
	mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / fileName;
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions());*/
}
void VDAnimation::loadAnimation() {

	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	// Create json file if it doesn't already exist.
	if (!fs::exists(mJsonFilePath)) {
		std::ofstream oStream(mJsonFilePath.string());
		oStream.close();
	}
	if (!fs::exists(mJsonFilePath)) {
		return;
	}
	try {
		JsonTree doc(loadFile(mJsonFilePath));
		JsonTree badtv(doc.getChild("badtv"));
		for (JsonTree::ConstIter item = badtv.begin(); item != badtv.end(); ++item) {
			const auto& key = std::stoi(item->getKey());
			const auto& value = item->getValue<float>();
			mBadTV[key] = value;

		}
	}
	catch (const JsonTree::ExcJsonParserError&) {
		CI_LOG_W("Failed to parse json file.");
	} */
}
/*
void VDAnimation::setExposure(float aExposure) {
	mExposure = aExposure;
}
void VDAnimation::setAutoBeatAnimation(bool aAutoBeatAnimation) {
	mAutoBeatAnimation = aAutoBeatAnimation;
}*/
bool VDAnimation::handleKeyDown(KeyEvent &event)
{
	//float newValue;
	/*bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_s:
		// save animation
		save();
		break;
	//case KeyEvent::KEY_u:
	//	// save badtv keyframe
	//	mBadTV[getElapsedFrames() - 10] = 1.0f;
	//	//iBadTvRunning = true;
	//	// duration = 0.2
	//	shaderUniforms["iBadTv"].floatValue = 5.0f;
	//	//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
	//	break;
	case KeyEvent::KEY_d:
		// save end keyframe
		setEndFrame(getElapsedFrames() - 10);
		break;

		//case KeyEvent::KEY_x:
	case KeyEvent::KEY_y:
		mVDSettings->iXorY = !mVDSettings->iXorY;
		break;

	default:
		handled = false;
	} */

	event.setHandled(false);

	return event.isHandled();
}
bool VDAnimation::handleKeyUp(KeyEvent &event)
{
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_u:
		// save badtv keyframe
		// not used for now mBadTV[getElapsedFrames()] = 0.001f;
		//shaderUniforms["iBadTv"].floatValue = 0.0f;
		break;

	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}
ci::gl::TextureRef VDAnimation::getAudioTexture() {

	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	auto ctx = audio::Context::master();
	if (!mLineInInitialized) {
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
		if (getUseLineIn()) {
			// linein
			preventLineInCrash(); // at next launch
			CI_LOG_W("trying to open mic/line in, if no line follows in the log, the app crashed so put UseLineIn to false in the VDSettings.xml file");
			mLineIn = ctx->createInputDeviceNode(); //crashes if linein is present but disabled, doesn't go to catch block
			CI_LOG_V("mic/line in opened");
			saveLineIn();
			mAudioName = mLineIn->getDevice()->getName();
			auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
			mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
			mLineIn >> mMonitorLineInSpectralNode;
			mLineIn->enable();
			mLineInInitialized = true;
		}
	}
#endif
	if (!mWaveInitialized) {
		if (getUseAudio()) {
			// also initialize wave monitor
			auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
			mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));
			ctx->enable();
			mAudioName = "wave";
			mWaveInitialized = true;
		}
	}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	if (getUseLineIn()) {
		mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
	}
	else {
#endif
		if (getUseAudio()) {
			if (isAudioBuffered()) {
				if (mBufferPlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
				}
			}
			else {
				if (mSamplePlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
					mPosition = mSamplePlayerNode->getReadPosition();
				}
			}
		}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	}
#endif
	if (!mMagSpectrum.empty()) {

		maxVolume = 0.0f;//mIntensity
		size_t mDataSize = mMagSpectrum.size();
		if (mDataSize > 0 && mDataSize < mFFTWindowSize) {// TODO 20200221 CHECK was + 1
			float db;
			unsigned char signal[mFFTWindowSize];
			for (size_t i = 0; i < mDataSize; i++) {
				float f = mMagSpectrum[i];
				db = audio::linearToDecibel(f);
				f = db * getFloatUniformValueByName("iAudioMult");
				if (f > maxVolume)
				{
					maxVolume = f;
				}
				iFreqs[i] = f;
				// update iFreq uniforms 
				if (i == getFreqIndex(0)) setFloatUniformValueByName("iFreq0", f);
				if (i == getFreqIndex(1)) setFloatUniformValueByName("iFreq1", f);
				if (i == getFreqIndex(2)) setFloatUniformValueByName("iFreq2", f);
				if (i == getFreqIndex(3)) setFloatUniformValueByName("iFreq3", f);

				if (i < mFFTWindowSize) {
					int ger = f;
					signal[i] = static_cast<unsigned char>(ger);
				}
			}
			// store it as a 512x2 texture
			// 20200222 mAudioTexture = gl::Texture::create(signal, GL_RED, 64, 2, mAudioFormat);
			mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
			/* TODO 20200221 useful?
			if (isAudioBuffered() && mBufferPlayerNode) {
				gl::ScopedFramebuffer fbScp(mFbo);
				gl::clear(Color::black());

				mAudioTexture->bind(0);

				//mWaveformPlot.draw();
				// draw the current play position
				mPosition = mBufferPlayerNode->getReadPosition();
				float readPos = (float)mWidth * mPosition / mBufferPlayerNode->getNumFrames();
				gl::color(ColorA(0, 1, 0, 0.7f));
				gl::drawSolidRect(Rectf(readPos - 2, 0, readPos + 2, (float)mHeight));
				mRenderedTexture = mFbo->getColorTexture();
				return mRenderedTexture;
			} */
		}
	}
	else {
		// generate random values
		// 20200222 for (int i = 0; i < 128; ++i) dTexture[i] = (unsigned char)(i);
		// 20200222 mAudioTexture = gl::Texture::create(dTexture, GL_RED, 64, 2, mAudioFormat);
		// get freqs from Speckthor in VDRouter.cpp
		float db;
		unsigned char signal[mFFTWindowSize];
		for (size_t i = 0; i < mFFTWindowSize; i++) {
			float f = iFreqs[i];
			if (f > maxVolume)
			{
				maxVolume = f;
			}
			// update iFreq uniforms 
			if (i == getFreqIndex(0)) setFloatUniformValueByName("iFreq0", f);
			if (i == getFreqIndex(1)) setFloatUniformValueByName("iFreq1", f);
			if (i == getFreqIndex(2)) setFloatUniformValueByName("iFreq2", f);
			if (i == getFreqIndex(3)) setFloatUniformValueByName("iFreq3", f);

			if (i < mFFTWindowSize) {
				int ger = f;
				signal[i] = static_cast<unsigned char>(ger);
			}
		}
		// store it as a 512x2 texture
		// 20200222 mAudioTexture = gl::Texture::create(signal, GL_RED, 64, 2, mAudioFormat);
		mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
		mAudioName = "speckthor";
	}

	return mAudioTexture;
};
void VDAnimation::update() {

	if (mBadTV[getElapsedFrames()] == 0) {
		// TODO check shaderUniforms["iBadTv"].floatValue = 0.0f;
	}
	else {
		// duration = 0.2
		//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		shaderUniforms["iBadTv"].floatValue = 5.0f;
	}

	mVDSettings->iChannelTime[0] = getElapsedSeconds();
	mVDSettings->iChannelTime[1] = getElapsedSeconds() - 1;
	mVDSettings->iChannelTime[2] = getElapsedSeconds() - 2;
	mVDSettings->iChannelTime[3] = getElapsedSeconds() - 3;
	// TIME
	if (mUseTimeWithTempo)
	{
		// Ableton Link from openframeworks websockets
		shaderUniforms["TIME"].floatValue = shaderUniforms["TIME"].floatValue * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;
		//shaderUniforms["iElapsed"].floatValue = shaderUniforms["iPhase"].floatValue * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;
		// sos
		// IBARBEAT = IBAR * 4 + IBEAT
		int current = getIntUniformValueByIndex(mVDSettings->IBARBEAT);
		if (current == 426 || current == 428 || current == 442) mLastBar = 0; //38 to set iStart
		if (mLastBar != getIntUniformValueByIndex(mVDSettings->IBAR)) {
			mLastBar =getIntUniformValueByIndex(mVDSettings->IBAR);
			//if (mLastBar != 5 && mLastBar != 9 && mLastBar < 113) mVDSettings->iStart = mVDSession->getFloatUniformValueByIndex(mVDSettings->ITIME);
			// TODO CHECK
			//if (mLastBar != 107 && mLastBar != 111 && mLastBar < 205) mVDSettings->iStart = mVDSession->getFloatUniformValueByIndex(mVDSettings->ITIME);
			if (mLastBar < 419 && mLastBar > 424) mVDSettings->iStart = getFloatUniformValueByIndex(mVDSettings->ITIME);
		}
	}
	else
	{
		shaderUniforms["TIME"].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;//mVDSettings->iTimeFactor;
		//shaderUniforms["iElapsed"].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;//mVDSettings->iTimeFactor;
	}
	// iResolution
	shaderUniforms["iResolution"].vec3Value = vec3(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"), 1.0);
	shaderUniforms["resolution"].vec2Value = vec2(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"));
	shaderUniforms["RENDERSIZE"].vec2Value = vec2(getFloatUniformValueByName("iResolutionX"), getFloatUniformValueByName("iResolutionY"));

	// iDate
	time_t now = time(0);
	tm *   t = gmtime(&now);
	shaderUniforms["iDate"].vec4Value = vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday), float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec));

#pragma region animation

	currentTime = mTimer.getSeconds();
	// TODO check bounds
	//if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;

	int elapsed = shaderUniforms["iDeltaTime"].floatValue * 1000000.0;
	int elapsedBeatPerBar = shaderUniforms["iDeltaTime"].floatValue / (shaderUniforms["iBeatsPerBar"].intValue + 1)*1000000.0;
	/*if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (shaderUniforms["iPhase"].intValue > shaderUniforms["iBeatsPerBar"].intValue ) shaderUniforms["iPhase"].intValue = 0;
			shaderUniforms["iPhase"].intValue++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	} */
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		shaderUniforms["iTempoTime"].floatValue = (float)abs(modulo);

		/* not used shaderUniforms["iDeltaTime"].floatValue = (float)abs(modulo);
		if (shaderUniforms["iTempoTime"].floatValue < previousTime)
		{
			//iBar++;
			//if (mAutoBeatAnimation) mVDSettings->iPhase++;
		}*/
		previousTime = shaderUniforms["iTempoTime"].floatValue;

		// TODO (modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		for (unsigned int anim = 1; anim < 29; anim++)
		{
			if (shaderUniforms[getUniformNameForIndex(anim)].autotime)
			{
				setFloatUniformValueByIndex(anim, (modulo < 0.1) ? shaderUniforms[getUniformNameForIndex(anim)].maxValue : shaderUniforms[getUniformNameForIndex(anim)].minValue);
			}
			else
			{
				if (shaderUniforms[getUniformNameForIndex(anim)].automatic) {
					setFloatUniformValueByIndex(anim, lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, getFloatUniformValueByIndex(mVDSettings->IDELTATIME), shaderUniforms[getUniformNameForIndex(anim)].minValue, shaderUniforms[getUniformNameForIndex(anim)].maxValue));
				}
				else
				{
					if (shaderUniforms[getUniformNameForIndex(anim)].autobass) {
						setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ0) / 25.0f);
					}
					else
					{
						if (shaderUniforms[getUniformNameForIndex(anim)].automid) {
							setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ1) / 5.0f);
						}
						else
						{
							if (shaderUniforms[getUniformNameForIndex(anim)].autotreble) {
								setFloatUniformValueByIndex(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getFloatUniformValueByIndex(mVDSettings->IFREQ2) / 2.0f);
							}
						}
					}
				}
			}
		}

		// foreground color vec3 update
		shaderUniforms["iColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(mVDSettings->IFR)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IFG)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IFB)].floatValue);
		// background color vec3 update
		shaderUniforms["iBackgroundColor"].vec3Value = vec3(shaderUniforms[getUniformNameForIndex(mVDSettings->IBR)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IBG)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IBB)].floatValue);
		// mouse vec4 update
		shaderUniforms["iMouse"].vec4Value = vec4(shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEX)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEY)].floatValue, shaderUniforms[getUniformNameForIndex(mVDSettings->IMOUSEZ)].floatValue, 0.0f);
		// TODO migrate:
		if (mVDSettings->autoInvert)
		{
			setBoolUniformValueByIndex(mVDSettings->IINVERT, (modulo < 0.1) ? 1.0 : 0.0);
		}

		if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(shaderUniforms["iTempoTime"].floatValue, 0.00001, getFloatUniformValueByIndex(mVDSettings->IDELTATIME), mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
		}

	}
#pragma endregion animation
}

// tempo
void VDAnimation::tapTempo()
{
	startTime = currentTime = mTimer.getSeconds();

	mTimer.stop();
	mTimer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time buffer is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void VDAnimation::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	setFloatUniformValueByIndex(mVDSettings->IDELTATIME, averageTime);
	setBpm(60 / averageTime);
}

void VDAnimation::preventLineInCrash() {
	setUseLineIn(false);
	mVDSettings->save();
}
void VDAnimation::saveLineIn() {
	setUseLineIn(true);
	mVDSettings->save();
}