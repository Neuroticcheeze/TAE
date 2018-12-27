#BEGIN V vUBER >>
#version 430

struct sParticle_Format0
{
	vec2 mPosition;
	vec2 mVelocity;
	float mLife;
	float mLifeStep;
};

layout( std430, binding = 3 ) buffer ParticleBuffer
{
	sParticle_Format0 bParticleBuffer[];
};

uniform mat3x3 uTransform;
uniform sampler2D uTintGradient;

out vec4 vColour;

void main()
{
	int id = gl_VertexID / 4;
	int subId = gl_VertexID % 4;
	sParticle_Format0 particle = bParticleBuffer[ id ];

	vec3 pPos = vec3( particle.mPosition, 1.0 );

	vec2 corner;

	switch ( subId )
	{
	case 0:
		corner = vec2( -1.0, -1.0 );
		break;
	case 1:
		corner = vec2( -1.0, 1.0 );
		break;
	case 2:
		corner = vec2( 1.0, 1.0 );
		break;
	case 3:
		corner = vec2( 1.0, -1.0 );
		break;
	}

	float scale = 0.015f;

	gl_Position = vec4( uTransform * ( pPos + vec3( corner * scale, 0.0f ) ), 1.0 );
	gl_Position.z = 0.0;

	vColour = texture( uTintGradient, vec2( particle.mLife, 0.25F ) );// * 0.5 + 0.25 ); //<--protects against black borders
}

////////////////////////////////////////////////////////////
#BEGIN F fUBER >>
#version 430

in vec4 vColour;

void main()
{
	gl_FragColor = vColour;
}