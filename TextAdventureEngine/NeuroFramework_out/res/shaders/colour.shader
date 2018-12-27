#BEGIN V Col_VMain >>
#version 420

layout( location = 0 ) in vec2 iPos;
uniform mat3x3 uTransform;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;
}

////////////////////////////////////////////////////////////
#BEGIN F Col_FMain >>
#version 420

uniform vec4 uColour;

void main()
{
	gl_FragColor = uColour;
}
//==========================================================

#BEGIN V Col_VColVertSrc >>
#version 420

layout( location = 0 ) in vec2 iPos;
layout( location = 1 ) in vec4 iCol;

uniform mat3x3 uTransform;

out vec4 vCol;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	//gl_Position.z = 0.0;
	vCol = iCol;
}

////////////////////////////////////////////////////////////
#BEGIN F Col_FColVertSrc >>
#version 420

in vec4 vCol;
uniform vec4 uColour;

void main()
{
	gl_FragColor = vCol * uColour;
}
//==========================================================

#BEGIN V Col_VExplicit >>
#version 420

uniform mat3x3 uTransform;
uniform vec2 uPositions[ 3 ];
uniform vec4 uColours[ 3 ];
out vec4 vCol;

void main()
{
	gl_Position = vec4( uTransform * vec3( uPositions[ gl_VertexID ], 1.0 ), 1.0 );
	gl_Position.z = 0.0;
	vCol = uColours[ gl_VertexID ];
}

////////////////////////////////////////////////////////////
#BEGIN F Col_FExplicit >>
#version 420

in vec4 vCol;

void main()
{
	gl_FragColor = vCol;
}
//==========================================================
