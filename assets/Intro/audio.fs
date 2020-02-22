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


float RainbowBump(float x) {
	return abs(x) > 1.0 ? 0.0 : 1.0 - x * x;
}
void main(void)
{
   vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy /100.0;

	float c = 3.0;
	vec3 color = vec3(1.0);
	color.x = 1.0;// RainbowBump(c * (uv.x - 0.75));
	color.y = RainbowBump(c * (uv.x - 0.5));
	color.z = RainbowBump(c * (uv.x - 0.25));
	

	float line = abs(0.01 / abs(0.5-uv.y) );
	uv.y = abs( uv.y - 0.5 );
	
	//vec4 soundWave =  IMG_NORM_PIXEL( inputImage, vec2(abs(0.5-uv.x)+0.005, uv.y) );
	vec4 soundWave =  IMG_NORM_PIXEL( inputImage, uv);
	//color *= line * (1.0 - 2.0 * abs( 0.5 - uv.xxx ) + pow( soundWave.y, 10.0 ) * 30.0 );
	color *= line * (1.0 - 2.0 * abs( 0.5 - uv.xxx ) + pow( soundWave.x, 10.0 ) * 30.0 );
	
  fragColor = vec4(soundWave.x, color.y, soundWave.y,1.0);
}
/*
void main(void)
{
    vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy; 
    // flip horizontally
	if (iFlipH)
	{
		uv.x = 1.0 - uv.x;
	}
	// flip vertically
	if (iFlipV)
	{
		uv.y = 1.0 - uv.y;
	}
	// rotate
	//float rad = radians(360.0 * fract(iTime*iRotationSpeed));
	//mat2 rotate = mat2(cos(rad),sin(rad),-sin(rad),cos(rad));
	//uv = rotate * (uv - 0.5) + 0.5;
    vec3 rgb = IMG_NORM_PIXEL(inputImage, uv).xyz;
    fragColor=vec4(rgb, 1.0);	
}*/
