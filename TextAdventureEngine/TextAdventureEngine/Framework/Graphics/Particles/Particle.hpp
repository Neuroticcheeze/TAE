#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Math/Gradient.hpp>
#include <Framework/Math/RandomRange.hpp>
#include <Framework/Graphics/SpriteSheet.hpp>

//=====================================================================================
struct ParticleInfo final
{
	Vector2 Acceleration;
	Gradient< Colour > TintOverTime;
	Gradient< float > ScaleOverTime;
	//Gradient< float > AnimationOverTime;
	SpriteSheet Sprite;
	RandomRange Torque;
	RandomRange Life;
	float Density;
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
	}

private:

	float m_Time;
	float m_Life;
	float m_Torque;
	float m_Angle;
	Vector2 m_Position;
	Vector2 m_Velocity;

	friend class ParticleSystem;
};

#endif//PARTICLE_H