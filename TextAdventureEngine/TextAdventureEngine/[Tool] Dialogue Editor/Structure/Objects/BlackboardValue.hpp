#pragma once

#include <[Tool] Dialogue Editor/Structure/Object.hpp>

class BlackboardValueObject : public Object
{
public:

	void Expose( Exposer * a_Exposer )
	{
		a_Exposer->Declaration( "Blackboard Value", Colour::COBALT.WithSaturation( 0.5F ) )
			->Field( "Blackboard",	&m_Blackboard )
			->Field( "Value Name",	&m_ValueName )
			->Input( "Ref IN",		&m_Ref )
			->Input( "Ref IN",		&m_Ref )
			->Input( "Ref IN",		&m_Ref )
			->Input( "Ref IN",		&m_Ref )
			->Input( "Ref IN",		&m_Ref )
			->Output( "Ref OUT",	&m_Ref )
			;
	}

private:
	
	CString m_Blackboard;
	CString m_ValueName;
	Colour m_TestColour;
	Ref m_Ref;
};