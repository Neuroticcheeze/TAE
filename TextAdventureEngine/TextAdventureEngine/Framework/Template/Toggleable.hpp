#pragma once
#ifndef TOGGLEABLE_H
#define TOGGLEABLE_H

//=====================================================================================
#define CHECK_ENABLED if ( !m_Enabled ) return;
#define CHECK_VALID_E( RETVAL ) if ( !m_Enabled ) return RETVAL;

//=====================================================================================
template< typename T >
class Toggleable
{
public:

	Toggleable()
	{
		Enable();
	}

	void Enable()
	{
		if ( !m_Enabled )
		{
			m_Enabled = true;
			//( ( T* )( this ) )->OnEnabled();
		}

		m_Enabled = true;
	}

	void Disable()
	{
		if ( m_Enabled )
		{
			m_Enabled = false;
			//( ( T* )( this ) )->OnDisabled();
		}

		m_Enabled = false;
	}

	inline bool IsEnabled() const
	{
		return m_Enabled;
	}

protected:

	virtual void OnEnabled() {}
	virtual void OnDisabled() {}

private:

	bool m_Enabled = false;
};

#endif//TOGGLEABLE_H