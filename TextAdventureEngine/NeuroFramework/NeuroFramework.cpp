// NeuroFramework.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "NeuroFramework.hpp"


double Add(double a, double b)
{
	return a + b;
}

double Multiply(double a, double b)
{
	return a * b;
}

double AddMultiply(double a, double b)
{
	return a + b * a * b;
}