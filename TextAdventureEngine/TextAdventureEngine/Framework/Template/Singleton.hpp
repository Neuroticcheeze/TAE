#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H

//=====================================================================================
template< typename T >
class Singleton
{
public:
	static T& Instance()
	{
		static T s_Instance;
		return s_Instance;
	}
};

#endif//SINGLETON_H