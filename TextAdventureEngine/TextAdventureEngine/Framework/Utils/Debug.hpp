#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#ifdef _DEBUG

#define PRINT( FMT, ... ) VSOut( __LINE__, __FUNCTION__, FMT, __VA_ARGS__ )
void VSOut( uint32_t a_Line, const char * a_FunctionName, const char * a_Msg, ... );

#define ASSERT( EXP, FMT, ... ) Assert( EXP, __LINE__, __FILE__, FMT, __VA_ARGS__ )
bool Assert( bool a_Expression, uint32_t a_Line, const char * a_File, const char * a_Msg, ... );

#define ASSERT_HRESULT( HRES, FMT, ... ) AssertHResult( HRES, __LINE__, __FILE__, FMT, __VA_ARGS__ )
bool AssertHResult( long a_HResult, uint32_t a_Line, const char * a_File, const char * a_Msg, ... );

struct _ScopedProfiler_ final
{
	explicit _ScopedProfiler_( const char * );
	~_ScopedProfiler_();

private:
	void * m_Start;
	const char * m_Name;
};

//Placed at the start of a function body, this will profile the performance of every call to this function in order to identify bottlenecks.
#define PROFILE _ScopedProfiler_ _##__LINE__( __FUNCTION__ "\n[" __FILE__ "]" )

#define PROFILE_DUMP_RECORDINGS PDump()
void PDump();

#else
#define PRINT( FMT, ... )
#define PROFILE
#define PROFILE_DUMP_RECORDINGS
#define ASSERT( EXP ) EXP
#endif

#endif//DEBUG_H