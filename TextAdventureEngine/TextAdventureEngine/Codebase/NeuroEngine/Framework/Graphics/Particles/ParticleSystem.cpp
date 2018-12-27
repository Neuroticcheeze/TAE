#include "ParticleSystem.hpp"
#include "Emitter.hpp"
#include "EmitterInfo.hpp"
#include <Framework/Containers/CString.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Thread/ThreadManager.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
ParticleSystem::ParticleSystem( uint32_t a_MaxParticles, const ParticleInfo & a_ParticleInfo )
	: m_ParticlesMax( a_MaxParticles )
	, m_ParticlesNum( 0 )
	, m_Particles( new Particle[ a_MaxParticles ] )
	, m_SimSpeed( 1.0F )
	, m_ParticleInfo( a_ParticleInfo )
{
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_Particles;
}

//=====================================================================================
WeakPointer< Emitter > ParticleSystem::CreateEmitter( const EmitterInfo & a_Info )
{
	Pointer< Emitter > em = Pointer< Emitter >( Emitter() );
	em->Setup( a_Info );
	em->m_Owner = this;
	m_Emitters.Append( em );
	return WeakPointer< Emitter >( em );
}

//=====================================================================================
void ParticleSystem::DestroyEmitter( const WeakPointer< Emitter > & a_EmitterToDestroy )
{
	m_Emitters.Remove( a_EmitterToDestroy );
}

//=====================================================================================
void ParticleSystem::Emit( const Vector2 & a_Position, const Vector2 & a_Velocity )
{
	if ( m_ParticlesNum == m_ParticlesMax )
	{
		return;
	}

	Particle ptcl;
	ptcl.m_Position = a_Position;
	ptcl.m_Velocity = a_Velocity;
	ptcl.m_Life = m_ParticleInfo.Life.GetValue();
	ptcl.m_Torque = m_ParticleInfo.Torque.GetValue();
	m_Particles[ m_ParticlesNum ] = ptcl;
	++m_ParticlesNum;
}

//=====================================================================================
void ParticleSystem::Clear()
{
	m_ParticlesNum = 0;
}

//=====================================================================================
void ParticleSystem::SetSimSpeed( float a_Speed )
{
	m_SimSpeed = Clamp( a_Speed, 0.0F, 10.0F );
}

//=====================================================================================
void ParticleSystem::Tick( float a_DeltaTime )
{
	PROFILE;

	const double dt = (double)a_DeltaTime * (double)m_SimSpeed;

	if ( m_Emitters.Count() > 0 )
	{
		uint32_t count = m_Emitters.Count() - 1;
		for ( int32_t k = count; k >= 0; --k )
		{
			Emitter * emitter = m_Emitters[ (uint32_t)k ].Ptr();
			
			emitter->Tick( static_cast< float >( dt ) );

			if ( emitter->m_Dead )
			{
				m_Emitters.RemoveAt( k );
			}
		}
	}

	{
		PROFILE2( "Sort" );
		Array< Particle > sortedArray;
		sortedArray.Resize( m_ParticlesNum );
		BCopy( m_Particles, sortedArray.First(), sizeof( Particle ) * m_ParticlesNum );
		sortedArray.Sort( []( const Particle & a_Left, const Particle & a_Right ){ return a_Left.m_Time < a_Right.m_Time; } );
		BCopy( sortedArray.First(), m_Particles, sizeof( Particle ) * m_ParticlesNum );
	}

	GraphicsManager::Instance().SetState( GraphicsManager::RS_TRANSPARENCY, true );		

	if ( m_ParticlesNum > 0 )
	{
		//float preTopMat[ 9 ];
		//float particleTransform[ 9 ];
		//float ppFinalTopMat[ 9 ];
		//BCopy( GraphicsManager::Instance().TfGetTop(), preTopMat, sizeof( float ) * 9 );
		//GraphicsManager::Instance().TfPush();
		
		uint32_t count = m_ParticlesNum - 1;
		for ( int32_t k = count; k >= 0; --k )
		{
			Particle & p = m_Particles[ (uint32_t)k ];
			const double t = p.m_Time / p.m_Life;

			Colour col = m_ParticleInfo.TintOverTime.Evaluate( static_cast< float >( t ) );
			float scale = m_ParticleInfo.ScaleOverTime.Evaluate( static_cast< float >( t ) );
			
			{
				PROFILE2( "Transform" );
				GraphicsManager::Instance().TfPush();
				GraphicsManager::Instance().TfTranslate( p.m_Position );
				GraphicsManager::Instance().TfRotate( p.m_Angle );
				GraphicsManager::Instance().TfScale( Vector2( scale, -scale ) );
				//GraphicsManager::Instance().TfMakeTRS( particleTransform, p.m_Position, p.m_Angle, Vector2( scale, -scale ) );
				//GraphicsManager::Instance().TfMul( particleTransform, preTopMat, ppFinalTopMat );
				//GraphicsManager::Instance().TfSetTop( preTopMat );
			}

			float additive = m_ParticleInfo.AdditiveBlendOverTime.Evaluate( static_cast< float >( t ) );

			uint32_t spriteIndex = 0;
			if ( m_ParticleInfo.AnimationOverTime.NumKeyFrames() > 1 )
			{
				PROFILE2( "Animation" );

				float afactor = static_cast< float >( t * m_ParticleInfo.AnimationSpeed );

				switch ( m_ParticleInfo.AnimationWrap )
				{
				case ParticleInfo::AW_CLAMP:
					afactor = Clamp( afactor, 0.0F, 1.0F );
					break;
				case ParticleInfo::AW_REPEAT:
					afactor = Wrap( afactor, 0.0F, 1.0F );
					break;
				case ParticleInfo::AW_MIRROR:
					if ( IsEven( static_cast< int32_t >( Floor( afactor ) ) ) )
					{
						afactor = Wrap( afactor, 0.0F, 1.0F );
					}
					else
					{
						afactor = 1.0F - Wrap( afactor, 0.0F, 1.0F );
					}
					break;
				default:
					break;
				}

				float localAnimT = 0.0F;
				float anim = m_ParticleInfo.AnimationOverTime.Evaluate( afactor, &localAnimT );

				switch ( m_ParticleInfo.AnimationFilter )
				{
				case ParticleInfo::AF_AOT_CEIL:
					spriteIndex = static_cast< uint32_t >( Ceil( anim ) );
					break;
				case ParticleInfo::AF_AOT_ROUND:
					spriteIndex = static_cast< uint32_t >( Round( anim ) );
					break;
				case ParticleInfo::AF_AOT_FLOOR:
				default:
					spriteIndex = static_cast< uint32_t >( Floor( anim ) );
				}

				//TODO blending between frames...
				//if ( m_ParticleInfo.AnimationEnableFrameBlend )
				//{
				//
				//	float fBetweenPrevSpriteAndCurrSprite = localAnimT;
				//
				//	// Draw blend source
				//	GraphicsManager::Instance().SetColour( col * Colour::WHITE.Lerp( Colour::INVISIBLE, fBetweenPrevSpriteAndCurrSprite ), GraphicsManager::COL_PRIMARY );
				//	m_ParticleInfo.Sprite->DrawSprite( spriteIndex, Vector2::ONE * -0.5F, Vector2::ONE );
				//
				//	// Draw blend destination
				//	GraphicsManager::Instance().SetColour( col * Colour::INVISIBLE.Lerp( Colour::WHITE, fBetweenPrevSpriteAndCurrSprite ), GraphicsManager::COL_PRIMARY );
				//	m_ParticleInfo.Sprite->DrawSprite( spriteIndex - 1, Vector2::ONE * -0.5F, Vector2::ONE );
				//
				//	//PRINT( "%u -> %u", p.m_PrevSprite, spriteIndex );
				//	
				//	if ( fBetweenPrevSpriteAndCurrSprite >= 1.0F )
				//	{
				//		p.m_PrevSprite = spriteIndex;
				//	}
				//}
			}

			if ( !m_ParticleInfo.AnimationEnableFrameBlend )
			{
				PROFILE2( "Animation Frameblend" );
				if ( additive <= 0.0F )
				{				
					GraphicsManager::Instance().SetColour( col, GraphicsManager::COL_PRIMARY );
					m_ParticleInfo.Sprite->DrawSprite( spriteIndex, Vector2::ONE * -0.5F, Vector2::ONE );
				}

				else if ( additive >= 1.0F )
				{
					GraphicsManager::Instance().SetState( GraphicsManager::RS_ADDITIVE_BLEND, true );
					GraphicsManager::Instance().SetColour( col, GraphicsManager::COL_PRIMARY );
					m_ParticleInfo.Sprite->DrawSprite( spriteIndex, Vector2::ONE * -0.5F, Vector2::ONE );
					GraphicsManager::Instance().SetState( GraphicsManager::RS_ADDITIVE_BLEND, false );
				}

				else
				{
					GraphicsManager::Instance().SetColour( col * Colour( 1.0F, 1.0F, 1.0F, Pow( Min( 1.0F, 1.0F - additive ), 1.9F ) ), GraphicsManager::COL_PRIMARY );
					m_ParticleInfo.Sprite->DrawSprite( spriteIndex, Vector2::ONE * -0.5F, Vector2::ONE );

					GraphicsManager::Instance().SetState( GraphicsManager::RS_ADDITIVE_BLEND, true );
					GraphicsManager::Instance().SetColour( col * additive, GraphicsManager::COL_PRIMARY );
					m_ParticleInfo.Sprite->DrawSprite( spriteIndex, Vector2::ONE * -0.5F, Vector2::ONE );
					GraphicsManager::Instance().SetState( GraphicsManager::RS_ADDITIVE_BLEND, false );
				}
			}

			GraphicsManager::Instance().TfPop();

			{
				PROFILE2( "Particle Update" );
				p.m_Angle += p.m_Torque * static_cast< float >( dt );
				p.m_Position.x += p.m_Velocity.x * static_cast< float >( dt );
				p.m_Position.y += p.m_Velocity.y * static_cast< float >( dt );
				p.m_Velocity.x += m_ParticleInfo.Acceleration.x * static_cast< float >( dt ) * m_ParticleInfo.Density;
				p.m_Velocity.y += m_ParticleInfo.Acceleration.y * static_cast< float >( dt ) * m_ParticleInfo.Density;
				p.m_Velocity *= Pow( Clamp( 1.0F - m_ParticleInfo.Dampening ), dt );
				p.m_Time += static_cast< float >( dt );
			}

			if ( p.m_Time >= p.m_Life )
			{
				if ( m_ParticleInfo.OnDeathEmitter )
				{
					for ( int i = rand() %32; i < 32; ++i )
					m_ParticleInfo.OnDeathEmitter->m_Owner->Emit( p.m_Position, m_Random.RandomVector2() * m_Random.Range( 0.0F, 10.0F ) );
				}

				m_Particles[ k ] = m_Particles[ --m_ParticlesNum ];
			}
		}

		//GraphicsManager::Instance().TfPop();
	}
}

//=====================================================================================
Random & ParticleSystem::GetRand()
{
	return m_Random;
}