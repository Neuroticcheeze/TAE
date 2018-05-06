#pragma once
#ifndef RANDOMRANGE_H
#define RANDOMRANGE_H

#include <Framework/Utils/Random.hpp>

//=====================================================================================
class RandomRange final
{
public:

	RandomRange( float a_Min = 0.0F, float a_Max = 1.0F );
	float GetValue() const;

private:

	float m_Min;
	float m_Max;
	static Random s_Random;
};

#endif//RANDOMRANGE_H