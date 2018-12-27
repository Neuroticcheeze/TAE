#pragma once
#ifndef EMITTER_H
#define EMITTER_H

#include "EmitterInfo.hpp"

//=====================================================================================
class ParticleSystem;

//=====================================================================================
class Emitter final
{
	friend class ParticleSystem;
public:
	EmitterInfo & GetProperties();
	const EmitterInfo & GetProperties() const;

private:
	void Tick( float a_DeltaTime );
	void Setup( const EmitterInfo & a_Info );
	float m_Time;
	EmitterInfo m_Info;
	ParticleSystem * m_Owner;
	bool m_Dead;
	Vector2 m_PrevPosition;
};

#endif//EMITTER_H