#pragma once
#ifndef TREE_H
#define TREE_H

//=====================================================================================
#include <Framework/Utils/MemoryOps.hpp>
#include <Framework/Math/Math.hpp>
#include <Framework/Template/Cloneable.hpp>
#include <Framework/Utils/Function.hpp>

//=====================================================================================
template< typename T, typename INTEGER_TYPE, INTEGER_TYPE MAX_NODES = 0, INTEGER_TYPE MAX_DEPTH = 0 >
class Tree final
{
public:

	static const INTEGER_TYPE INF = 0;

	struct Node : public Cloneable< Node * >
	{
		friend class Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH >;

		T Value;

		inline Node * GetParent()
		{
			return m_Parent;
		}

		inline const Node * GetParent() const
		{
			return m_Parent;
		}
		
		void operator+=( T a_Value )
		{
			if ( ( m_Owner->m_MaxDepth == INF || m_Depth < m_Owner->m_MaxDepth ) &&
				 ( m_Owner->m_MaxNodes == INF || m_ChildrenCount < m_Owner->m_MaxNodes ) )
			{
				if ( ASSERT( ( m_ChildrenCount > 0 ) == ( m_Children != nullptr ), "[%s] Broken child state; memory leaks likely!", __FUNCTION__ ) )
				{
					const INTEGER_TYPE prev = m_ChildrenCount;
					const INTEGER_TYPE curr = ++m_ChildrenCount;

					if ( m_Children == nullptr )
					{
						m_Children = new Node*[ curr ];
					}

					else
					{
						Node ** tmp = m_Children;
						m_Children = new Node*[ curr ];
						BCopy( tmp, m_Children, curr * sizeof( Node* ) );
						delete[] tmp;
					}

					m_Children[ prev ] = new Node( m_Owner, m_Depth + 1, this );
					m_Children[ prev ]->Value = a_Value;
				}
			}
		}

		void operator-=( INTEGER_TYPE a_Index )
		{
			ASSERT( ( m_ChildrenCount > 0 ) == ( m_Children != nullptr ), "[%s] Broken child state; memory leaks likely!", __FUNCTION__ );
			if ( m_ChildrenCount > 0 && 
				 m_Children && 
				 InRange( a_Index, 
						  static_cast< INTEGER_TYPE >( 0 ), 
						  ( INTEGER_TYPE )( m_ChildrenCount - static_cast< INTEGER_TYPE >( 1 ) ) ) )
			{
				const INTEGER_TYPE prev = m_ChildrenCount;
				const INTEGER_TYPE curr = --m_ChildrenCount;

				if ( curr == 0 )
				{
					for ( INTEGER_TYPE c = 0; c < m_ChildrenCount; ++c )
					{
						delete *( m_Children + c );
					}

					delete[] m_Children;
					m_Children = nullptr;
				}

				else
				{
					Node ** tmp = m_Children;

					auto tmpNode = *( tmp + a_Index );
					*( tmp + a_Index ) = *( tmp + curr );
					*( tmp + curr ) = tmpNode;

					m_Children = new Node*[ curr ];
					BCopy( tmp, m_Children, curr * sizeof( Node* ) );
					delete *( tmp + curr );
					delete[] tmp;
				}
			}
		}

		Node * operator[]( INTEGER_TYPE a_Index )
		{
			return ( m_ChildrenCount > static_cast< INTEGER_TYPE >( 0 ) && 
					 InRange( a_Index, static_cast< INTEGER_TYPE >( 0 ), ( INTEGER_TYPE )( m_ChildrenCount - static_cast< INTEGER_TYPE >( 1 ) ) ) ) ? 
						m_Children[ a_Index ] :
						nullptr;
		}

		const Node * operator[]( INTEGER_TYPE a_Index ) const
		{
			return ( m_ChildrenCount > static_cast< INTEGER_TYPE >( 0 ) && 
					 InRange( a_Index, static_cast< INTEGER_TYPE >( 0 ), ( INTEGER_TYPE )( m_ChildrenCount - static_cast< INTEGER_TYPE >( 1 ) ) ) ) ? 
						m_Children[ a_Index ] :
						nullptr;
		}

		inline INTEGER_TYPE GetChildCount() const
		{
			return m_ChildrenCount;
		}

		inline INTEGER_TYPE GetDepth() const
		{
			return m_Depth;
		}

		Node * Clone() const override
		{
			Node * out = new Node( m_Owner, m_Depth, m_Parent );
			out->Value = Value;
			out->m_ChildrenCount = m_ChildrenCount;
			out->m_Children = out->m_ChildrenCount > 0 ? new Node*[ out->m_ChildrenCount ] : nullptr;

			Node ** it = out->m_Children;
			Node * const * end = out->m_Children + out->m_ChildrenCount;
			while ( it != end )
			{
				*it = ( *( m_Children + ( it - out->m_Children ) ) )->Clone();
				++it;
			}

			return out;
		}

	private:

		Node( const Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH > * a_Owner,
			  INTEGER_TYPE a_Depth,
			  const Node * a_Parent = nullptr )
			: m_ChildrenCount( 0 )
			, m_Children( nullptr )
			, m_Parent( a_Parent )
			, m_Owner( a_Owner )
			, m_Depth( m_Parent ? a_Depth : 0 )
			, Value( T() )
		{}

		~Node()
		{
			for ( INTEGER_TYPE c = 0; c < m_ChildrenCount; ++c )
			{
				delete *( m_Children + c );
			}

			delete[] m_Children;
			m_Children = nullptr;
		}

		INTEGER_TYPE m_Depth;
		INTEGER_TYPE m_ChildrenCount;
		Node ** m_Children;
		const Node * m_Parent;
		const Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH > * m_Owner;
	};

public:

	Tree()
		: m_Root( nullptr )
	{}

	Tree( T a_RootValue )
		: m_Root( new Node( this, 0, nullptr ) )
	{
		m_Root->Value = a_RootValue;
	}

	Tree( const Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH > & a_Other )
	{
		*this = a_Other;
	}

	~Tree()
	{
		Clear();
	}

	Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH > & operator=( const Tree< T, INTEGER_TYPE, MAX_NODES, MAX_DEPTH > & a_Other )
	{
		if ( m_Root )
		{
			Clear();
		}

		if ( a_Other.m_Root )
		{
			m_Root = a_Other.m_Root->Clone();
		}

		return *this;
	}

	inline Node * GetRoot() const
	{
		return m_Root;
	}

	void Clear()
	{
		delete m_Root;
		m_Root = nullptr;
	}

	Node * Find( const T & a_Value )
	{
		return FindRecursively( m_Root, a_Value );
	}

	const Node * Find( const T & a_Value ) const
	{
		return FindRecursively( m_Root, a_Value );
	}

	template< typename UserData >
	Node * Find( Predicate< const T &, UserData > a_Predicate, UserData a_UserData )
	{
		return FindRecursively< UserData >( m_Root, a_Predicate, a_UserData );
	}

	template< typename UserData >
	const Node * Find( Predicate< const T &, UserData > a_Predicate, UserData a_UserData ) const
	{
		return FindRecursively< UserData >( m_Root, a_Predicate, a_UserData );
	}

private:

	template< typename UserData >
	Node * FindRecursively( Node * a_From, Predicate< const T &, UserData > a_Predicate, UserData a_UserData ) const
	{
		if ( a_From && a_Predicate )
		{
			if ( a_Predicate( a_From->Value, a_UserData ) )
			{
				return a_From;
			}
			
			const uint32_t num = a_From->GetChildCount();
			for ( uint32_t k = 0; k < num; ++k )
			{
				Node * result = FindRecursively< UserData >( ( *a_From )[ k ], a_Predicate, a_UserData );

				if ( result )
				{
					return result;
				}
			}
		}

		return nullptr;
	}

	Node * FindRecursively( Node * a_From, const T & a_Value ) const
	{
		if ( a_From )
		{
			if ( a_From.Value == a_Value )
			{
				return a_From;
			}
			
			const uint32_t num = a_From->GetChildrenCount();	
			for ( uint32_t k = 0; k < num; ++k )
			{
				Node * result = FindRecursively( ( *a_From )[ k ], a_Value );

				if ( result )
				{
					return result;
				}
			}
		}

		return nullptr;
	}

	Node * m_Root;

	const INTEGER_TYPE m_MaxNodes = MAX_NODES;
	const INTEGER_TYPE m_MaxDepth = MAX_DEPTH;
};

#endif//TREE_H