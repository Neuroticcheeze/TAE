#BEGIN V MAIN >>
#version 420

layout( location = 0 ) in vec2 iPos;

void main()
{
	gl_Position = vec4( iPos, 0.0, 1.0 );
}

#BEGIN F HVPASS >>
#version 420

uniform sampler2D uPrevPassResult;
out vec4 oFinalColour;

// Gaussian weights for a 21-wide stretch
float gGaussWeights[ 21 ] = float[]( 
	0.000001,	0.00001,	0.000078,	0.000489,	0.002403,	0.009245,	0.027835,	0.065591,	0.120978,	0.174666,	0.197413,
	0.174666,	0.120978,	0.065591,	0.027835,	0.009245,	0.002403,	0.000489,	0.000078,	0.00001,	0.000001
);

void main()
{
	ivec2 coords = ivec2( gl_FragCoord.xy );
	coords.y = textureSize( uPrevPassResult, 0 ).y - coords.y;

	float FAC = 3.0;

	oFinalColour = vec4( 0.0 );
	for ( uint i = 0; i < 21; ++i )
	{
		float w = gGaussWeights[ i ];
		
		ivec2 offs = ivec2( int((float(i)-21.0/2.0)*FAC), 0 );
		oFinalColour += texelFetch( uPrevPassResult, coords + offs, 0 ) * w;
	}
	for ( uint i = 0; i < 21; ++i )
	{
		float w = gGaussWeights[ i ];
		
		ivec2 offs = ivec2( 0, int((float(i) - 21.0 / 2.0)*FAC));
		oFinalColour += texelFetch( uPrevPassResult, coords + offs, 0 ) * w;
	}

	oFinalColour /= 2.0;
}