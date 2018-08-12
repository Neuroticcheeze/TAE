#BEGIN V MAIN >>
#version 420

layout( location = 0 ) in vec2 pos;
layout( location = 1 ) in vec2 texw;

out vec2 texv;
uniform mat3x3 uTransform;

void main()
{
	gl_Position = vec4( uTransform * vec3( pos, 0 ), 1);
	texv = texw;
}

#BEGIN F MAIN >>
#version 420

in vec2 texv;

layout(binding = 0 ) uniform sampler2D gTex0;


void main()
{
	gl_FragColor.a = texture(gTex0, texv).a;
	gl_FragColor.rgb = vec3(1, 1, 1);
}