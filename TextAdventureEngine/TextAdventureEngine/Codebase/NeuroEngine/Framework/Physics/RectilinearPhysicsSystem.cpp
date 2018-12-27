#include "RectilinearPhysicsSystem.hpp"

//=====================================================================================
const PosInt32 RectilinearPhysicsSystem::DirectionToPosInt32Mapping[ 4 ] =
{
	{ +1, +0 },
	{ +0, +1 },
	{ -1, +0 },
	{ +0, -1 },
};

//=====================================================================================
bool RectilinearPhysicsSystem::CheckAgainstAll( const PhysObject & a_Object ) const
{
	const auto & targCollisionShapes = a_Object->m_CollisionShapes;

	auto objIt = m_Objects.First();
	auto objEnd = m_Objects.End();

	while ( objIt != objEnd )
	{
		if ( *objIt == a_Object )
		{
			++objIt;
			continue;
		}

		const auto & collisionShapes = ( *objIt )->m_CollisionShapes;
		auto csIt = collisionShapes.First();
		auto csEnd = collisionShapes.End();

		while ( csIt != csEnd )
		{
			RectInt32 csItOffset = ( *csIt ) + ( *objIt )->m_Position;

			auto targCsIt = targCollisionShapes.First();
			auto targCsEnd = targCollisionShapes.End();

			while ( targCsIt != targCsEnd )
			{
				RectInt32 targCsItOffset = ( *targCsIt ) + a_Object->m_Position;

				if ( CheckRectOverlap( targCsItOffset, csItOffset ) )
				{
					return false;
				}

				++targCsIt;
			}

			++csIt;
		}

		++objIt;
	}

	return true;
}

//=====================================================================================
RaycastInfo RectilinearPhysicsSystem::RayCast( const Vector2 & a_Origin, const Vector2 & a_Direction ) const
{
	RaycastInfo closest = { 999.0F, Vector2::ZERO };

	auto objIt = m_Objects.First();
	auto objEnd = m_Objects.End();

	while ( objIt != objEnd )
	{
		const auto & collisionShapes = ( *objIt )->m_CollisionShapes;
		auto csIt = collisionShapes.First();
		auto csEnd = collisionShapes.End();

		while ( csIt != csEnd )
		{
			RectInt32 csItOffset = ( *csIt ) + ( *objIt )->m_Position;

			
			auto f = RayBoxIntersect( a_Origin, a_Direction, 
							Vector2( csItOffset.Min.XPos, csItOffset.Min.YPos ),
							Vector2( csItOffset.Max.XPos + 1.0F, csItOffset.Max.YPos + 1.0F ) );

			if ( closest.m_Distance > f.m_Distance )
			{
				closest = f;
			}

			++csIt;
		}

		++objIt;
	}

	return closest;
}

//=====================================================================================
RectilinearPhysicsSystem::PhysObject RectilinearPhysicsSystem::CreatePhysObject( const RectArray & a_CollisionShapes, const PosInt32 & a_Position )
{
	POData pod;
	pod.m_CollisionShapes = a_CollisionShapes;
	pod.m_Position = a_Position;
	auto ptr = PPhysObject( pod );
	m_Objects.Append( ptr );
	return PhysObject( ptr );
}

//=====================================================================================
void RectilinearPhysicsSystem::DestroyPhysObject( const PhysObject & a_Ref )
{
	m_Objects.Remove( a_Ref );
}

//=====================================================================================
bool RectilinearPhysicsSystem::MovePhysObject( const PhysObject & a_Ref, Direction a_Direction )
{
	if ( a_Ref )
	{
		PosInt32 prev = a_Ref->m_Position;
		a_Ref->m_Position = a_Ref->m_Position + DirectionToPosInt32Mapping[ a_Direction ];

		if ( !CheckAgainstAll( a_Ref ) )
		{
			a_Ref->m_Position = prev;
			return false;
		}

		return true;
	}

	return false;
}

//=====================================================================================
bool RectilinearPhysicsSystem::CheckRectOverlap( const RectInt32 & a_Rect1, const RectInt32 & a_Rect2 ) const
{
	return 
		a_Rect1.Min.XPos < ( a_Rect2.Max.XPos + 1 ) &&
		( a_Rect1.Max.XPos + 1 ) > a_Rect2.Min.XPos &&
		a_Rect1.Min.YPos < ( a_Rect2.Max.YPos + 1 ) &&
		( a_Rect1.Max.YPos + 1 ) > a_Rect2.Min.YPos;
}