#BEGIN V VMain >>
#version 420

layout( location = 0 ) in vec2 iPos;

uniform mat3x3 uTransform;
uniform vec2 uControlPoints[ 64 ];
uniform int uControlPointsCount;
uniform float uThickness;

vec2 evaluateBezier( float a_T )
{
	vec2 kTemp[ 64 ];
	for ( int i = 0; i < uControlPointsCount; ++i )
	{
		kTemp[ i ] = uControlPoints[ i ];
	}

	int kStackLength = uControlPointsCount;

	while ( kStackLength > 1 )
	{
		--kStackLength;
		for ( int t = 0; t < kStackLength; ++t )
		{
			vec2 A = kTemp[ t + 0 ];
			vec2 B = kTemp[ t + 1 ];

			kTemp[ t + 0 ] = A * ( 1.0F - a_T ) + B * a_T;
		}
	}

	return kTemp[ 0 ];
}

void main()
{
	int kTIdx = gl_VertexID >> 1;
	float kT = kTIdx / 128.0;
	float kTAux = ( kTIdx + 1 ) / 128.0;

	// Vertex Solver
	vec2 kTEvalPos2 = evaluateBezier( kT );
	vec2 kTEvalPos2Aux = evaluateBezier( kTAux );
	vec2 kTEvalPos2DiffN = normalize( kTEvalPos2Aux - kTEvalPos2 );
	vec2 kAlignment = vec2( kTEvalPos2DiffN.y, -kTEvalPos2DiffN.x );

	vec2 kFinalPos = kTEvalPos2 + ( ( int( mod( gl_VertexID, 2 ) ) * 2 - 1 ) * kAlignment ) * uThickness * 0.5;

	gl_Position = vec4( uTransform * vec3( kFinalPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;

}

////////////////////////////////////////////////////////////
#BEGIN F FMain >>
#version 420

uniform vec4 uColour;

void main()
{
	gl_FragColor = uColour;
}

//==========================================================

#BEGIN V VMainDashed >>
#version 420

layout( location = 0 ) in vec2 iPos;

uniform mat3x3 uTransform;
uniform vec2 uControlPoints[ 64 ];
uniform int uControlPointsCount;
uniform float uThickness;

out float vT;

vec2 evaluateBezier( float a_T )
{
	vec2 kTemp[ 64 ];
	for ( int i = 0; i < uControlPointsCount; ++i )
	{
		kTemp[ i ] = uControlPoints[ i ];
	}

	int kStackLength = uControlPointsCount;

	while ( kStackLength > 1 )
	{
		--kStackLength;
		for ( int t = 0; t < kStackLength; ++t )
		{
			vec2 A = kTemp[ t + 0 ];
			vec2 B = kTemp[ t + 1 ];

			kTemp[ t + 0 ] = A * ( 1.0F - a_T ) + B * a_T;
		}
	}

	return kTemp[ 0 ];
}

void main()
{
	int kTIdx = gl_VertexID >> 1;
	float kT = kTIdx / 128.0;
	float kTAux = ( kTIdx + 1 ) / 128.0;



	// Ark Length Solver
	const float SOLVER_RESOLUTION = ( 1.0 / 128.0 );

	float kPartialCurveLength = 0.0;
	float kTotalCurveLength = 0.0;
	for ( float kTRemapIter = 0.0; kTRemapIter < 1.0; kTRemapIter += SOLVER_RESOLUTION )
	{
		kTotalCurveLength += length( evaluateBezier( kTRemapIter + SOLVER_RESOLUTION ) - evaluateBezier( kTRemapIter ) );
		kPartialCurveLength = ( kTRemapIter <= kT ) ? kTotalCurveLength : kPartialCurveLength;
	}

	vT = kPartialCurveLength;//		/ kTotalCurveLength;



	// Vertex Solver
	vec2 kTEvalPos2 = evaluateBezier( kT );
	vec2 kTEvalPos2Aux = evaluateBezier( kTAux );
	vec2 kTEvalPos2DiffN = normalize( kTEvalPos2Aux - kTEvalPos2 );
	vec2 kAlignment = vec2( kTEvalPos2DiffN.y, -kTEvalPos2DiffN.x );

	vec2 kFinalPos = kTEvalPos2 + ( ( int( mod( gl_VertexID, 2 ) ) * 2 - 1 ) * kAlignment ) * uThickness * 0.5;

	gl_Position = vec4( uTransform * vec3( kFinalPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;

}

////////////////////////////////////////////////////////////
#BEGIN F FMainDashed >>
#version 420

uniform vec4 uColour1;
uniform vec4 uColour2;
uniform float uDash;
uniform float uDashRatio;

in float vT;

void main()
{
	gl_FragColor = ( mod( vT, uDash ) >= uDashRatio * uDash ) ? uColour2 : uColour1;
}
//==========================================================
