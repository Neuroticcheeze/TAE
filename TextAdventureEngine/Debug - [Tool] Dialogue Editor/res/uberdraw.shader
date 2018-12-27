#BEGIN V MAIN >>
#version 410

layout( location = 0 ) in vec2 pos;
layout( location = 1 ) in vec2 texw;

out vec4 colMix;
out vec2 texv;
uniform float time;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform float str;
uniform mat3 transform;
uniform int glow;

void main()
{
	float flex = str * (1- glow);
	colMix = textureLod(tex2, texw * 0.1 + time * 0.1, 2);
	float f = sin(( pos.x + pos.y) * 3.14159 * 10 + time * 3.0 ) * 0.01;
	vec3 tpos = transform * vec3(pos, 1);
	gl_Position = vec4( tpos.xy + vec2(colMix.g * 0.3*flex - 0.25, 0.0), 0, 1);
	texv = texw;
	//colMix = vec4(vec3(clamp(pow(f*2.0, 1.0),0,1)), 1);
}

#BEGIN F MAIN >>
#version 410
#include <math.glsl>

in vec4 colMix;
in vec2 texv;
uniform float time;
uniform sampler2D tex;
uniform sampler2D texGlow;
uniform int glow;

void main()
{
	if (glow == 0)
	{
		gl_FragColor = texture(tex, texv);
		if (dot(gl_FragColor.rgb, vec3(1, 0, 0)) > 0.9)
		{
			gl_FragColor.rgb = vec3(texv, 0.5);
		}
	}
	else
	{
		float f = 0.01;
		float lod = 3.7;
		gl_FragColor =
			clamp( textureLod(texGlow, texv + vec2(0.0, 0.00), lod) +
				textureLod(texGlow, texv + vec2(f, 0.00), lod) +
				textureLod(texGlow, texv + vec2(-f, 0.00), lod) +
				textureLod(texGlow, texv + vec2(0.00, f), lod) +
				textureLod(texGlow, texv + vec2(0.00, -f), lod), vec4(0), vec4(1)) * 0.666666
			+ texture(texGlow, texv);
	}
}