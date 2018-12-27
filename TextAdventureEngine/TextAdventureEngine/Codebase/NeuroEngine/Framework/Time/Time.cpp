#include "Time.hpp"
#include <chrono>

using namespace std::chrono;
typedef time_point< steady_clock > tp;

//=====================================================================================
tp g_BeginTime;

//=====================================================================================
void Time::Begin()
{
	g_BeginTime = tp( high_resolution_clock::now() );
}

//=====================================================================================
double Time::EndMSF64()
{
	tp::duration diff = high_resolution_clock::now() - g_BeginTime;
	duration< double, std::milli > ms = duration< double, std::milli >( diff );
	return static_cast< double >( ms.count() );
}