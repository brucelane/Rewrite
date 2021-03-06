//
//  VDSettings.cpp
//

#include "VDSettings.h"

using namespace ci;
using namespace std;
using namespace videodromm;

VDSettings::VDSettings(string filename)
{
	// reset no matter what, so we don't miss anything
	reset();
	settingsFileName = "VD" + filename + ".xml";
	// check to see if Settings.xml file exists and restore if it does
	fs::path params = getAppPath() / settingsFileName;
	if (fs::exists(params))
	{
		restore();
	}
	else
	{
		save();
	}
}

VDSettingsRef VDSettings::create(string filename)
{
	return shared_ptr<VDSettings>(new VDSettings(filename));
}

bool VDSettings::save()
{
	fs::path directory = getAppPath();
	/*if (!fs::exists(directory)) {
		if (!createDirectories(directory / settingsFileName)) {
		return false;
		}
		}*/

	fs::path path = directory / settingsFileName;

	XmlTree settings("settings", "");

	XmlTree AutoLayout("AutoLayout", "");
	AutoLayout.setAttribute("value", toString(mAutoLayout));
	settings.push_back(AutoLayout);

	XmlTree Standalone("Standalone", "");
	Standalone.setAttribute("value", toString(mStandalone));
	settings.push_back(Standalone);

	XmlTree MainWindowWidth("MainWindowWidth", "");
	MainWindowWidth.setAttribute("value", toString(mMainWindowWidth));
	settings.push_back(MainWindowWidth);

	XmlTree MainWindowHeight("MainWindowHeight", "");
	MainWindowHeight.setAttribute("value", toString(mMainWindowHeight));
	settings.push_back(MainWindowHeight);

	XmlTree MainWindowX("MainWindowX", "");
	MainWindowX.setAttribute("value", toString(mMainWindowX));
	settings.push_back(MainWindowX);

	XmlTree MainWindowY("MainWindowY", "");
	MainWindowY.setAttribute("value", toString(mMainWindowY));
	settings.push_back(MainWindowY);

	XmlTree RenderWidth("RenderWidth", "");
	RenderWidth.setAttribute("value", toString(mRenderWidth));
	settings.push_back(RenderWidth);

	XmlTree RenderHeight("RenderHeight", "");
	RenderHeight.setAttribute("value", toString(mRenderHeight));
	settings.push_back(RenderHeight);

	XmlTree RenderX("RenderX", "");
	RenderX.setAttribute("value", toString(mRenderX));
	settings.push_back(RenderX);

	XmlTree RenderY("RenderY", "");
	RenderY.setAttribute("value", toString(mRenderY));
	settings.push_back(RenderY);

	XmlTree AspectRatio("AspectRatio", "");
	AspectRatio.setAttribute("value", toString(mAspectRatio));
	settings.push_back(AspectRatio);

	XmlTree FboWidth("FboWidth", "");
	FboWidth.setAttribute("value", toString(mFboWidth));
	settings.push_back(FboWidth);

	XmlTree FboHeight("FboHeight", "");
	FboHeight.setAttribute("value", toString(mFboHeight));
	settings.push_back(FboHeight);

	XmlTree MIDIEnabled("MIDIEnabled", "");
	MIDIEnabled.setAttribute("value", toString(mMIDIEnabled));
	settings.push_back(MIDIEnabled);

	XmlTree OSCEnabled("OSCEnabled", "");
	OSCEnabled.setAttribute("value", toString(mOSCEnabled));
	settings.push_back(OSCEnabled);

	XmlTree OSCReceiverPort("OSCReceiverPort", "");
	OSCReceiverPort.setAttribute("value", toString(mOSCReceiverPort));
	settings.push_back(OSCReceiverPort);

	XmlTree OSCDestinationPort("OSCDestinationPort", "");
	OSCDestinationPort.setAttribute("value", toString(mOSCDestinationPort));
	settings.push_back(OSCDestinationPort);

	XmlTree OSCDestinationHost("OSCDestinationHost", "");
	OSCDestinationHost.setAttribute("value", toString(mOSCDestinationHost));
	settings.push_back(OSCDestinationHost);

	XmlTree OSCDestinationPort2("OSCDestinationPort2", "");
	OSCDestinationPort2.setAttribute("value", toString(mOSCDestinationPort2));
	settings.push_back(OSCDestinationPort2);

	XmlTree OSCDestinationHost2("OSCDestinationHost2", "");
	OSCDestinationHost2.setAttribute("value", toString(mOSCDestinationHost2));
	settings.push_back(OSCDestinationHost2);

	XmlTree MIDIOpenAllInputPorts("MIDIOpenAllInputPorts", "");
	MIDIOpenAllInputPorts.setAttribute("value", toString(mMIDIOpenAllInputPorts));
	settings.push_back(MIDIOpenAllInputPorts);

	XmlTree ShowUI("ShowUI", "");
	ShowUI.setAttribute("value", toString(mShowUI));
	settings.push_back(ShowUI);

	XmlTree CursorVisible("CursorVisible", "");
	CursorVisible.setAttribute("value", toString(mCursorVisible));
	settings.push_back(CursorVisible);

	XmlTree IsOSCSender("IsOSCSender", "");
	IsOSCSender.setAttribute("value", toString(mIsOSCSender));
	settings.push_back(IsOSCSender);

	XmlTree AreWebSocketsEnabledAtStartup("AreWebSocketsEnabledAtStartup", "");
	AreWebSocketsEnabledAtStartup.setAttribute("value", toString(mAreWebSocketsEnabledAtStartup));
	settings.push_back(AreWebSocketsEnabledAtStartup);

	XmlTree IsRouter("IsRouter", "");
	IsRouter.setAttribute("value", toString(mIsRouter));
	settings.push_back(IsRouter);

	XmlTree IsWebSocketsServer("IsWebSocketsServer", "");
	IsWebSocketsServer.setAttribute("value", toString(mIsWebSocketsServer));
	settings.push_back(IsWebSocketsServer);

	XmlTree WebSocketsProtocol("WebSocketsProtocol", "");
	WebSocketsProtocol.setAttribute("value", mWebSocketsProtocol);
	settings.push_back(WebSocketsProtocol);

	XmlTree WebSocketsHost("WebSocketsHost", "");
	WebSocketsHost.setAttribute("value", mWebSocketsHost);
	settings.push_back(WebSocketsHost);

	XmlTree WebSocketsPort("WebSocketsPort", "");
	WebSocketsPort.setAttribute("value", toString(mWebSocketsPort));
	settings.push_back(WebSocketsPort);

	XmlTree WebSocketsRoom("WebSocketsRoom", "");
	WebSocketsRoom.setAttribute("value", mWebSocketsRoom);
	settings.push_back(WebSocketsRoom);

	XmlTree WebSocketsNickname("WebSocketsNickname", "");
	WebSocketsNickname.setAttribute("value", mWebSocketsNickname);
	settings.push_back(WebSocketsNickname);

	XmlTree Info("Info", "");
	Info.setAttribute("value", toString(mInfo));
	settings.push_back(Info);

	XmlTree AssetsPath("AssetsPath", "");
	AssetsPath.setAttribute("value", toString(mAssetsPath));
	settings.push_back(AssetsPath);

	XmlTree UseAudio("UseAudio", "");
	UseAudio.setAttribute("value", mUseAudio);
	settings.push_back(UseAudio);

	XmlTree UseLineIn("UseLineIn", "");
	UseLineIn.setAttribute("value", mUseLineIn);
	settings.push_back(UseLineIn);

	XmlTree SplitWarpH("SplitWarpH", "");
	SplitWarpH.setAttribute("value", toString(mSplitWarpH));
	settings.push_back(SplitWarpH);

	XmlTree SplitWarpV("SplitWarpV", "");
	SplitWarpV.setAttribute("value", toString(mSplitWarpV));
	settings.push_back(SplitWarpV);

	// write XML file
	settings.write(writeFile(path));

	return true;
}

bool VDSettings::restore()
{
	// check to see if Settings.xml file exists
	fs::path params = getAppPath() / settingsFileName;
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");

			if (settings.hasChild("Standalone")) {
				XmlTree Standalone = settings.getChild("Standalone");
				mStandalone = Standalone.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AutoLayout")) {
				XmlTree AutoLayout = settings.getChild("AutoLayout");
				mAutoLayout = AutoLayout.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AspectRatio")) {
				XmlTree AspectRatio = settings.getChild("AspectRatio");
				mAspectRatio = AspectRatio.getAttributeValue<float>("value");
			}
			if (settings.hasChild("FboWidth")) {
				XmlTree FboWidth = settings.getChild("FboWidth");
				mFboWidth = FboWidth.getAttributeValue<int>("value");
			}
			if (settings.hasChild("FboHeight")) {
				XmlTree FboHeight = settings.getChild("FboHeight");
				mFboHeight = FboHeight.getAttributeValue<int>("value");
			}
			if (settings.hasChild("MIDIEnabled")) {
				XmlTree MIDIEnabled = settings.getChild("MIDIEnabled");
				mMIDIEnabled = MIDIEnabled.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCEnabled")) {
				XmlTree OSCEnabled = settings.getChild("OSCEnabled");
				mOSCEnabled = OSCEnabled.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCReceiverPort")) {
				XmlTree OSCReceiverPort = settings.getChild("OSCReceiverPort");
				mOSCReceiverPort = OSCReceiverPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("IsOSCSender")) {
				XmlTree IsOSCSender = settings.getChild("IsOSCSender");
				mIsOSCSender = IsOSCSender.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("OSCDestinationPort")) {
				XmlTree OSCDestinationPort = settings.getChild("OSCDestinationPort");
				mOSCDestinationPort = OSCDestinationPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost")) {
				XmlTree OSCDestinationHost = settings.getChild("OSCDestinationHost");
				mOSCDestinationHost = OSCDestinationHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("OSCDestinationPort2")) {
				XmlTree OSCDestinationPort2 = settings.getChild("OSCDestinationPort2");
				mOSCDestinationPort2 = OSCDestinationPort2.getAttributeValue<int>("value");
			}
			if (settings.hasChild("OSCDestinationHost2")) {
				XmlTree OSCDestinationHost2 = settings.getChild("OSCDestinationHost2");
				mOSCDestinationHost2 = OSCDestinationHost2.getAttributeValue<string>("value");
			}
			if (settings.hasChild("MIDIOpenAllInputPorts")) {
				XmlTree MIDIOpenAllInputPorts = settings.getChild("MIDIOpenAllInputPorts");
				mMIDIOpenAllInputPorts = MIDIOpenAllInputPorts.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("ShowUI")) {
				XmlTree ShowUI = settings.getChild("ShowUI");
				mShowUI = ShowUI.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("CursorVisible")) {
				XmlTree CursorVisible = settings.getChild("CursorVisible");
				mCursorVisible = CursorVisible.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("AreWebSocketsEnabledAtStartup")) {
				XmlTree AreWebSocketsEnabledAtStartup = settings.getChild("AreWebSocketsEnabledAtStartup");
				mAreWebSocketsEnabledAtStartup = AreWebSocketsEnabledAtStartup.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsRouter")) {
				XmlTree IsRouter = settings.getChild("IsRouter");
				mIsRouter = IsRouter.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("IsWebSocketsServer")) {
				XmlTree IsWebSocketsServer = settings.getChild("IsWebSocketsServer");
				mIsWebSocketsServer = IsWebSocketsServer.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("WebSocketsProtocol")) {
				XmlTree WebSocketsProtocol = settings.getChild("WebSocketsProtocol");
				mWebSocketsProtocol = WebSocketsProtocol.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsHost")) {
				XmlTree WebSocketsHost = settings.getChild("WebSocketsHost");
				mWebSocketsHost = WebSocketsHost.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsPort")) {
				XmlTree WebSocketsPort = settings.getChild("WebSocketsPort");
				mWebSocketsPort = WebSocketsPort.getAttributeValue<int>("value");
			}
			if (settings.hasChild("WebSocketsRoom")) {
				XmlTree WebSocketsRoom = settings.getChild("WebSocketsRoom");
				mWebSocketsRoom = WebSocketsRoom.getAttributeValue<string>("value");
			}
			if (settings.hasChild("WebSocketsNickname")) {
				XmlTree WebSocketsNickname = settings.getChild("WebSocketsNickname");
				mWebSocketsNickname = WebSocketsNickname.getAttributeValue<string>("value");
			}

			if (settings.hasChild("Info")) {
				XmlTree Info = settings.getChild("Info");
				mInfo = Info.getAttributeValue<string>("value");
			}
			if (settings.hasChild("AssetsPath")) {
				XmlTree AssetsPath = settings.getChild("AssetsPath");
				mAssetsPath = AssetsPath.getAttributeValue<string>("value");
				fs::path mPath = getAssetPath("") / mAssetsPath;
				if (!fs::exists(mPath)) {
					// reset path
					mAssetsPath = "";
				}
			}
			if (settings.hasChild("UseAudio")) {
				XmlTree UseAudio = settings.getChild("UseAudio");
				mUseAudio = UseAudio.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("UseLineIn")) {
				XmlTree UseLineIn = settings.getChild("UseLineIn");
				mUseLineIn = UseLineIn.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SplitWarpH")) {
				XmlTree SplitWarpH = settings.getChild("SplitWarpH");
				mSplitWarpH = SplitWarpH.getAttributeValue<bool>("value");
			}
			if (settings.hasChild("SplitWarpV")) {
				XmlTree SplitWarpV = settings.getChild("SplitWarpV");
				mSplitWarpV = SplitWarpV.getAttributeValue<bool>("value");
			}
			// if AutoLayout is false we have to read the custom screen layout
			if (mAutoLayout)
			{
				// init, overriden by GetWindowsResolution
				mMainWindowWidth = 1280;
				mMainWindowHeight = 720;
				mRenderWidth = 1280;
				mRenderHeight = 720;
				mRenderX = 1024;
				mRenderY = 0;
			}
			else
			{
				if (settings.hasChild("MainWindowWidth")) {
					XmlTree MainWindowWidth = settings.getChild("MainWindowWidth");
					mMainWindowWidth = MainWindowWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowHeight")) {
					XmlTree MainWindowHeight = settings.getChild("MainWindowHeight");
					mMainWindowHeight = MainWindowHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowX")) {
					XmlTree MainWindowX = settings.getChild("MainWindowX");
					mMainWindowX = MainWindowX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("MainWindowY")) {
					XmlTree MainWindowY = settings.getChild("MainWindowY");
					mMainWindowY = MainWindowY.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderWidth")) {
					XmlTree RenderWidth = settings.getChild("RenderWidth");
					mRenderWidth = RenderWidth.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderHeight")) {
					XmlTree RenderHeight = settings.getChild("RenderHeight");
					mRenderHeight = RenderHeight.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderX")) {
					XmlTree RenderX = settings.getChild("RenderX");
					mRenderX = RenderX.getAttributeValue<int>("value");
				}
				if (settings.hasChild("RenderY")) {
					XmlTree RenderY = settings.getChild("RenderY");
					mRenderY = RenderY.getAttributeValue<int>("value");
				}

			}
			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void VDSettings::resetSomeParams() {
	mCamPosXY = vec2(0.0f);
	mCount = 1;

	// EyePointZ
	defaultEyePointZ = -400.f;
	minEyePointZ = -800.0;
	maxEyePointZ = 0.0;
	tEyePointZ = autoEyePointZ = false;
	// unicorns
	iAlpha = 1.0f;
	iSpeedMultiplier = 1.0f;// = 0.0985f;// nearly ok for 160 = 0.0985f;
	iFade = mSendToOutput = iRepeat = false;

	// transition
	iTransition = 0;
	iAnim = 0.0;
	mTransitionDuration = 2.0f;
	mTransitionTime = 1.0f;

	autoInvert = false;
	// imgui
	uiMargin = 3;
	// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15 mPreviewWidth = 160;mPreviewHeight = 120;
	uiPreviewH = mPreviewHeight + uiMargin;
	uiLargeW = (mPreviewFboWidth + uiMargin) * 3;
	uiLargeH = (mPreviewFboHeight + uiMargin) * 6;
	uiLargePreviewW = mPreviewFboWidth * 1.7;
	uiLargePreviewH = mPreviewHeight * 2.8;

	uiXPosCol1 = uiLargeW + uiMargin * 2;
	uiXPosCol2 = uiXPosCol1 + uiMargin * 2 + mPreviewFboWidth;
	uiXPosCol3 = uiXPosCol2 + uiMargin;
	//uiYPos;
	uiYPosRow1 = 18;
	uiYPosRow2 = uiYPosRow1 + uiLargeH + uiMargin;
	uiYPosRow3 = uiYPosRow2 + uiLargePreviewH + uiMargin;
	uiYPosRow4 = uiYPosRow3 + uiLargePreviewH + uiMargin;
	uiYPosRow5 = uiYPosRow4 + uiLargePreviewH + uiMargin;
	uiYPosRow6 = uiYPosRow5 + uiLargePreviewH + uiMargin;
	
	uiSmallH = uiYPosRow2 -uiYPosRow1 - uiMargin;

	shaderEditIndex = 0;
}

void VDSettings::reset()
{
	// parameters exposed in XML
	mMIDIOpenAllInputPorts = mAutoLayout = mShowUI = mCursorVisible = true;
	mRenderThumbs = mStandalone = mSplitWarpH = mSplitWarpV = false;
	//mOutputVideoResolution = 1024;
	mInfo = "";
	mTrackName = "";
	iTrack = 0;
	mMainWindowX = mRenderX = 0;
	mMainWindowY = mRenderY = 0;
	mxLeft = 0;
	mxRight = 1280;
	myLeft = 0;
	myRight = 720;
	mMainWindowWidth = 1280;
	mMainWindowHeight = 720;
	mRenderWidth = 1280;
	mRenderHeight = 720;
	// parameters not exposed in XML
	mRenderXY = mTexMult = vec2(1.0f);
	mLeftRenderXY = mRightRenderXY = mPreviewRenderXY = mWarp1RenderXY = mWarp2RenderXY = vec2(0.0f);
	mRenderPosXY = vec2(0.0, 320.0);
	mPreviewFragXY = vec2(0.0, 0.0);
	mAspectRatio = 0.5625; // ratio 4:3 (0.75) 16:9 (0.5625)
	mFboWidth = 1280;
	mFboHeight = mFboWidth * mAspectRatio;
	mPreviewFboWidth = 70;
	mPreviewFboHeight = mPreviewFboWidth* mAspectRatio;
	// jpg / gif thumb for HeavyM ShaderLibrary
	mPreviewWidth = 120;
	mPreviewHeight = 68; // mPreviewWidth * mAspectRatio;
	mRenderCodeEditorXY.x = 0;
	mRenderCodeEditorXY.y = 0;
	mCodeEditorWidth = 800;
	mCodeEditorHeight = 600;
	mCurrentFilePath = "currentMix.frag";
	mAssetsPath = "";
	mMarginSmall = 2;
	mPreviewEnabled = true;
	//audio
	mIsPlaying = false;
	iSeed = 0.1;
	//liveMeter = 0.0f;

	// shader uniforms
	for (int i = 0; i < 4; i++)
	{
		iChannelTime[i] = i;
	}

	sFps = "60";
	iShowFps = true;

	currentSelectedIndex = 0;
	selectedWarp = 0;

	for (int a = 0; a < MAX; a++)
	{
		iChannels[a] = a;
	}
	iChannels[4] = 10;//spout
	selectedChannel = 0;
	// fbo indexes for warp (should be constants)
	mFboResolution = 2048;
	
	mWarpCount = 3;
	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	mLiveCode = false;
	mStateTransition = 1.0;
	mOptimizeUI = false;

	// initialize our camera
	mCamEyePointXY = vec2(0.f, 0.f);
	mCamEyePointZ = -400.f;
	mCamera.setEyePoint(vec3(mCamEyePointXY.x, mCamEyePointXY.y, mCamEyePointZ));
	//mCamera.setCenterOfInterestPoint(vec3(0.f, 0.f, 0.f));

	mUIZoom = 1;

	// midi
	mMIDIEnabled = true;

	// OSC
	mOSCEnabled = false;
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7001;
	mOSCDestinationHost2 = "127.0.0.1";
	mOSCDestinationPort2 = 7002;
	mOSCReceiverPort = 7000;
	mOSCMsg = "";
	mMsg = "";
	mErrorMsg = "";
	mWebSocketsMsg = "";
	mMidiMsg = "";
	mShaderMsg = "";
	mFboMsg = "";
	mIsOSCSender = false;
	/*xFade = 1.0f;
	xFadeChanged = false;*/
	// web sockets
	mIsRouter = false;
	mAreWebSocketsEnabledAtStartup = true;
	mIsWebSocketsServer = false;
	mWebSocketsProtocol = "ws://";
	mWebSocketsHost = "127.0.0.1";
	mWebSocketsPort = 8088;
	mWebSocketsRoom = "roomtest"; 
	mWebSocketsNickname = "bruce";
	// Blendmode 
	iBlendmode = 0;
	
	mDefaultVextexShaderString = "#version 150\n"
		"uniform mat4 ciModelViewProjection;\n"
		"in vec4 ciPosition;\n"
		"in vec4 ciColor;\n"
		"in vec2 ciTexCoord0;\n"
		"out vec4 vertColor;\n"
		"out vec2 vertTexCoord0;\n"
		"void main()\n"
		"{\n"
		"vertColor = ciColor;\n"
		"vertTexCoord0 = ciTexCoord0;\n"
		"gl_Position = ciModelViewProjection * ciPosition;\n"
		"}\n";
	mDefaultFragmentShaderString = "out vec4 fragColor;\n"
		"uniform sampler2D iChannel0;\n"
		"uniform sampler2D iChannel1;\n"
		"uniform sampler2D iChannel2;\n"
		"uniform sampler2D iChannel3;\n"
		"uniform sampler2D iChannel4;\n"
		"uniform sampler2D iChannel5;\n"
		"uniform sampler2D iChannel6;\n"
		"uniform sampler2D iChannel7;\n"
		"uniform sampler2D iChannel8;\n"
		"uniform sampler2D iChannel9;\n"
		"uniform vec3 iResolution;\n"
		"void main(void)\n"
		"{\n"
		"vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
		"vec3 t0 = texture(iChannel0, uv).rgb;\n"
		"vec3 t1 = 0.9*texture(iChannel1, uv).rgb;\n"
		"vec3 t2 = 0.8*texture(iChannel2, uv).rgb;\n"
		"vec3 t3 = 0.7*texture(iChannel3, uv).rgb;\n"
		"vec3 t4 = 0.6*texture(iChannel4, uv).rgb;\n"
		"vec3 t5 = 0.5*texture(iChannel5, uv).rgb;\n"
		"vec3 t6 = 0.4*texture(iChannel6, uv).rgb;\n"
		"vec3 t7 = 0.3*texture(iChannel7, uv).rgb;\n"
		"vec3 t8 = 0.2*texture(iChannel8, uv).rgb;\n"
		"vec3 t9 = 0.1*texture(iChannel9, uv).rgb;\n"
		"fragColor = vec4(t0 + t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9, 1.0);\n"
		"}\n";
	mHydraFragmentShaderString = "precision mediump float;\n"
		"varying vec2 uv;\n"
		"void main() {\n"
		"vec2 st = gl_FragCoord.xy/RENDERSIZE.xy;\n"
		"gl_FragColor = vec4(st.x,st.y,sin(TIME), 1.0);\n"
		"}\n";
	mMixetteFragmentShaderString = "uniform vec3      	iResolution;\n"
		"uniform sampler2D 	iChannel0;\n"
		"uniform sampler2D 	iChannel1;\n"
		"uniform sampler2D   iChannel2;\n"
		"uniform sampler2D   iChannel3;\n"
		"uniform sampler2D   iChannel4;\n"
		"uniform sampler2D   iChannel5;\n"
		"uniform sampler2D   iChannel6;\n"
		"uniform sampler2D   iChannel7;\n"
		"uniform float       iWeight0;\n"
		"uniform float       iWeight1;\n"
		"uniform float       iWeight2;\n"
		"uniform float       iWeight3;\n"
		"uniform float       iWeight4;\n"
		"uniform float       iWeight5;\n"
		"uniform float       iWeight6;\n"
		"uniform float       iWeight7;\n"
		"vec2  fragCoord = gl_FragCoord.xy;\n"
		"void main() {\n"
		"vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
		"vec3 c = texture(iChannel0, uv).xyz * iWeight0 + texture(iChannel1, uv).xyz * iWeight1 + texture(iChannel2, uv).xyz * iWeight2 + texture(iChannel3, uv).xyz * iWeight3 +\n"
		"texture(iChannel4, uv).xyz * iWeight4 + texture(iChannel4, uv).xyz * iWeight5 + texture(iChannel4, uv).xyz * iWeight6 + texture(iChannel4, uv).xyz * iWeight7;\n"
		"gl_FragColor = vec4(c.r, c.g, c.b, 1.0);\n"
		"}";
	//mPostFragmentShaderString;uv.y = 1.0 - uv.y;
	mPostFragmentShaderString = "uniform vec3 iResolution;\n"
		"uniform float TIME;\n"
		"uniform sampler2D 	iChannel0;\n"
		"uniform sampler2D 	iChannel1;\n"
		"uniform float		iExposure;\n"
		"uniform float		iSobel;\n"
		"uniform float		iChromatic;\n"
		"vec2  fragCoord = gl_FragCoord.xy; // keep the 2 spaces between vec2 and fragCoord\n"
		"float intensity(in vec4 c){return sqrt((c.x*c.x)+(c.y*c.y)+(c.z*c.z));}\n"
		"vec4 sobel(float stepx, float stepy, vec2 center) {\n"
		"float tleft = intensity(texture(iChannel0, center + vec2(-stepx, stepy)));\n"
		"float left = intensity(texture(iChannel0, center + vec2(-stepx, 0)));\n"
		"float bleft = intensity(texture(iChannel0, center + vec2(-stepx, -stepy)));\n"
		"float top = intensity(texture(iChannel0, center + vec2(0, stepy)));\n"
		"float bottom = intensity(texture(iChannel0, center + vec2(0, -stepy)));\n"
		"float tright = intensity(texture(iChannel0, center + vec2(stepx, stepy)));\n"
		"float right = intensity(texture(iChannel0, center + vec2(stepx, 0)));\n"
		"float bright = intensity(texture(iChannel0, center + vec2(stepx, -stepy)));\n"
		"float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;\n"
		"float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;\n"
		"return vec4(sqrt((x*x) + (y*y)));\n"
		"}\n"
		"vec4 chromatic(vec2 uv) {\n"
		"vec2 offset = vec2(iChromatic / 36., .0);\n"
		"return vec4(texture(iChannel0, uv + offset.xy).r, texture(iChannel0, uv).g, texture(iChannel0, uv + offset.yx).b, 1.0);\n"
		"}\n"
		"void main () {\n"
		"vec2 uv = gl_FragCoord.xy/iResolution.xy;\n"
		"uv.y = 1.0 - uv.y;\n"
		"vec4 t0 = texture(iChannel0, uv);\n"
		"vec4 c = vec4(0.0);\n"
		"if (iSobel > 0.03) { t0 = sobel(iSobel * 3.0 / iResolution.x, iSobel * 3.0 / iResolution.y, uv); }\n"
		"if (iChromatic > 0.0) { t0 = chromatic(uv) * t0; }\n"
		"c = t0;\n"
		"c *= iExposure;\n"
		"gl_FragColor = c; \n"
		"}\n";
	mMixFragmentShaderString = "#version 150\n"
		"uniform vec3 iResolution;\n"
		"uniform sampler2D iChannel0;\n"
		"uniform sampler2D iChannel1;\n"
		"uniform vec4      iMouse;\n"
		"uniform float     TIME;\n"
		"uniform vec3      iBackgroundColor;\n"
		"uniform vec3      iColor;\n"
		"uniform int       iSteps;\n"
		"uniform int       iFade;\n"
		"uniform int       iToggle;\n"
		"uniform int       iBlendmode;\n"
		"uniform int       iGreyScale;\n"
		"uniform int       iGlitch;\n"
		"uniform int       iTransition;\n"
		"uniform int       iRepeat;\n"
		"uniform int       iVignette;\n"
		"uniform int       iInvert;\n"
		"uniform float     iRatio;\n"

		"uniform float     iSobel;\n"

		"uniform float     iZoom;\n"
		"uniform float     iRotationSpeed;\n"
		"uniform float     iCrossfade;\n"
		"uniform float     iPixelate;\n"
		"uniform float     iAlpha;\n"
		"uniform float     iExposure;\n"
		"uniform float     iDeltaTime;\n"
		"uniform float     iTempoTime;\n"
		"uniform float     iChromatic;\n"
		"uniform float     iTrixels; // trixels if > 0.\n"
		"uniform float     iSeed;\n"
		"uniform float     iRedMultiplier;\n"
		"uniform float     iGreenMultiplier;\n"
		"uniform float     iBlueMultiplier;\n"
		"uniform float     iPixelX; // slitscan (or other) \n"
		"uniform float     iPixelY;\n"
		"uniform float     iBadTv; // badtv if > 0.01\n"
		"uniform float     iContour; // contour size if > 0.01\n"
		"uniform bool	   iFlipH;\n"
		"uniform bool	   iFlipV;\n"
		"uniform bool	   iXorY;\n"
		"uniform vec2	   iRenderXY;\n"
		"vec4 trixels( vec2 inUV, sampler2D tex )\n"
		"{ vec4 rtn;\n"
		"float height = iResolution.x/(1.01 - iTrixels)/90.0;\n"
		"float halfHeight = height*0.5;\n"
		"float halfBase = height/sqrt(3.0);\n"
		"float base = halfBase*2.0;\n"
		"float screenX = gl_FragCoord.x;\n"
		"float screenY = gl_FragCoord.y; \n"
		"float upSlope = height / halfBase;\n"
		"float downSlope = -height / halfBase;\n"
		"float oddRow = mod(floor(screenY / height), 2.0);\n"
		"screenX -= halfBase*oddRow;\n"
		"float oddCollumn = mod(floor(screenX / halfBase), 2.0);\n"
		"float localX = mod(screenX, halfBase);\n"
		"float localY = mod(screenY, height);\n"
		"if (oddCollumn == 0.0)\n"
		"{\n"
		"if (localY >= localX*upSlope)\n"
		"{\n"
		"screenX -= halfBase;\n"
		"}\n"
		"}\n"
		"else\n"
		"{\n"
		"	if (localY <= height + localX*downSlope)\n"
		"	{\n"
		"		screenX -= halfBase;\n"
		"	}\n"
		"}\n"
		"float startX = floor(screenX / halfBase)*halfBase;\n"
		"float startY = floor(screenY / height)*height;\n"
		"vec4 blend = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"for (float x = 0.0; x < 3.0; x += 1.0)\n"
		"{\n"
		"	for (float y = 0.0; y < 3.0; y += 1.0)\n"
		"	{\n"
		"		vec2 screenPos = vec2(startX + x*halfBase, startY + y*halfHeight);\n"
		"		vec2 uv1 = screenPos / iResolution.xy;\n"
		"		blend += texture(tex, uv1);\n"
		"	}\n"
		"}\n"
		"rtn = (blend / 9.0);\n"
		"return rtn;\n"
		"}\n"
		"float glitchHash(float x)\n"
		"{\n"
		"	return fract(sin(x * 11.1753) * 192652.37862);\n"
		"}\n"
		"float glitchNse(float x)\n"
		"{\n"
		"	float fl = floor(x);\n"
		"	return mix(glitchHash(fl), glitchHash(fl + 1.0), smoothstep(0.0, 1.0, fract(x)));\n"
		"}\n"
		"vec3 greyScale(vec3 colored)\n"
		"{\n"
		"	return vec3((colored.r + colored.g + colored.b) / 3.0);\n"
		"}\n"

			"float intensity(in vec4 c){return sqrt((c.x*c.x)+(c.y*c.y)+(c.z*c.z));}\n"
			"vec4 sobel0(float stepx, float stepy, vec2 center) {\n"
			"	float tleft = intensity(texture(iChannel0,center + vec2(-stepx,stepy)));\n"
			"	float left = intensity(texture(iChannel0,center + vec2(-stepx,0)));\n"
			"	float bleft = intensity(texture(iChannel0,center + vec2(-stepx,-stepy)));\n"
			"	float top = intensity(texture(iChannel0,center + vec2(0,stepy)));\n"
			"	float bottom = intensity(texture(iChannel0,center + vec2(0,-stepy)));\n"
			"	float tright = intensity(texture(iChannel0,center + vec2(stepx,stepy)));\n"
			"	float right = intensity(texture(iChannel0,center + vec2(stepx,0)));\n"
			"	float bright = intensity(texture(iChannel0,center + vec2(stepx,-stepy)));\n"
			"	float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;\n"
			"	float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;"
			"	return vec4(sqrt((x*x) + (y*y)));\n"
			"}\n"
			"vec4 sobel1(float stepx, float stepy, vec2 center) {\n"
			"	float tleft = intensity(texture(iChannel1,center + vec2(-stepx,stepy)));\n"
			"	float left = intensity(texture(iChannel1,center + vec2(-stepx,0)));\n"
			"	float bleft = intensity(texture(iChannel1,center + vec2(-stepx,-stepy)));\n"
			"	float top = intensity(texture(iChannel1,center + vec2(0,stepy)));\n"
			"	float bottom = intensity(texture(iChannel1,center + vec2(0,-stepy)));\n"
			"	float tright = intensity(texture(iChannel1,center + vec2(stepx,stepy)));\n"
			"	float right = intensity(texture(iChannel1,center + vec2(stepx,0)));\n"
			"	float bright = intensity(texture(iChannel1,center + vec2(stepx,-stepy)));\n"
			"	float x = tleft + 2.0*left + bleft - tright - 2.0*right - bright;\n"
			"	float y = -tleft - 2.0*top - tright + bleft + 2.0 * bottom + bright;"
			"	return vec4(sqrt((x*x) + (y*y)));\n"
			"}\n"

		"// left main lines begin\n"
		"vec3 shaderLeft(vec2 uv)\n"
		"{\n"
		"	vec4 left = texture(iChannel0, uv);\n"
		"	// chromatic aberration\n"
		"	if (iChromatic > 0.0)\n"
		"	{\n"
		"		vec2 offset = vec2(iChromatic / 50., .0);\n"
		"		left.r = texture(iChannel0, uv + offset.xy).r;\n"
		"		left.g = texture(iChannel0, uv).g;\n"
		"		left.b = texture(iChannel0, uv + offset.yx).b;\n"
		"	}\n"
			"	// sobel\n"
			"	if (iSobel > 0.03) { left = sobel0(iSobel * 3.0 /iResolution.x, iSobel * 3.0 /iResolution.y, uv); }\n"

		"	// Trixels\n"
		"	if (iTrixels > 0.0)\n"
		"	{\n"
		"		left = trixels(uv, iChannel0);\n"
		"	}\n"
		"	return vec3(left.r, left.g, left.b);\n"
		"}\n"
		"// left main lines end\n"
		"// right main lines begin\n"
		"vec3 shaderRight(vec2 uv)\n"
		"{ \n"
		"	vec4 right = texture(iChannel1, uv);\n"
		"	// chromatic aberation\n"
		"	if (iChromatic > 0.0)\n"
		"	{\n"
		"		vec2 offset = vec2(iChromatic / 50., .0);\n"
		"		right.r = texture(iChannel1, uv + offset.xy).r;\n"
		"		right.g = texture(iChannel1, uv).g;\n"
		"		right.b = texture(iChannel1, uv + offset.yx).b;\n"
		"	}\n"
			"	// sobel\n"
			"	if (iSobel > 0.03) { right = sobel1(iSobel * 3.0 /iResolution.x, iSobel * 3.0 /iResolution.y, uv); }\n"

		"	// Trixels\n"
		"	if (iTrixels > 0.0)\n"
		"	{\n"
		"		right = trixels(uv, iChannel1);\n"
		"	}\n"
		"	return vec3(right.r, right.g, right.b);\n"
		"}\n"
		"// Blend functions begin\n"
		"vec3 multiply(vec3 s, vec3 d)\n"
		"{\n"
		"	return s*d;\n"
		"}\n"
		"vec3 colorBurn(vec3 s, vec3 d)\n"
		"{\n"
		"	return 1.0 - (1.0 - d) / s;\n"
		"}\n"
		"vec3 linearBurn(vec3 s, vec3 d)\n"
		"{\n"
		"	return s + d - 1.0;\n"
		"}\n"
		"vec3 darkerColor(vec3 s, vec3 d)\n"
		"{\n"
		"	return (s.x + s.y + s.z < d.x + d.y + d.z) ? s : d;\n"
		"}\n"
		"vec3 lighten(vec3 s, vec3 d)\n"
		"{\n"
		"	return max(s, d);\n"
		"}\n"
		"vec3 darken(vec3 s, vec3 d)\n"
		"{\n"
		"	return min(s, d);\n"
		"}\n"
		"vec3 screen(vec3 s, vec3 d)\n"
		"{\n"
		"	return s + d - s * d;\n"
		"}\n"

		"vec3 colorDodge(vec3 s, vec3 d)\n"
		"{\n"
		"	return d / (1.0 - s);\n"
		"}\n"

		"vec3 linearDodge(vec3 s, vec3 d)\n"
		"{\n"
		"	return s + d;\n"
		"}\n"

		"vec3 lighterColor(vec3 s, vec3 d)\n"
		"{\n"
		"	return (s.x + s.y + s.z > d.x + d.y + d.z) ? s : d;\n"
		"}\n"

		"float overlay(float s, float d)\n"
		"{\n"
		"	return (d < 0.5) ? 2.0 * s * d : 1.0 - 2.0 * (1.0 - s) * (1.0 - d);\n"
		"}\n"

		"vec3 overlay(vec3 s, vec3 d)\n"
		"{\n"
		"	vec3 c;\n"
		"	c.x = overlay(s.x, d.x);\n"
		"	c.y = overlay(s.y, d.y);\n"
		"	c.z = overlay(s.z, d.z);\n"
		"	return c;\n"
		"}\n"

		"float softLight(float s, float d)\n"
		"{\n"
		"	return (s < 0.5) ? d - (1.0 - 2.0 * s) * d * (1.0 - d)\n"
		"		: (d < 0.25) ? d + (2.0 * s - 1.0) * d * ((16.0 * d - 12.0) * d + 3.0)\n"
		"		: d + (2.0 * s - 1.0) * (sqrt(d) - d);\n"
		"}\n"

		"vec3 softLight(vec3 s, vec3 d)\n"
		"{\n"
		"	vec3 c;\n"
		"	c.x = softLight(s.x, d.x);\n"
		"	c.y = softLight(s.y, d.y);\n"
		"	c.z = softLight(s.z, d.z);\n"
		"	return c;\n"
		"}\n"

		"float hardLight(float s, float d)\n"
		"{\n"
		"	return (s < 0.5) ? 2.0 * s * d : 1.0 - 2.0 * (1.0 - s) * (1.0 - d);\n"
		"}\n"

		"vec3 hardLight(vec3 s, vec3 d)\n"
		"{\n"
		"	vec3 c;\n"
		"	c.x = hardLight(s.x, d.x);\n"
		"	c.y = hardLight(s.y, d.y);\n"
		"	c.z = hardLight(s.z, d.z);\n"
		"	return c;\n"
		"}\n"

		"float vividLight(float s, float d)\n"
		"{\n"
		"	return (s < 0.5) ? 1.0 - (1.0 - d) / (2.0 * s) : d / (2.0 * (1.0 - s));\n"
		"}\n"

		"vec3 vividLight(vec3 s, vec3 d)\n"
		"{\n"
		"	vec3 c;\n"
		"	c.x = vividLight(s.x, d.x);\n"
		"	c.y = vividLight(s.y, d.y);\n"
		"	c.z = vividLight(s.z, d.z);\n"
		"	return c;\n"
		"}\n"

		"vec3 linearLight(vec3 s, vec3 d)\n"
		"{\n"
		"	return 2.0 * s + d - 1.0;\n"
		"}\n"

		"float pinLight(float s, float d)\n"
		"{\n"
		"	return (2.0 * s - 1.0 > d) ? 2.0 * s - 1.0 : (s < 0.5 * d) ? 2.0 * s : d;\n"
		"}\n"

		"vec3 pinLight(vec3 s, vec3 d)\n"
		"{\n"
		"	vec3 c;\n"
		"	c.x = pinLight(s.x, d.x);\n"
		"	c.y = pinLight(s.y, d.y);\n"
		"	c.z = pinLight(s.z, d.z);\n"
		"	return c;\n"
		"}\n"

		"vec3 hardMix(vec3 s, vec3 d)\n"
		"{\n"
		"	return floor(s + d);\n"
		"}\n"

		"vec3 difference(vec3 s, vec3 d)\n"
		"{\n"
		"	return abs(d - s);\n"
		"}\n"

		"vec3 exclusion(vec3 s, vec3 d)\n"
		"{\n"
		"	return s + d - 2.0 * s * d;\n"
		"}\n"

		"vec3 subtract(vec3 s, vec3 d)\n"
		"{\n"
		"	return s - d;\n"
		"}\n"

		"vec3 divide(vec3 s, vec3 d)\n"
		"{\n"
		"	return s / d;\n"
		"}\n"

		"// rgb<-->hsv functions by Sam Hocevar\n"
		"// http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl\n"
		"vec3 rgb2hsv(vec3 c)\n"
		"{\n"
		"	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
		"	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
		"	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"

		"	float d = q.x - min(q.w, q.y);\n"
		"	float e = 1.0e-10;\n"
		"	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
		"}\n"

		"vec3 hsv2rgb(vec3 c)\n"
		"{\n"
		"	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
		"	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
		"	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
		"}\n"

		"vec3 hue(vec3 s, vec3 d)\n"
		"{\n"
		"	d = rgb2hsv(d);\n"
		"	d.x = rgb2hsv(s).x;\n"
		"	return hsv2rgb(d);\n"
		"}\n"

		"vec3 color(vec3 s, vec3 d)\n"
		"{\n"
		"	s = rgb2hsv(s);\n"
		"	s.z = rgb2hsv(d).z;\n"
		"	return hsv2rgb(s);\n"
		"}\n"

		"vec3 saturation(vec3 s, vec3 d)\n"
		"{\n"
		"	d = rgb2hsv(d);\n"
		"	d.y = rgb2hsv(s).y;\n"
		"	return hsv2rgb(d);\n"
		"}\n"

		"vec3 luminosity(vec3 s, vec3 d)\n"
		"{\n"
		"	float dLum = dot(d, vec3(0.3, 0.59, 0.11));\n"
		"	float sLum = dot(s, vec3(0.3, 0.59, 0.11));\n"
		"	float lum = sLum - dLum;\n"
		"	vec3 c = d + lum;\n"
		"	float minC = min(min(c.x, c.y), c.z);\n"
		"	float maxC = max(max(c.x, c.y), c.z);\n"
		"	if (minC < 0.0) return sLum + ((c - sLum) * sLum) / (sLum - minC);\n"
		"	else if (maxC > 1.0) return sLum + ((c - sLum) * (1.0 - sLum)) / (maxC - sLum);\n"
		"	else return c;\n"
		"}\n"
		"// Blend functions end\n"

		"vec3 mainFunction(vec2 uv)\n"
		"{\n"
		"	vec3 c = vec3(0.0);\n"
		"	switch (iBlendmode)\n"
		"	{\n"
		"	case 0:\n"
		"		c = mix(shaderLeft(uv), shaderRight(uv), iCrossfade);\n"
		"		break;\n"
		"	case 1:\n"
		"		c = multiply(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 2:\n"
		"		c = colorBurn(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 3:\n"
		"		c = linearBurn(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 4:\n"
		"		c = darkerColor(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 5:\n"
		"		c = lighten(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 6:\n"
		"		c = screen(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 7:\n"
		"		c = colorDodge(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 8:\n"
		"		c = linearDodge(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 9:\n"
		"		c = lighterColor(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 10:\n"
		"		c = overlay(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 11:\n"
		"		c = softLight(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 12:\n"
		"		c = hardLight(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 13:\n"
		"		c = vividLight(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 14:\n"
		"		c = linearLight(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 15:\n"
		"		c = pinLight(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 16:\n"
		"		c = hardMix(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 17:\n"
		"		c = difference(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 18:\n"
		"		c = exclusion(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 19:\n"
		"		c = subtract(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 20:\n"
		"		c = divide(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 21:\n"
		"		c = hue(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 22:\n"
		"		c = color(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 23:\n"
		"		c = saturation(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 24:\n"
		"		c = luminosity(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 25:\n"
		"		c = darken(shaderLeft(uv), shaderRight(uv));\n"
		"		break;\n"
		"	case 26:\n"
		"		c = shaderLeft(uv);\n"
		"		break;\n"
		"	default: // in any other case.\n"
		"		c = shaderRight(uv);\n"
		"		break;\n"
		"	}\n"
		"	return c;\n"
		"}\n"
		"float BadTVResoRand(in float a, in float b) { return fract((cos(dot(vec2(a, b), vec2(12.9898, 78.233))) * 43758.5453)); }\n"
		"out vec4 oColor;\n"
		"void main(void)\n"
		"{\n"
		"vec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
		"// flip horizontally\n"
		"if (iFlipH)\n"
		"{\n"
		"	uv.x = 1.0 - uv.x;\n"
		"}\n"
		"// flip vertically\n"
		"if (iFlipV)\n"
		"{\n"
		"	uv.y = 1.0 - uv.y;\n"
		"}\n"
		"// zoom centered might invert flipH and V!\n"
		"float xZ = (uv.x - 0.5)*iZoom*2.0;\n"
		"float yZ = (uv.y - 0.5)*iZoom*2.0;\n"
		"vec2 cZ = vec2(xZ, yZ);\n"

		"// slitscan\n"
		"if (iRatio < 20.0)\n"
		"{\n"
		"	float x = gl_FragCoord.x;\n"
		"	float y = gl_FragCoord.y;\n"
		"	float x2 = x;\n"
		"	float y2 = y;\n"
		"	if (iXorY)\n"
		"	{\n"
		"		float z1 = floor((x / iPixelX) + 0.5);     //((x/20.0) + 0.5)\n"
		"		x2 = x + (sin(z1 + (TIME * 2.0)) * iRatio);\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		float z2 = floor((y / iPixelY) + 0.5);     //((x/20.0) + 0.5)\n"
		"		y2 = y + (sin(z2 + (TIME * 2.0)) * iRatio);\n"
		"	}\n"
		"\n"
		"	vec2 uv2 = vec2(x2 / iResolution.x, y2 / iResolution.y);\n"
		"	uv = texture(iChannel1, uv2).rg;\n"
		"}\n"
		"// glitch\n"
		"if (iGlitch == 1)\n"
		"{\n"
		"	// glitch the point around\n"
		"	float s = iTempoTime * iRatio;//50.0;\n"
		"	float te = iTempoTime * 9.0 / 16.0;//0.25 + (iTempoTime + 0.25) / 2.0 * 128.0 / 60.0;\n"
		"	vec2 shk = (vec2(glitchNse(s), glitchNse(s + 11.0)) * 2.0 - 1.0) * exp(-5.0 * fract(te * 4.0)) * 0.1;\n"
		"	uv += shk;\n"
		"}\n"
		"// pixelate\n"
		"if (iPixelate < 1.0)\n"
		"{\n"
		"	vec2 divs = vec2(iResolution.x * iPixelate / iResolution.y*60.0, iPixelate*60.0);\n"
		"	uv = floor(uv * divs) / divs;\n"
		"}\n"
		"vec3 col;\n"
		"if (iCrossfade > 0.99)\n"
		"{\n"
		"	col = shaderRight(uv - cZ);\n"
		"}\n"
		"else\n"
		"{\n"
		"	if (iCrossfade < 0.01)\n"
		"	{\n"
		"		col = shaderLeft(uv - cZ);\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		col = mainFunction(uv - cZ);\n"
		"	}\n"
		"}\n"
		"if (iToggle == 1)\n"
		"{\n"
		"	col.rgb = col.gbr;\n"
		"}\n"
		"col *= iExposure;\n"
		"if (iInvert == 1) col = 1. - col;\n"
		"// badtv\n"
		"if (iBadTv > 0.01)\n"
		"{\n"
		"	float c = 1.;\n"
		"	if (iXorY)\n"
		"	{\n"
		"		c += iBadTv * sin(TIME * 2. + uv.y * 100. * iPixelX);\n"
		"		c += iBadTv * sin(TIME * 1. + uv.y * 80.);\n"
		"		c += iBadTv * sin(TIME * 5. + uv.y * 900. * iPixelY);\n"
		"		c += 1. * cos(TIME + uv.x);\n"
		"	}\n"
		"	else\n"
		"	{\n"
		"		c += iBadTv * sin(TIME * 2. + uv.x * 100. * iPixelX);\n"
		"		c += iBadTv * sin(TIME * 1. + uv.x * 80.);\n"
		"		c += iBadTv * sin(TIME * 5. + uv.x * 900. * iPixelY);\n"
		"		c += 1. * cos(TIME + uv.y);\n"
		"	}\n"

		"	//vignetting\n"
		"	c *= sin(uv.x*3.15);\n"
		"	c *= sin(uv.y*3.);\n"
		"	c *= .9;\n"

		"	uv += TIME;\n"

		"	float r = BadTVResoRand(uv.x, uv.y);\n"
		"	float g = BadTVResoRand(uv.x * 9., uv.y * 9.);\n"
		"	float b = BadTVResoRand(uv.x * 3., uv.y * 3.);\n"

		"	col.x *= r*c*.35;\n"
		"	col.y *= b*c*.95;\n"
		"	col.z *= g*c*.35;\n"
		"}\n"

		"// grey scale mode\n"
		"if (iGreyScale == 1)\n"
		"{\n"
		"	col = greyScale(col);\n"
		"}\n"
		"col.r *= iRedMultiplier;\n"
		"col.g *= iGreenMultiplier;\n"
		"col.b *= iBlueMultiplier;\n"

		"// contour \n"
		"if (iContour> 0.01) {\n"
		"	if (uv.y > 1.0 - iContour)\n"
		"		col = iBackgroundColor;\n"
		"	if (uv.y < iContour)\n"
		"		col = iBackgroundColor;\n"
		"	if (uv.x > 1.0 - iContour)\n"
		"		col = iBackgroundColor;\n"
		"	if (uv.x < iContour)\n"
		"		col = iBackgroundColor;\n"
		"}\n"
		"oColor = iAlpha * vec4(col, 1.0);\n"
		"}\n";
	resetSomeParams();
}
