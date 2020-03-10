/*{
	"CREDIT" : "Matrix 01 by Bruce Lane",
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
			"NAME": "iColor", 
			"TYPE" : "color", 
			"DEFAULT" : [
				0.6, 
				0.0, 
				0.9, 
				1.0
			]
		}
	],
}
*/
float hash11(float x)
{
 	return fract(sin(x * 42.146291) * 4215.4827);   
}

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
    //vec3 rgb = IMG_NORM_PIXEL(inputImage, uv + vec2(iPixelX,hash11(iPixelY-sin(TIME*0.000001)))).xyz;
    //vec3 rgb = IMG_NORM_PIXEL(inputImage, uv + vec2( iPixelX,sin (TIME*0.000001) ) ).xyz;
    //fragColor=IMG_NORM_PIXEL(inputImage, uv + vec2( iPixelX, hash11(iPixelY-(sin(TIME*0.000001)) )*2.0 ) );	
    fragColor=IMG_NORM_PIXEL(inputImage, uv + vec2( iPixelX, iPixelY) );	
    	
}
