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
   vec2 uv = gl_FragCoord.xy / RENDERSIZE.xy;

	float c = 3.0;
	vec3 color = vec3(1.0);
	color.x = 1.0;// RainbowBump(c * (uv.x - 0.75));
	color.y = RainbowBump(c * (uv.x - 0.5));
	color.z = RainbowBump(c * (uv.x - 0.25));
	

	float line = abs(0.01 / abs(0.5-uv.y) );
	uv.y = abs( uv.y - 0.5 );
	
	vec4 soundWave =  IMG_NORM_PIXEL( iChannel0, vec2(abs(0.5-uv.x)+0.005, uv.y) );
	color *= line * (1.0 - 2.0 * abs( 0.5 - uv.xxx ) + pow( soundWave.y, 10.0 ) * 30.0 );
	
  fragColor = vec4(color,1.0);
}
