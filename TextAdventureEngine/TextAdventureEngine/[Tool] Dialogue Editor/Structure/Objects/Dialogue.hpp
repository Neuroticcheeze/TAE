#pragma once

#include <[Tool] Dialogue Editor/Structure/Object.hpp>

class DialogueObject : public Object
{
public:

	void Expose( Exposer * a_Exposer )
	{
		a_Exposer->Declaration( "Dialogue", Colour::ORANGE.WithSaturation( 0.5F ) )
			->Field( "Dialogue String ID",	&m_Dialogue )
			->Field( "Value Name",	&m_ValueName )
			->Input( "Decision",		&m_Ref )
			->Input( "Decision 2",		&m_Ref )
			->Output( "Link",	&m_Ref )
			->Output( "Link 1",	&m_Ref )
			->Output( "Link 2",	&m_Ref )
			->Output( "Link 3",	&m_Ref )
			;
	}

private:
	
	CString m_Dialogue;
	CString m_ValueName;
	Colour m_TestColour;
	Ref m_Ref;
};