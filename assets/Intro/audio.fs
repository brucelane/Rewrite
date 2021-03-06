/*{
	"CREDIT" : "ClonedRainbowSpectrum by Hornet",
	"CATEGORIES" : [
		"ci"
	],
	"DESCRIPTION": "",
	"INPUTS": [
		{
			"NAME": "inputImage",
			"TYPE" : "image"
		}
	],
}
*/

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

float PrintValue(const in vec2 vStringCharCoords, const in float fValue, const in float fMaxDigits, const in float fDecimalPlaces)
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
    return PrintValue((ingl_FragCoord.xy - vPixelCoords) / vFontSize, fValue, fMaxDigits, fDecimalPlaces);
}
float RainbowBump(float x) {
	return abs(x) > 1.0 ? 0.0 : 1.0 - x * x;
}
vec4 Rainbow(vec2 uv) {
	vec4 color = vec4(0.0);
	float c = 3.0;
	color.x = RainbowBump(c * (uv.x - 0.75));
	color.y = RainbowBump(c * (uv.x - 0.5));
	color.z = RainbowBump(c * (uv.x - 0.25));
	color.w = 1.0;
	float line = abs(0.01 / abs(0.5-uv.y) );//abs(0.01 / abs(0.5-uv.y) );
	//uv.y = abs( uv.y - 0.5 );
	uv.y = abs( uv.y - 0.005 );
	
	//vec4 soundWave =  IMG_NORM_PIXEL( inputImage, vec2(abs(0.5-uv.x)+0.005, uv.y) );
	vec4 soundWave = IMG_NORM_PIXEL( inputImage, vec2(abs(0.5-uv.x)+0.005, uv.y) );
	//color *= line * (1.0 - 2.0 * abs( 0.5 - uv.xxxx ) + pow( soundWave.y, 10.0 ) * 30.0 );
	color *= line * (1.0 - 2.0 * abs( 0.5 - uv.xxxx ) + pow( soundWave.y, 10.0 ) * 60.0 );

    return color;
}
void main(void)
{
   	vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;// 
	if (iFlipH)
	{
		uv.x = 1.0 - uv.x;
	}
	// flip vertically
	if (iFlipV)
	{
		uv.y = 1.0 - uv.y;
	}
	// Multiples of 4x5 work best
    vec2 vFontSize = vec2(20.0, 40.0);
  	vec4 vColour = vec4(0.7);
    vColour = mix( vColour, vec4(1.0, 1.0, 0.0, 0.0), PrintValue(gl_FragCoord.xy, vec2(30.0, 150.0), vFontSize, RENDERSIZE.x, 2.0, 0.0));
    vColour = mix( vColour, vec4(0.7, 0.5, 0.0, 0.0), PrintValue(gl_FragCoord.xy, vec2(100.0, 150.0), vFontSize, RENDERSIZE.y, 2.0, 0.0));
    vColour = mix( vColour, vec4(0.7, 0.0, 0.5, 0.0), PrintValue(gl_FragCoord.xy, vec2(930.0, 150.0), vFontSize, TIME, 2.0, 1.0));
	fragColor = mix( vec4(1.0, 1.0, 1.0, 1.0), Rainbow(uv), vColour);
  	
}

