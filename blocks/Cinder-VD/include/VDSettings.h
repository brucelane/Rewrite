/*
	VDSettings
	Global settings for the app
*/
// TODO switch from xml to json
// TODO remove public
// TODO create enum

#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Timeline.h"
#include "cinder/Xml.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm {

	typedef std::shared_ptr<class VDSettings> VDSettingsRef;

	class VDSettings
	{
	public:
		VDSettings(string filename);
		static VDSettingsRef create(string filename);

		//! maximum number of fbos, shaders, textures
		static const int			MAX = 14;
		// uniform indexes
		// float
		static const int			ITIME = 0;
		static const int			IFR = 1;
		static const int			IFG = 2;
		static const int			IFB = 3;
		static const int			IFA = 4;
		static const int			IFRX = 5;
		static const int			IFGX = 6;
		static const int			IFBX = 7;
		static const int			ISOBEL = 8;
		static const int			ISTEPS = 10;
		static const int			IRATIO = 11;
		static const int			IZOOM = 12;
		static const int			IAUDIOX = 13;
		static const int			IEXPOSURE = 14;
		static const int			IPIXELATE = 15;
		static const int			ITRIXELS = 16;
		static const int			ICHROMATIC = 17;
		static const int			IXFADE = 18;
		static const int			ITEMPOTIME = 19;
		static const int			IFPS = 20;

		static const int			IBPM = 21;
		static const int			ISPEED = 22;
		static const int			IPIXELX = 23;
		static const int			IPIXELY = 24;
		static const int			IDELTATIME = 25;
		static const int			IBR = 26;
		static const int			IBG = 27;
		static const int			IBB = 28;
		static const int			IRESX = 29;
		static const int			IRESY = 30;
		static const int			IWEIGHT0 = 31;
		static const int			IWEIGHT1 = 32;
		static const int			IWEIGHT2 = 33;
		static const int			IWEIGHT3 = 34;
		static const int			IWEIGHT4 = 35;
		static const int			IWEIGHT5 = 36;
		static const int			IWEIGHT6 = 37;
		static const int			IWEIGHT7 = 38;
		static const int			IWEIGHT8 = 39; // 20200304 was IELAPSED

		static const int			ICONTOUR = 40;
		static const int			IROTATIONSPEED = 41;
		static const int			IMOUSEX = 42;
		static const int			IMOUSEY = 43;
		static const int			IMOUSEZ = 44;
		static const int			IVAMOUNT = 45;
		static const int			IVFALLOFF = 46;
		static const int			TIME = 47;
		//static const int			IPHASE = 48;
		static const int			ITIMEFACTOR = 49;//iTimeFactor
		
		// int
		static const int			IBLENDMODE = 50;
		static const int			IBEAT = 51;
		static const int			IBAR = 52;
		static const int			IBARBEAT = 53;
		static const int			IFBOA = 54;
		static const int			IFBOB = 55;
		static const int			IOUTW = 56;
		static const int			IOUTH = 57;	
		static const int			IBEATSPERBAR = 59;
		static const int			IRESOLUTION = 60;
		static const int			ICOLOR = 61;
		static const int			IBACKGROUNDCOLOR = 62;
		static const int			IMOUSE = 70;
		// booleans 80 to 110
		static const int			IGLITCH = 81;	// A4
		static const int			IVIGN = 82;		// A#4
		static const int			ITOGGLE = 83;	// B4 for sos
		static const int			IINVERT = 86;	// D5
		static const int			IGREYSCALE = 87;
		static const int			ICLEAR = 88;	// 

		
		static const int			IDEBUG = 129;
		static const int			IXORY = 130;
		static const int			IFLIPH = 131;
		static const int			IFLIPV = 132;
		static const int			IFLIPPOSTH = 133;
		static const int			IFLIPPOSTV = 134;		
		
		/*
		static const int			IRHANDX = 110;
		static const int			IRHANDY = 111;
		static const int			IRHANDZ = 112;
		static const int			ILHANDX = 113;
		static const int			ILHANDY = 114;
		static const int			ILHANDZ = 115;*/

		static const int			IFREQ0 = 140;
		static const int			IFREQ1 = 141;
		static const int			IFREQ2 = 142;
		static const int			IFREQ3 = 143;
		// vec2
		static const int			RESOLUTION = 150; // vec2 for hydra
		static const int			RENDERSIZE = 151; // vec2 for ISF
		// srcarea
		static const int			SRCXLEFT = 160;
		static const int			SRCXRIGHT = 161;
		static const int			SRCYLEFT = 162;
		static const int			SRCYRIGHT = 163;		
		// texture modes
		static const int			TEXTUREMODEMIX = 0;				// mix two shaders
		static const int			TEXTUREMODEAUDIO = 1;			// audio spectrum
		static const int			TEXTUREMODELEFT = 2;			// left
		static const int			TEXTUREMODERIGHT = 3;			// right
		static const int			TEXTUREMODEINPUT = 4;			// spout
		static const int			TEXTUREMODESHADER = 5;			// shader
		static const int			TEXTUREMODEIMAGE = 6;			// image
		static const int			TEXTUREMODEIMAGESEQUENCE = 7;	// image sequence
		static const int			TEXTUREMODETHUMB = 8;			// thumb
		static const int			TEXTUREMODEFBO = 9;				// fbo
		static const int			TEXTUREMODETEXT = 10;			// text
		static const int			TEXTUREMODEMOVIE = 11;			// movie
		bool						save();
		bool						restore();
		void						reset();
		void						resetSomeParams();

		// params
		int							mPreviewWidth, mPreviewHeight, mPreviewFboWidth, mPreviewFboHeight;
		float						mAspectRatio;
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight, mFboWidth, mFboHeight;
		float						mCodeEditorWidth, mCodeEditorHeight;
		// MIDI
		bool						mMIDIEnabled;
		// OSC
		bool						mOSCEnabled;
		std::string					mOSCDestinationHost;
		uint16_t					mOSCDestinationPort;
		std::string					mOSCDestinationHost2;
		uint16_t					mOSCDestinationPort2;
		uint16_t					mOSCReceiverPort;
		std::string					mOSCMsg;
		std::string					mMsg;
		std::string					mErrorMsg;
		std::string					mShaderMsg;
		std::string					mFboMsg;
		bool						mIsOSCSender;
		static const int			mMsgLength = 150;

		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		vec2						mRenderXY, mTexMult, mLeftRenderXY, mRightRenderXY, mPreviewRenderXY, mWarp1RenderXY, mWarp2RenderXY;
		vec2						mRenderPosXY;
		vec2						mPreviewFragXY;
		vec2						mCamPosXY;
		int							mxLeft;
		int							mxRight;
		int							myLeft;
		int							myRight;
		bool						mAutoLayout;
		bool						mStandalone;

		// code editor
		vec2						mRenderCodeEditorXY;
		bool						mLiveCode;
		bool						mShowUI;
		bool						mCursorVisible;
		bool						isUIDirty;
		bool						mMIDIOpenAllInputPorts;
		int							mCount;
		string						mImageFile;

		// shader uniforms	
		float						iSpeedMultiplier;   // speed multiplier
		float						iStart = 1.0f;		// start adjustment
		float						iBarDuration = 1.0f;// between 2 bars (seconds)
		float						iChannelTime[4];
		bool						iFade;
		bool						iRepeat;
		int							iBlendmode;
		ci::Anim<float> 			iAlpha;

		string						mAssetsPath;
		bool						iShowFps;
		string						sFps;
		unsigned int				shaderEditIndex;
		// transition
		int							iTransition;
		Anim<float>					iAnim;
		double						mTransitionDuration;
		//! Time in seconds at which the transition to the next shader starts.
		double						mTransitionTime;

		// windows and params
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mPreviewEnabled;
		string						mCurrentFilePath;
		// Textures
		bool						mRenderThumbs;
		int							currentSelectedIndex;

		// modes, should be the same in App
		static const int			MODE_MIX = 0;
		static const int			MODE_IMAGE = 1;
		static const int			MODE_SHARED = 2;
		static const int			MODE_STREAM = 3;
		static const int			MODE_SHADER = 4;
		static const int			MODE_HYDRA = 5;

		/* static const int			MODE_WARP = 1;
		static const int			MODE_AUDIO = 2;
		static const int			MODE_SPHERE = 3;
		static const int			MODE_MESH = 4;
		static const int			MODE_LIVE = 5;
		static const int			MODE_ABP = 6;
		static const int			MODE_KINECT = 8; */

		// windows to create, should be the same in App and UI
		/*static const int			NONE = 0;
		static const int			RENDER_1 = 1;
		static const int			RENDER_DELETE = 5;
		static const int			MIDI_IN = 6;*/
		
		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		// audio
		bool						mIsPlaying;
		bool						mUseAudio;
		bool						mUseLineIn;
		float						iSeed;

		// z EyePointZ
		float						defaultEyePointZ;
		float						minEyePointZ;
		float						maxEyePointZ;
		bool						tEyePointZ;
		bool						autoEyePointZ;

		ci::Anim<float>				mStateTransition;

		// 
		bool						mSendToOutput;
		bool						autoInvert;

		// spout
		int							mFboResolution;
		// indexes for textures
		map<int, int>				iChannels;
		int							selectedChannel;
		int							selectedWarp;
		int							mWarpCount;
		bool						mOptimizeUI;
		bool						mSplitWarpH;
		bool						mSplitWarpV;
		int							mUIZoom;
		int							mCurrentPreviewFboIndex;
		int							iTrack;
				
		// camera
		CameraPersp					mCamera;
		vec2						mCamEyePointXY;
		float						mCamEyePointZ;
		// web sockets
		bool						mIsRouter;
		bool						mAreWebSocketsEnabledAtStartup;
		bool						mIsWebSocketsServer;
		string						mWebSocketsProtocol;
		string						mWebSocketsHost;
		string						mWebSocketsRoom;
		string						mWebSocketsNickname;
		uint16_t					mWebSocketsPort;
		std::string					mWebSocketsMsg;
		// midi
		std::string					mMidiMsg;

		// info to backup in XML
		string						mInfo;
		// ableton liveOSC
		string						mTrackName;
		// imgui positions and sizes
		int							uiMargin;
		// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15
		int							uiXPosCol1;
		int							uiXPosCol2;
		int							uiXPosCol3;
		//int							uiYPos;
		int							uiYPosRow1; // top
		int							uiYPosRow2; // details, textures
		int							uiYPosRow3;
		int							uiYPosRow4;
		int							uiYPosRow5;
		int							uiYPosRow6;
		int							uiLargeW;
		int							uiLargeH;
		int							uiSmallH;
		int							uiLargePreviewW;
		int							uiLargePreviewH;
		int							uiPreviewH;
		std::string					getDefaultVextexShaderString() { return mDefaultVextexShaderString; };
		std::string					getDefaultFragmentShaderString() { return mDefaultFragmentShaderString; };
		std::string					getMixFragmentShaderString() { return mMixFragmentShaderString; };
		std::string					getHydraFragmentShaderString() { return mHydraFragmentShaderString; };
		std::string					getMixetteFragmentShaderString() { return mMixetteFragmentShaderString; };
		std::string					getPostFragmentShaderString() { return mPostFragmentShaderString; };
	private:
		std::string					settingsFileName = "VD";
		//! default vertex shader
		std::string					mDefaultVextexShaderString;
		std::string					mDefaultFragmentShaderString;
		std::string					mMixFragmentShaderString; 
		std::string					mHydraFragmentShaderString;
		std::string					mMixetteFragmentShaderString;
		std::string					mPostFragmentShaderString;

	};

}
