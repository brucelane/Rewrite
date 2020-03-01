
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
// https://www.shadertoy.com/view/ls2Xzd
// Algorithm found in https://medium.com/community-play-3d/god-rays-whats-that-5a67f26aeac2
float PI = 3.14159265358979586;

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
	if (iDebug) {		
		fragColor = IMG_NORM_PIXEL( inputImage, uv*2.5) ;
	} else {
		fragColor = crepuscular_rays(uv, pos.xy);
	}
}
