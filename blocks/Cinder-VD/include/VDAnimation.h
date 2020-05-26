/*
	VDAnimation
	Handles all uniform variables for shaders: index, name, type, bounds and their animation.
*/
// TODO put audio in a separate class
// TODO put timing in separate class?
// TODO Implement a double map to replace map<int, string> controlIndexes and map<string, VDUniform> shaderUniforms
// TODO remove struct
// TODO implement lazy loading for audio

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
//!  audio
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/Rand.h"
//!  json
#include "cinder/Json.h"
//!  Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	//enum class UniformTypes { FLOAT, SAMPLER2D, VEC2, VEC3, VEC4, INT, BOOL };

	struct VDUniform
	{
		int								uniformType;
		int								index;
		float							defaultValue;
		float							floatValue;
		bool							boolValue;
		int								intValue;
		vec2							vec2Value;
		vec3							vec3Value;
		vec4							vec4Value;
		float							minValue;
		float							maxValue;
		bool							autotime;
		bool							automatic;
		bool							autobass;
		bool							automid;
		bool							autotreble;
		int								textureIndex;
		bool							isValid;
	};

	class VDAnimation {
	public:
		VDAnimation(VDSettingsRef aVDSettings);

		static VDAnimationRef			create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDAnimation>(new VDAnimation(aVDSettings));
		}
		void							update();
		void							load();
		void							save();
		string							getAssetsPath() {
			return mVDSettings->mAssetsPath;
		}
		/*Color							getBackgroundColor() { return mBackgroundColor; };
		float							getExposure() { return mExposure; };
		bool							getAutoBeatAnimation() { return mAutoBeatAnimation; };
		void							setExposure(float aExposure);
		void							setAutoBeatAnimation(bool aAutoBeatAnimation);*/

		const int						mBlendModes = 28;
		void							blendRenderEnable(bool render) { mBlendRender = render; };
		// tap tempo
		bool							mUseTimeWithTempo;
		void							toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; };
		void							useTimeWithTempo() { mUseTimeWithTempo = true; };
		bool							getUseTimeWithTempo() { return mUseTimeWithTempo; };
		float							iTempoTimeBeatPerBar;
		float							getBpm() {
			return getFloatUniformValueByIndex(mVDSettings->IBPM);
		};
		void							setBpm(float aBpm) {
			if (aBpm > 0.0f) {
				setFloatUniformValueByIndex(mVDSettings->IBPM, aBpm);
				setFloatUniformValueByIndex(mVDSettings->IDELTATIME, 60 / aBpm);
			}
		};
		void							tapTempo();
		int								getEndFrame() { return mEndFrame; };
		void							setEndFrame(int frame) { mEndFrame = frame; };
		int								mLastBeat = 0;
		// animation
		int								currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool							handleKeyDown(KeyEvent &event);
		bool							handleKeyUp(KeyEvent &event);
		// audio
		ci::gl::TextureRef				getAudioTexture();
		string							getAudioTextureName() { return mAudioName; };
		float							maxVolume;
		bool							mLineInInitialized;
		bool							mWaveInitialized;
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;
		audio::SamplePlayerNodeRef		mSamplePlayerNode;
		audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;
		audio::BufferPlayerNodeRef		mBufferPlayerNode;

		vector<float>					mMagSpectrum;

		// number of frequency bands of our spectrum
		static const int				mFFTWindowSize = 32; // fft window size 20200222 was 128;
		float							iFreqs[mFFTWindowSize];
		int								mPosition;
		string							mAudioName;
		void							preventLineInCrash(); // at next launch
		void							saveLineIn();
		bool							getUseAudio() {
			return mVDSettings->mUseAudio;
		};
		bool							getUseLineIn() {
			return mVDSettings->mUseLineIn;
		};
		void							setUseLineIn(bool useLineIn) {
			mVDSettings->mUseLineIn = useLineIn;
		};
		void							toggleUseLineIn() { mVDSettings->mUseLineIn = !mVDSettings->mUseLineIn; };

		// audio
		bool							isAudioBuffered() { return mAudioBuffered; };
		void							toggleAudioBuffered() { mAudioBuffered = !mAudioBuffered; };

		// shaders
		bool							isExistingUniform(const string& aName);
		int								getUniformType(const string& aName);
		string							getUniformNameForIndex(int aIndex) {
			return controlIndexes[aIndex];
		};
		int								getUniformIndexForName(const string& aName) {
			return shaderUniforms[aName].index;
		};
		bool							toggleAuto(unsigned int aIndex);
		bool							toggleValue(unsigned int aIndex);
		bool							toggleTempo(unsigned int aIndex);
		bool							toggleBass(unsigned int aIndex);
		bool							toggleMid(unsigned int aIndex);
		bool							toggleTreble(unsigned int aIndex);
		void							resetAutoAnimation(unsigned int aIndex);
		bool							setFloatUniformValueByIndex(unsigned int aIndex, float aValue);

		bool							setBoolUniformValueByIndex(unsigned int aIndex, bool aValue) {
			if (getUniformNameForIndex(aIndex) == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[getUniformNameForIndex(aIndex)].boolValue = aValue;
			}
			return aValue;
		}
		void							setIntUniformValueByName(const string& aName, int aValue) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[aName].intValue = aValue;
			}
		};
		void							setIntUniformValueByIndex(unsigned int aIndex, int aValue) {
			if (getUniformNameForIndex(aIndex) == "") {
				CI_LOG_V("empty error");
			}
			else {
				if (mVDSettings->IBEAT == aIndex) {
					if (aValue != mLastBeat) {
						mLastBeat = aValue;
						if (aValue == 0) setIntUniformValueByIndex(mVDSettings->IBAR, getIntUniformValueByIndex(mVDSettings->IBAR) + 1);
					}
				}
				shaderUniforms[getUniformNameForIndex(aIndex)].intValue = aValue;
			}
		}
		void							setFloatUniformValueByName(const string& aName, float aValue) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[aName].floatValue = aValue;
			}
		}
		void setVec2UniformValueByName(const string& aName, vec2 aValue) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[aName].vec2Value = aValue;
			}
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec2Value = aValue;
		}
		void setVec3UniformValueByName(const string& aName, vec3 aValue) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[aName].vec3Value = aValue;
			}
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec3Value = aValue;
		}
		void setVec4UniformValueByName(const string& aName, vec4 aValue) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			else {
				shaderUniforms[aName].vec4Value = aValue;
			}
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			shaderUniforms[getUniformNameForIndex(aIndex)].vec4Value = aValue;
		}
		bool							getBoolUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].boolValue;
		}
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].minValue;
		}
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].maxValue;
		}
		float							getMinUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			
				return shaderUniforms[aName].minValue;
		}
		float							getMaxUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}
			
			return shaderUniforms[aName].maxValue;
		}


		bool							getBoolUniformValueByName(const string& aName) {
			return shaderUniforms[aName].boolValue;
		}
		vec2							getVec2UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec2Value;
		};
		vec3							getVec3UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec3Value;
		};
		vec4							getVec4UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].vec4Value;
		};
		float							getFloatUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].floatValue;
		}
		float							getFloatUniformDefaultValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].defaultValue;
		}
		int								getIntUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[getUniformNameForIndex(aIndex)].intValue;
		}
		int								getSampler2DUniformValueByName(const string& aName) {
			return shaderUniforms[aName].textureIndex;
		}
		float							getFloatUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("getFloatUniformValueByName name empty");
				return 1.0f;
			}
			else {
				return shaderUniforms[aName].floatValue;
			}
		}
		vec2							getVec2UniformValueByName(const string& aName) {
			return shaderUniforms[aName].vec2Value;
		}
		vec3							getVec3UniformValueByName(const string& aName) {
			return shaderUniforms[aName].vec3Value;
		}
		vec4							getVec4UniformValueByName(const string& aName) {
			return shaderUniforms[aName].vec4Value;
		}
		int								getIntUniformValueByName(const string& aName) {
			return shaderUniforms[aName].intValue;
		};

		// mix fbo
		bool							isFlipH() { return getBoolUniformValueByIndex(mVDSettings->IFLIPH); };
		bool							isFlipV() { return getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		void							flipH() { setBoolUniformValueByIndex(mVDSettings->IFLIPH, !getBoolUniformValueByIndex(mVDSettings->IFLIPH)); };
		void							flipV() { setBoolUniformValueByIndex(mVDSettings->IFLIPV, !getBoolUniformValueByIndex(mVDSettings->IFLIPV)); };

		unsigned int					getBlendModesCount() { return mBlendModes; };
		bool							renderBlend() { return mBlendRender; };

		// timed animation
		int								mEndFrame;
		int								getFreqIndexSize() { return freqIndexes.size(); };
		int								getFreqIndex(unsigned int aFreqIndex) { return freqIndexes[aFreqIndex]; };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { freqIndexes[aFreqIndex] = aFreq; };
		//float							getFreq(unsigned int aFreqIndex) { return iFreqs[freqIndexes[aFreqIndex]]; };
		// public for hydra
		void							createFloatUniform(const string& aName, int aCtrlIndex, float aValue = 0.01f, float aMin = 0.0f, float aMax = 1.0f);
		void							createSampler2DUniform(const string& aName, int aCtrlIndex, int aTextureIndex = 0);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		map<int, int>					freqIndexes;
		bool							mAudioBuffered;
		ci::gl::TextureRef				mAudioTexture;
		gl::Texture2d::Format			mAudioFormat;
		unsigned char					dTexture[256];// MUST be < mWindowSize
		// Live json params
		/*fs::path						mJsonFilePath;
		Parameter<Color>				mBackgroundColor;
		Parameter<float>				mExposure;
		Parameter<string>				mText;
		Parameter<bool>					mAutoBeatAnimation;*/
		// shaders
		map<int, string>				controlIndexes;
		map<string, VDUniform>			shaderUniforms;
		//! read a uniforms json file 
		void							loadUniforms(const ci::DataSourceRef &source);
		void							floatFromJson(const ci::JsonTree &json);
		void							sampler2dFromJson(const ci::JsonTree &json);
		void							vec2FromJson(const ci::JsonTree &json);
		void							vec3FromJson(const ci::JsonTree &json);
		void							vec4FromJson(const ci::JsonTree &json);
		void							intFromJson(const ci::JsonTree &json);
		void							boolFromJson(const ci::JsonTree &json);
		fs::path						mUniformsJson;
		void							createVec2Uniform(const string& aName, int aCtrlIndex, vec2 aValue = vec2(0.0));
		void							createVec3Uniform(const string& aName, int aCtrlIndex, vec3 aValue = vec3(0.0));
		void							createVec4Uniform(const string& aName, int aCtrlIndex, vec4 aValue = vec4(0.0));
		void							createIntUniform(const string& aName, int aCtrlIndex, int aValue = 1);
		void							createBoolUniform(const string& aName, int aCtrlIndex, bool aValue = false);
		//! write a uniforms json file
		void							saveUniforms();
		ci::JsonTree					uniformToJson(int i);

		// time
		ci::Timer						mTimer;
		std::deque <double>				buffer;
		void							calculateTempo();
		int								counter;
		double							averageTime;
		double							currentTime;
		double							startTime;
		float							previousTime;
		float							previousTimeBeatPerBar;
		JsonTree						mData;
		void							loadAnimation();
		void							saveAnimation();
		int								mLastBar = 0;
		std::unordered_map<int, float>	mBadTV;
		bool							mBlendRender;

	};
}
