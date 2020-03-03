uniform vec3      	iResolution;
uniform sampler2D 	iChannel0;
uniform sampler2D 	iChannel1;
uniform sampler2D   iChannel2;
uniform sampler2D   iChannel3;
uniform sampler2D   iChannel4;
uniform sampler2D   iChannel5;
uniform sampler2D   iChannel6;
uniform sampler2D   iChannel7;
uniform float       iWeight0;
uniform float       iWeight1;
uniform float       iWeight2;
uniform float       iWeight3;
uniform float       iWeight4;
uniform float       iWeight5;
uniform float       iWeight6;
uniform float       iWeight7;

void main() {
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec3 c = texture(iChannel0, uv).xyz * iWeight0 + texture(iChannel1, uv).xyz * iWeight1 + texture(iChannel2, uv).xyz * iWeight2 + texture(iChannel3, uv).xyz * iWeight3 +
        texture(iChannel4, uv).xyz * iWeight4 + texture(iChannel4, uv).xyz * iWeight5 + texture(iChannel4, uv).xyz * iWeight6 + texture(iChannel4, uv).xyz * iWeight7;
   	gl_FragColor = vec4(c.r, c.g, c.b, 1.0);
}
