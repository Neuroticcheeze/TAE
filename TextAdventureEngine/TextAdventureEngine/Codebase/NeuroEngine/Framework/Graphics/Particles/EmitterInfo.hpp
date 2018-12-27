#pragma once
#ifndef EMITTERINFO_H
#define EMITTERINFO_H

#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
struct EmitterInfo final
{
	Vector2 m_Position;
	float m_Rate;
	float m_Radius;
	Vector2 m_Throw;
	float m_Accuracy;
};

#endif//EMITTERINFO_H