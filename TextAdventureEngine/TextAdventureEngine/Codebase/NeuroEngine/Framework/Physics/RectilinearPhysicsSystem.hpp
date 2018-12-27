#pragma once
#ifndef RECTILINEARPHYSICSSYSTEM_H
#define RECTILINEARPHYSICSSYSTEM_H

#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/StaticArray.hpp>
#include <Framework/Utils/Memory.hpp>
#include <Framework/Utils/Uuid.hpp>
#include <Framework/Math/Linear/Vector2.hpp>

//=====================================================================================
struct PosInt32
{
	int32_t XPos = 0, YPos = 0;

	PosInt32 operator+( const PosInt32 & a_Other ) const
	{
		PosInt32 out = *this;
		out.XPos += a_Other.XPos;
		out.YPos += a_Other.YPos;
		return out;
	}
};

//=====================================================================================
struct RectInt32
{
	PosInt32 Min, Max;

	RectInt32 operator+( const PosInt32 & a_Other ) const
	{
		RectInt32 out = *this;
		out.Min = out.Min + a_Other;
		out.Max = out.Max + a_Other;
		return out;
	}
};

struct RaycastInfo
{
	float m_Distance;
	Vector2 m_Normal;
};

//=====================================================================================
class RectilinearPhysicsSystem
{
private:

	struct POData;

public:

	typedef WeakPointer< POData > PhysObject;
	typedef Array< RectInt32 > RectArray;

private:

	// The internal data that describes a PhysObject
	struct POData
	{
		friend class RectilinearPhysicsSystem;

		inline const PosInt32 & GetPosition() const { return m_Position; }
		inline const RectArray & GetCollisionShapes() const { return m_CollisionShapes; }

	private:

		PosInt32 m_Position;
		RectArray m_CollisionShapes;
	};

public:

	ENUM( Direction, uint8_t )
		DIR_RIGHT,
		DIR_UP,
		DIR_LEFT,
		DIR_DOWN,
	END_ENUM;

public:

	PhysObject CreatePhysObject( const RectArray & a_CollisionShapes, const PosInt32 & a_Position );
	void DestroyPhysObject( const PhysObject & a_Ref );
	bool MovePhysObject( const PhysObject & a_Ref, Direction a_Direction );

	RaycastInfo RayCast( const Vector2 & a_Origin, const Vector2 & a_Direction ) const;

private:

	RaycastInfo RayBoxIntersect( Vector2 rpos, Vector2 rdir, Vector2 vmin, Vector2 vmax ) const
	{
		float tx1 = ( vmin.x - rpos.x ) / rdir.x;
		float tx2 = ( vmax.x - rpos.x ) / rdir.x;
		float tminx = fminf( tx1, tx2 );
		float tmaxx = fmaxf( tx1, tx2 );
	
		float ty1 = ( vmin.y - rpos.y ) / rdir.y;
		float ty2 = ( vmax.y - rpos.y ) / rdir.y;
		float tminy = fminf( ty1, ty2 );
		float tmaxy = fmaxf( ty1, ty2 );
	
		float tmin = fmaxf( tminx, tminy );
		float tmax = fminf( tmaxx, tmaxy );
	
		if ( tmin <= 0.0F || tmax < 0.0F || tmax < tmin ) return { FLT_MAX, Vector2::ZERO };

		Vector2 n;

		if ( tmin == tx1 )
		{
			n = -Vector2::RIGHT;
		}
		else if ( tmin == tx2 )
		{
			n = Vector2::RIGHT;
		}
		else if ( tmin == ty1 )
		{
			n = -Vector2::UP;
		}
		else if ( tmin == ty2 )
		{
			n = Vector2::UP;
		}

		return { tmin, n };
	}

	typedef Pointer< POData > PPhysObject;
	static const PosInt32 DirectionToPosInt32Mapping[ 4 ];
	bool CheckRectOverlap( const RectInt32 & a_Rect1, const RectInt32 & a_Rect2 ) const;
	bool CheckAgainstAll( const PhysObject & a_Object ) const;
	Array< PPhysObject > m_Objects;
};

#endif//RECTILINEARPHYSICSSYSTEM_H