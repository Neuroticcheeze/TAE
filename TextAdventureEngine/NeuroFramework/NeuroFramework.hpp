// MathLibrary.h - Contains declaration of Function class  
#pragma once  

#ifdef NF_EXPORT  
#define NF_DECLSPEC __declspec(dllexport)   
#else  
#define NF_DECLSPEC __declspec(dllimport)   
#endif

extern "C"
{
	NF_DECLSPEC double Add( double a, double b );
	NF_DECLSPEC double Multiply( double a, double b );
	NF_DECLSPEC double AddMultiply( double a, double b );
}