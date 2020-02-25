
/*{
	"CREDIT" : "Crepuscular light by toninoni",
	"CATEGORIES" : [
		"ci"
	],
	"DESCRIPTION": "",
	"INPUTS": [
		{
			"NAME": "inputImage",
			"TYPE" : "image"
		},
		{
			"NAME": "iDebug",
			"TYPE" : "bool",
			"DEFAULT" : 0.0
		},
		{
			"NAME": "iExposure",
			"TYPE" : "float",
			"MIN" : 0.1,
			"MAX" : 2.0,
			"DEFAULT" : 1.0
		},
		{
			"NAME": "iTimeFactor",
			"TYPE" : "float",
			"MIN" : 0.1,
			"MAX" : 2.0,
			"DEFAULT" : 1.0
		},
		{
			"NAME": "iBar",
			"TYPE" : "float",
			"MIN" : 0.1,
			"MAX" : 2.0,
			"DEFAULT" : 1.0
		},
		{
			"NAME": "iBpm",
			"TYPE" : "float",
			"MIN" : 0.1,
			"MAX" : 2.0,
			"DEFAULT" : 1.0
		},
		{
			"NAME": "iBarBeat",
			"TYPE" : "float",
			"MIN" : 2.0,
			"MAX" : 128.0,
			"DEFAULT" : 64.0
		},
		{
			"NAME": "iBeat",
			"TYPE" : "float",
			"MIN" : 0.01,
			"MAX" : 10.0,
			"DEFAULT" : 1.0
		},
		{
			"NAME" :"iMouse",
			"TYPE" : "point2D",
			"DEFAULT" : [0.0, 0.0],
			"MAX" : [640.0, 480.0],
			"MIN" : [0.0, 0.0]
		},
		{
			"NAME": "iColor", 
			"TYPE" : "color", 
			"DEFAULT" : [
				0.9, 
				0.6, 
				0.0, 
				1.0
			]
		}
	],
}
*/
// https://www.shadertoy.com/view/4sf3RN

float PI = 3.14159265358979586;

// Number Printing - @P_Malin
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

const float kCharBlank = 12.0;
const float kCharMinus = 11.0;
const float kCharDecimalPoint = 10.0;

// https://www.shadertoy.com/view/ls2Xzd
// Algorithm found in https://medium.com/community-play-3d/god-rays-whats-that-5a67f26aeac2

float SampleDigit(const in float fDigit, const in vec2 vUV)
{       
    if(vUV.x < 0.0) return 0.0;
    if(vUV.y < 0.0) return 0.0;
    if(vUV.x >= 1.0) return 0.0;
    if(vUV.y >= 1.0) return 0.0;
    
    // In this version, each digit is made up of a 4x5 array of bits
    
    float fDigitBinary = 0.0;
    
    if(fDigit < 0.5) // 0
    {
        fDigitBinary = 7.0 + 5.0 * 16.0 + 5.0 * 256.0 + 5.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 1.5) // 1
    {
        fDigitBinary = 2.0 + 2.0 * 16.0 + 2.0 * 256.0 + 2.0 * 4096.0 + 2.0 * 65536.0;
    }
    else if(fDigit < 2.5) // 2
    {
        fDigitBinary = 7.0 + 1.0 * 16.0 + 7.0 * 256.0 + 4.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 3.5) // 3
    {
        fDigitBinary = 7.0 + 4.0 * 16.0 + 7.0 * 256.0 + 4.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 4.5) // 4
    {
        fDigitBinary = 4.0 + 7.0 * 16.0 + 5.0 * 256.0 + 1.0 * 4096.0 + 1.0 * 65536.0;
    }
    else if(fDigit < 5.5) // 5
    {
        fDigitBinary = 7.0 + 4.0 * 16.0 + 7.0 * 256.0 + 1.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 6.5) // 6
    {
        fDigitBinary = 7.0 + 5.0 * 16.0 + 7.0 * 256.0 + 1.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 7.5) // 7
    {
        fDigitBinary = 4.0 + 4.0 * 16.0 + 4.0 * 256.0 + 4.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 8.5) // 8
    {
        fDigitBinary = 7.0 + 5.0 * 16.0 + 7.0 * 256.0 + 5.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 9.5) // 9
    {
        fDigitBinary = 7.0 + 4.0 * 16.0 + 7.0 * 256.0 + 5.0 * 4096.0 + 7.0 * 65536.0;
    }
    else if(fDigit < 10.5) // '.'
    {
        fDigitBinary = 2.0 + 0.0 * 16.0 + 0.0 * 256.0 + 0.0 * 4096.0 + 0.0 * 65536.0;
    }
    else if(fDigit < 11.5) // '-'
    {
        fDigitBinary = 0.0 + 0.0 * 16.0 + 7.0 * 256.0 + 0.0 * 4096.0 + 0.0 * 65536.0;
    }
    
    vec2 vPixel = floor(vUV * vec2(4.0, 5.0));
    float fIndex = vPixel.x + (vPixel.y * 4.0);
    
    return mod(floor(fDigitBinary / pow(2.0, fIndex)), 2.0);
}

float PrintVal(const in vec2 vStringCharCoords, const in float fValue, const in float fMaxDigits, const in float fDecimalPlaces)
{
    float fAbsValue = abs(fValue);
    
    float fStringCharIndex = floor(vStringCharCoords.x);
    
    float fLog10Value = log2(fAbsValue) / log2(10.0);
    float fBiggestDigitIndex = max(floor(fLog10Value), 0.0);
    
    // This is the character we are going to display for this pixel
    float fDigitCharacter = kCharBlank;
    
    float fDigitIndex = fMaxDigits - fStringCharIndex;
    if(fDigitIndex > (-fDecimalPlaces - 1.5))
    {
        if(fDigitIndex > fBiggestDigitIndex)
        {
            if(fValue < 0.0)
            {
                if(fDigitIndex < (fBiggestDigitIndex+1.5))
                {
                    fDigitCharacter = kCharMinus;
                }
            }
        }
        else
        {       
            if(fDigitIndex == -1.0)
            {
                if(fDecimalPlaces > 0.0)
                {
                    fDigitCharacter = kCharDecimalPoint;
                }
            }
            else
            {
                if(fDigitIndex < 0.0)
                {
                    // move along one to account for .
                    fDigitIndex += 1.0;
                }

                float fDigitValue = (fAbsValue / (pow(10.0, fDigitIndex)));

                // This is inaccurate - I think because I treat each digit independently
                // The value 2.0 gets printed as 2.09 :/
                //fDigitCharacter = mod(floor(fDigitValue), 10.0);
                fDigitCharacter = mod(floor(0.0001+fDigitValue), 10.0); // fix from iq
            }       
        }
    }

    vec2 vCharPos = vec2(fract(vStringCharCoords.x), vStringCharCoords.y);

    return SampleDigit(fDigitCharacter, vCharPos);  
}

float PrintValue(in vec2 ingl_FragCoord, const in vec2 vPixelCoords, const in vec2 vFontSize, const in float fValue, const in float fMaxDigits, const in float fDecimalPlaces)
{
    return PrintVal((ingl_FragCoord.xy - vPixelCoords) / vFontSize, fValue, fMaxDigits, fDecimalPlaces);
}
vec4 crepuscular_rays(vec2 texCoords, vec2 pos) {

    float decay = 0.98;
    float density = 1.0;
    float weight = 0.58767;
    // NUM_SAMPLES will describe the rays quality, you can play with
    const int nsamples = 150;

    vec2 tc = texCoords.xy;
    vec2 deltaTexCoord = tc - pos.xy; //* sin(TIME)
    deltaTexCoord *= (1.0 / float(nsamples) * density);
    float illuminationDecay = 1.0;

    vec4 color = texture(inputImage, tc.xy) * vec4(0.9);
	
    tc += deltaTexCoord * fract( sin(dot(texCoords.xy+fract(TIME), vec2(12.9898, 78.233)))* 43758.5453 );
    for (int i = 0; i < nsamples; i++)
	{
        tc -= deltaTexCoord;
        vec4 sampl = texture(inputImage, tc.xy) * vec4(0.2); 

        sampl *= illuminationDecay * weight;

		// ok with iExposure= 1.93 TIME/45.0
		color += sampl * (sin( TIME / 45.0 * iBpm * iTimeFactor * 2 * PI ) + iExposure - 1.0);
        illuminationDecay *= decay;
    }
    
    return color;
}
void main( void ){
	vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;
	//uv.x *= RENDERSIZE.x/RENDERSIZE.y; //fix aspect ratio
    //vec3 pos = vec3(iMouse.xy/RENDERSIZE.xy - 0.5,iMouse.z-.5);
	//vec3 pos = vec3(iMouse.xy/RENDERSIZE.xy + vec2(iMouse.x, iMouse.y),iMouse.z-.5);
	//vec3 pos = vec3(iMouse.xy/RENDERSIZE.xy + vec2(0.32, 0.530),iMouse.z-.5);
	//vec3 pos = vec3(iMouse.xy/RENDERSIZE.xy + vec2(0.3, 0.530),iMouse.z-.5);
	vec3 pos = vec3(iMouse.xy/RENDERSIZE.xy + vec2(iMouse.x, iMouse.y),iMouse.z-.5);
	if (iMouse.z>.5) {
		pos.x=sin(TIME*.09)*.95;
		pos.y=sin(TIME*.09)*.95;
	}
	pos.x *= RENDERSIZE.x/RENDERSIZE.y; //fix aspect ratio
	// Multiples of 4x5 work best
    vec2 vFontSize = vec2(20.0, 40.0);//vec2(8.0, 15.0);
	if (iDebug) {
		vec4 vColour = vec4(0.7);
		vColour = mix( vColour, vec4(1.0, 1.0, 0.0, 0.0), PrintValue(gl_FragCoord.xy, vec2(0.0, 10.0), vFontSize, sin( TIME / 60.0 * iBpm * iTimeFactor * PI ) + iExposure - 1.0, 3.0, 2.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(150.0, 10.0), vFontSize, TIME, 2.0, 2.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(250.0, 10.0), vFontSize, iBeat, 2.0, 0.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(350.0, 10.0), vFontSize, iBar, 2.0, 0.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(450.0, 10.0), vFontSize, iBarBeat, 2.0, 0.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(550.0, 10.0), vFontSize, iExposure, 2.0, 2.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(750.0, 10.0), vFontSize, iTimeFactor, 2.0, 3.0));
		vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(970.0, 10.0), vFontSize, iBpm, 2.0, 2.0));
		fragColor = mix( vec4(1.0, 1.0, 1.0, 0.0), crepuscular_rays(uv, pos.xy), vColour);
	} else {
		fragColor = crepuscular_rays(uv, pos.xy);
	}
}
