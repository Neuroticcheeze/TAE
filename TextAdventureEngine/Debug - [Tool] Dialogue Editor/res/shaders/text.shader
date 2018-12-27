#BEGIN V Text_VMain >>
#version 420

layout( location = 0 ) in vec2 iPos;
layout( location = 1 ) in vec2 iTex;
uniform mat3x3 uTransform;
out vec2	   vTex;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;
	vTex = iTex;
}

////////////////////////////////////////////////////////////
// Perfect Edges -- Black border artifacts removed
////////////////////////////////////////////////////////////
#BEGIN F Text_FPerfectEdges >>
#version 420

in vec2			  vTex;
uniform sampler2D uTex0;
uniform vec4	  uColour;

void main()
{
	gl_FragColor = vec4( vec3( 1.0 ), texture( uTex0, vTex ).a ) * uColour;
}
//==========================================================


////////////////////////////////////////////////////////////
// Perfect Edges + Gradiented -- Black border artifacts removed with gradient colour
////////////////////////////////////////////////////////////
#BEGIN F Text_FPerfectEdgesGradiented >>
#version 420

in vec2			  vTex;
uniform sampler2D uTex0;
uniform vec4	  uColour;
uniform vec4	  uColour2;

void main()
{
	float kAlpha = texture( uTex0, vTex ).a;
	vec4 kColour = mix( uColour, uColour2, vTex.y );

	// Apply correct blending
	float kBlendCorrectionFactor = smoothstep( 0.0, 1.0, ( 0.5 - abs( vTex.y - 0.5 ) ) * 2.0 );
	kColour.rgb = kColour.rgb / mix( 1.0, max( kColour.r, max( kColour.g, kColour.b ) ), kBlendCorrectionFactor );

	gl_FragColor = vec4( vec3( 1.0 ), kAlpha ) * kColour;
}
//==========================================================

////////////////////////////////////////////////////////////
// Glyph-based Text Rendering
////////////////////////////////////////////////////////////
#BEGIN V Text_VGlyph >>
#version 420

layout( location = 0 ) in vec2 iPos;
uniform mat3x3 uTransform;
uniform vec2   uGlyphUV;
out vec2	   vTex;

void main()
{
	gl_Position = vec4( uTransform * vec3( iPos, 1.0 ), 1.0 );
	gl_Position.z = 0.0;

	switch( gl_VertexID )
	{
	case 0: vTex = vec2( uGlyphUV.s, 0.0 );
		break;
	case 1: vTex = vec2( uGlyphUV.s, 1.0 );
		break;
	case 2: vTex = vec2( uGlyphUV.s + uGlyphUV.y, 1.0 );
		break;
	case 3: vTex = vec2( uGlyphUV.s + uGlyphUV.y, 0.0 );
		break;
	}
}