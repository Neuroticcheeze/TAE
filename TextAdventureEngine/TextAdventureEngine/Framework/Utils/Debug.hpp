#pragma once
#ifndef DEBUG_H
#define DEBUG_H

///* Enable this macro for usage of debug toolset */
#define ALWAYS_DEBUG

#if defined( _DEBUG ) || defined( ALWAYS_DEBUG )

extern bool g_StartProfiling;
#define PROFILE_BEGIN g_StartProfiling = true;

#define PRINT( FMT, ... ) VSOut( __LINE__, __FUNCTION__, FMT, __VA_ARGS__ )
void VSOut( uint32_t a_Line, const char * a_FunctionName, const char * a_Msg, ... );

#define ASSERT( EXP, FMT, ... ) Assert( true, EXP, __LINE__, __FILE__, FMT, __VA_ARGS__ )
#define ASSERT_WARN( EXP, FMT, ... ) Assert( false, EXP, __LINE__, __FILE__, FMT, __VA_ARGS__ )
bool Assert( bool a_Fatal, bool a_Expression, uint32_t a_Line, const char * a_File, const char * a_Msg, ... );

#define VERIFY( EXP, CONDITION, FMT, ... ) Assert( true, EXP CONDITION, __LINE__, __FILE__, FMT, __VA_ARGS__ )

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

//Placed at the start of a scope body, this will profile the performance of every processing of this block in order to identify bottlenecks.
#define PROFILE _ScopedProfiler_ _##__LINE__( __FUNCTION__ "\n[" __FILE__ "]" )
#define PROFILE2( BLOCK_NAME ) _ScopedProfiler_ _##__LINE__( __FUNCTION__ " >> \"" BLOCK_NAME "\"\n[" __FILE__ "]"  )

//Output all profiling information
#define PROFILE_DUMP_RECORDINGS PDump()
void PDump();

#else
#define PROFILE_BEGIN
#define PRINT( FMT, ... )
#define PROFILE
#define PROFILE2( BLOCK_NAME )
#define PROFILE_DUMP_RECORDINGS
#define ASSERT( EXP ) ( EXP )
#define ASSERT_WARN( EXP ) ( EXP )
#define VERIFY( EXP, CONDITION ) ( EXP )
#define PROFILE_DUMP_RECORDINGS
#endif

#endif//DEBUG_H