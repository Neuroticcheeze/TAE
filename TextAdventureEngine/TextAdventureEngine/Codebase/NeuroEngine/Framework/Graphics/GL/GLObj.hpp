#pragma once

#include <Framework/Containers/CString.hpp>
#include <Framework/Containers/StaticDictionary.hpp>

//=====================================================================================
#define GLOBJ_RELEASE if ( !ShouldRelease() ) return;

//=====================================================================================
template< typename T >
class GLObj
{
public:

	GLObj() 
		: m_Name( "<Unnamed>" )
		, m_IsValid( false )
	{}

	GLObj( const char * a_Name )
		: m_Name( a_Name )
		, m_IsValid( false )
	{
		GLenum error;
		if ( !ASSERT_WARN( ( error = glGetError() ) == GL_NO_ERROR, "Existing GL Error before creating '%s'", m_Name.Get() ) )
		{
			_CrtDbgBreak();
		}
	}

	void Validate()
	{
		m_IsValid = true;
	}

	bool ShouldRelease() const
	{
		return m_IsValid;
	}

	inline const char * Name() const
	{
		return m_Name.Get();
	}

	inline bool IsValid() const
	{
		return m_IsValid;
	}

protected:

	static const char * GLErrorString( GLenum a_ErrorCode )
	{
		return "";
#ifdef _DEBUG
		static StaticDictionary< GLenum, const char *, 16 > * errors = nullptr;
		if ( !errors )
		{
			errors = new StaticDictionary< GLenum, const char *, 16 >();
#define ERRORSTR( ERROR ) errors->Put( ERROR, #ERROR )
			ERRORSTR( GL_NO_ERROR );
			ERRORSTR( GL_INVALID_ENUM );
			ERRORSTR( GL_INVALID_VALUE );
			ERRORSTR( GL_INVALID_OPERATION );
			ERRORSTR( GL_STACK_OVERFLOW );
			ERRORSTR( GL_STACK_UNDERFLOW );
			ERRORSTR( GL_OUT_OF_MEMORY );
			ERRORSTR( GL_INVALID_FRAMEBUFFER_OPERATION );
			ERRORSTR( GL_CONTEXT_LOST );
			ERRORSTR( GL_TABLE_TOO_LARGE );
#undef ERRORSTR
		}

		return *( *errors )[ a_ErrorCode ];
#else
		return "";
#endif
	}

private:

	bool m_IsValid;
	CString m_Name;
};