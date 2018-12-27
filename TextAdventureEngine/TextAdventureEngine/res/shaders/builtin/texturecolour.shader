#BEGIN V VSimpleTextureColour >>
#version 420

layout( location = 0 ) in vec2 iPos;
layout( location = 1 ) in vec2 iTex;
layout( location = 2 ) in vec4 iCol;

uniform mat3x3 uTransform;

out vec4 vColour;
out vec2 vTexCoord;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;
	vTexCoord = iTex;
	vColour = iCol;
}

////////////////////////////////////////////////////////////
#BEGIN F FSimpleTextureColour >>
#version 420

in vec4 vColour;
in vec2 vTexCoord;

uniform vec4 uColour;
uniform sampler2D uTexture;

void main()
{
	gl_FragColor = texture( uTexture, vTexCoord ) * uColour * vColour;
}
//==========================================================