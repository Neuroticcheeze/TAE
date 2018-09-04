#pragma once

#include <[Tool] Dialogue Editor/Structure/Object.hpp>

class BlackboardValueObject : public Object
{
public:

	static void Expose( Exposer * a_Exposer )
	{
		a_Exposer->Declaration( "Blackboard Value", Colour::COBALT.Lerp( Colour::GRAY, 0.5F ) )
			->Field( "Value Name" )
			->Input( "Blackboard", Exposer::PlugType::STRING )
			->Output( "Number", Exposer::PlugType::NUMERIC )
			->Output( "String", Exposer::PlugType::STRING )
			;
	}

private:
};