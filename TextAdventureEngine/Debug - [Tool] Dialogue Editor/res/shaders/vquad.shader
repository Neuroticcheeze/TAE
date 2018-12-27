#BEGIN V VQuad_VMain >>
#version 420

uniform mat3x3 uTransform;
uniform vec2 u2xPos[ 3 ];
uniform vec4 uColour1;
uniform vec4 uColour2;

out vec4 vCol;

void main()
{
	float kThickness = u2xPos[ 2 ].x;
	vec2 kDiffN = normalize( u2xPos[ 1 ] - u2xPos[ 0 ] );
	vec2 kDiffN90 = vec2( kDiffN.y, -kDiffN.x ) * kThickness * 0.5;
	vec2 kLinePos;

	switch ( gl_VertexID )
	{
	case 0:
		kLinePos = u2xPos[ 0 ] + kDiffN90;
		vCol = uColour1;
		break;
	case 1:
		kLinePos = u2xPos[ 0 ] - kDiffN90;
		vCol = uColour1;
		break;
	case 2:
		kLinePos = u2xPos[ 1 ] - kDiffN90;
		vCol = uColour2;
		break;
	case 3:
		kLinePos = u2xPos[ 1 ] + kDiffN90;
		vCol = uColour2;
		break;
	}

	gl_Position = vec4( uTransform * vec3( kLinePos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;
}

////////////////////////////////////////////////////////////
#BEGIN F VQuad_FMain >>
#version 420

in vec4 vCol;

void main()
{
	gl_FragColor = vCol;
}
//==========================================================

