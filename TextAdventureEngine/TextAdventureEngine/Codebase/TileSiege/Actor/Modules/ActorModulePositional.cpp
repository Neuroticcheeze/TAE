#include "ActorModulePositional.hpp"

//=====================================================================================
ACTOR_MODULE_CONFIG_POST_ACTOR_MODULE( ActorModulePositional );

//=====================================================================================
void ActorModulePositional::SetPosition( const Vector2 & a_Position )
{
	m_Position = a_Position;
	//TODO callback?
}

//=====================================================================================
const Vector2 & ActorModulePositional::GetPosition() const
{
	return m_Position;
}