#BEGIN V Col_VExplicitUV >>
#version 420

layout( location = 0 ) in vec2 iPos;
layout( location = 1 ) in vec2 iTex;
uniform mat3x3 uTransform;
uniform vec2 uTextureCoords[ 4 ];
out vec2 vTexCoord;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;
	vTexCoord = uTextureCoords[ gl_VertexID ];
}

////////////////////////////////////////////////////////////
#BEGIN F Col_FTextureColour >>
#version 420

in vec2 vTexCoord;
uniform float uAlphaPow;
uniform vec4 uColour;
uniform sampler2D uTexture;

void main()
{
	gl_FragColor = texture( uTexture, vTexCoord ) * uColour;
	gl_FragColor.a = pow( gl_FragColor.a, uAlphaPow );
}
//==========================================================