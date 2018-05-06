#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include <Framework/Template/Manager.hpp>

//=====================================================================================
class Engine final : public Manager< Engine >
{
public:


	float GetTime() const;


private:
};

#endif//ENGINE_H