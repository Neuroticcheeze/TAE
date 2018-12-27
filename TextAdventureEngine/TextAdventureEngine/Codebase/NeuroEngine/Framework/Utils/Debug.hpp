#pragma once
#ifndef DEBUG_H
#define DEBUG_H

///* Enable this macro for usage of debug toolset */
#define ALWAYS_DEBUG

#if defined( _DEBUG ) || defined( ALWAYS_DEBUG )

extern bool g_StartProfiling;
#define PROFILE_BEGIN g_StartProfiling = true;

extern bool g_PrintToConsole;
// Enable in-game console echoing.
#define PRINT_TO_CONSOLE_ENABLE g_PrintToConsole = true
// Disable in-game console echoing, only print out to VS output.
#define PRINT_TO_CONSOLE_DISABLE g_PrintToConsole = false
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
#define xyPROFILE( X, Y ) _ScopedProfiler_ _sp##Y##_xx( __FUNCTION__ "\n[" __FILE__ "]" )
#define xyPROFILE2( X, Y ) _ScopedProfiler_ _sp##Y##_x2( __FUNCTION__ " >> \"" X "\"\n[" __FILE__ "]"  )
#define xyzPROFILE( X, Y ) xyPROFILE( X, Y )
#define xyzPROFILE2( X, Y ) xyPROFILE2( X, Y )
#define PROFILE xyzPROFILE( x, __LINE__ )
#define PROFILE2( BLOCK_NAME ) xyzPROFILE2( BLOCK_NAME, __LINE__ )

//Output all profiling information
#define PROFILE_DUMP_RECORDINGS PDump()
void PDump();

#else
#define PROFILE_BEGIN
#define PRINT_TO_CONSOLE_ENABLE 
#define PRINT_TO_CONSOLE_DISABLE
#define PRINT( FMT, ... )
#define PROFILE
#define PROFILE2( BLOCK_NAME )
#define PROFILE_DUMP_RECORDINGS
#define ASSERT( EXP, FMT, ... ) ( EXP )
#define ASSERT_WARN( EXP, FMT, ... ) ( EXP )
#define VERIFY( EXP, CONDITION, FMT, ... ) ( EXP )
#define PROFILE_DUMP_RECORDINGS
#endif

#endif//DEBUG_H