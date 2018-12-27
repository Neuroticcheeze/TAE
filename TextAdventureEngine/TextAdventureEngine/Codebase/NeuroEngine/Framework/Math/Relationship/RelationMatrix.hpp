#pragma once
#ifndef RELATIONMATRIX_H
#define RELATIONMATRIX_H

template< typename T >
class RelationMatrix final
{
public:

	RelationMatrix() 
	{
	}

	RelationMatrix( uint32_t a_Elems )
	{
		if ( a_Elems > 0 )
		{
			m_Size = a_Elems;
			m_Matrix = new T[ m_Size * m_Size ];
		}
	}

	~RelationMatrix()
	{
		CFree( m_Matrix );
		m_Size = 0;
	}

	void SetRelation( uint32_t a_AIdx, uint32_t a_BIdx, const T & a_Relation )
	{
		m_Matrix[ a_AIdx + m_Size * a_BIdx ] = a_Relation;
		m_Matrix[ a_BIdx + m_Size * a_AIdx ] = a_Relation;
	}
	
	const T & GetRelation( uint32_t a_AIdx, uint32_t a_BIdx )
	{
		return m_Matrix[ a_AIdx + m_Size * a_BIdx ];
	}

private:
	T * m_Matrix = nullptr;
	uint32_t m_Size = 0;
};

#endif//RELATIONMATRIX_H