#BEGIN V GAUSSBLUR >>
#version 420

layout( location = 0 ) in vec2 iPos;

void main()
{
	gl_Position = vec4( iPos, 0.0, 1.0 );
}

#BEGIN F GAUSSBLUR >>
#version 420

// Gaussian weights for a 9-wide stretch
float gGaussWeights[21] = float[]( 
	0.000001,	0.00001,	0.000078,	0.000489,	0.002403,	0.009245,	0.027835,	0.065591,	0.120978,	0.174666,	0.197413,
	0.174666,	0.120978,	0.065591,	0.027835,	0.009245,	0.002403,	0.000489,	0.000078,	0.00001,	0.000001
);

uniform sampler2D uPrevPassResult;
uniform int uBlurPass;

out vec4 oFinalColour;

void main()
{
	oFinalColour = vec4( 0.0 );
	for ( uint kOff = 9; kOff < 10; ++kOff )
	{
		const uint kDupeTotal = 5;
		for ( uint kDupe = 0; kDupe < kDupeTotal; ++kDupe )
		{
			uint kPix = ( kOff - 10 ) * kDupeTotal + kDupe;
			ivec2 kXY = ivec2( gl_FragCoord.xy ) + ( ( uBlurPass == 0 ) ? ivec2( kPix, 0 ) : ivec2( 0, kPix ) );
			oFinalColour += texelFetch( uPrevPassResult, kXY, 0 ) * gGaussWeights[ kOff ] * 0.5 * ( 5.0 / float( kDupeTotal ) );
		}
	}
}