#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Math/Gradient.hpp>
#include <Framework/Math/RandomRange.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/Particles/Emitter.hpp>

//=====================================================================================
struct ParticleInfo final
{
	float Dampening = 0.0F;
	Vector2 Acceleration;
	Gradient< Colour > TintOverTime;
	Gradient< float > ScaleOverTime;
	Gradient< float > AnimationOverTime;
	Gradient< float > AdditiveBlendOverTime;
	float AnimationSpeed = 1.0F;
	enum { AW_CLAMP, AW_REPEAT, AW_MIRROR } AnimationWrap = AW_CLAMP;
	enum { AF_AOT_FLOOR, AF_AOT_CEIL, AF_AOT_ROUND } AnimationFilter = AF_AOT_ROUND;
	bool AnimationEnableFrameBlend = false;
	GraphicsManager::TextureAtlas Sprite;
	RandomRange Torque;
	RandomRange Life;
	float Density = 1.0F;
	WeakPointer< Emitter > OnDeathEmitter;
};

//=====================================================================================
struct Particle final
{
	Particle()
	{
		m_Time = 0.0F;
		m_Life = 0.0F;
		m_Torque = 0.0F;
		m_Angle = 0.0F;
		m_PrevSprite = 0;
	}

private:

	float m_Time;
	float m_Life;
	float m_Torque;
	float m_Angle;
	Vector2 m_Position;
	Vector2 m_Velocity;
	int32_t m_PrevSprite;

	friend class ParticleSystem;
};

#endif//PARTICLE_H