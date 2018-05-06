#include "CommandLineManager.hpp"

//=====================================================================================
void CommandLineManager::Init( const char * a_CommandLine )
{
	m_CommandLine = a_CommandLine ? a_CommandLine : "";

	// Break it down
	m_Arguments.Clear();
	auto cuts = CString( m_CommandLine ).Split( " " );

	for ( uint32_t i = 0; i < cuts.Count(); )
	{
		const CString & str = cuts[ i ];

		CLArg arg;

		// Argument
		if ( str.Length() > 2 && str.SubString( 0, 2 ) == "--" )
		{
			arg.m_Name = str.SubString( 2, str.Length() - 2 );
			++i;

			// Arg param
			if ( i < cuts.Count() && cuts[ i ].Length() > 0 && cuts[ i ].SubString( 0, 2 ) != "--" )
			{
				arg.m_Parameter = cuts[ i ];
				++i;
			}
		}

		m_Arguments.Append( arg );
	}
}

//=====================================================================================
void CommandLineManager::Finalise()
{
	m_CommandLine = nullptr;
	m_Arguments.Clear();
}

//=====================================================================================
bool CommandLineManager::ArgumentExists( const char * a_Name ) const
{
	auto out = m_Arguments.Find< const char * >( []( const char * a_Name, const CLArg & a_Arg )
	{
		return a_Arg.GetName() == a_Name;
	}, a_Name );

	return out != nullptr;
}

//=====================================================================================
CLArg CommandLineManager::GetArgument( const char * a_Name ) const
{
	auto out = m_Arguments.Find< const char * >( []( const char * a_Name, const CLArg & a_Arg )
	{
		return a_Arg.GetName() == a_Name;
	}, a_Name );

	if ( !out )
	{
		return CLArg();
	}

	return CLArg( *out );
}