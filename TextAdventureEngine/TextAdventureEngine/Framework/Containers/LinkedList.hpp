#pragma once
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

//=====================================================================================
static struct LL final
{
	ENUM( NodeFinderFlags, uint8_t )
		FIND_NONE = 0,
		FIND_ABOVE = 1,
		FIND_BELOW = 2,
	END_ENUM_F( NodeFinderFlags, uint8_t )
};

//=====================================================================================
template< typename T >
class LinkedList final
{
public:
	struct Node final
	{
		friend class LinkedList< T >;
		friend struct Iterator;

	public:

		bool IsLast() const
		{
			return m_Next == nullptr;
		}

		bool IsFirst() const
		{
			return m_Prev == nullptr;
		}

		Node * Next()
		{
			return m_Next;
		}

		const Node * Next() const
		{
			return m_Next;
		}

		Node * Prev()
		{
			return m_Prev;
		}

		const Node * Prev() const
		{
			return m_Prev;
		}

		T & Data()
		{
			return m_Data;
		}

		const T & Data() const
		{
			return m_Data;
		}

	private:

		Node()
			: m_Next( nullptr )
			, m_Prev( nullptr )
		{
		}

		~Node()
		{
			if ( m_Next )
			{
				delete m_Next;
			}

			if ( m_Prev )
			{
				m_Prev->m_Next = nullptr;
			}
		}

		Node( const Node & a_Other )
		{
			*this = a_Other;
		}

		Node & operator=( const Node & a_Other )
		{
			if ( !IsLast() )
			{
				delete m_Next;
			}

			if ( !IsFirst() )
			{
				m_Prev->m_Next = nullptr;
			}

			m_Data = a_Other.m_Data;
			m_Next = a_Other.m_Next;
			m_Prev = a_Other.m_Prev;
		}

		bool operator==( const Node & a_Other )
		{
			return m_Data == a_Other.m_Data;
		}

		bool operator!=(const Node & a_Other)
		{
			return !( *this == a_Other );
		}

		T m_Data;
		Node * m_Next;
		Node * m_Prev;
	};

	struct Iterator final
	{
	public:

		Iterator & operator++()
		{
			if ( m_Node && m_Node->m_Next )
			{
				m_Node = m_Node->m_Next;
			}

			return *this;
		}

		Iterator & operator--()
		{
			if ( m_Node && m_Node->m_Prev )
			{
				m_Node = m_Node->m_Prev;
			}

			return *this;
		}

		bool Fwd() const
		{
			return m_Node->m_Next;
		}

		bool Bck() const
		{
			return m_Node->m_Prev;
		}

		Node & operator->()
		{
			if ( m_Node )
			{
				return m_Node;
			}

			return s_Dummy;
		}

		const Node & operator->() const
		{
			if ( m_Node )
			{
				return m_Node;
			}

			return s_Dummy;
		}

	private:

		Node * m_Node;
	};
	
	LinkedList()
		: m_Root( nullptr )
		, m_Length( 0 )
	{

	}

	LinkedList( Node * a_SingleNode, LL::NodeFinderFlags a_Flags = LL::FIND_ABOVE | LL::FIND_BELOW )
	{
		if ( a_SingleNode == nullptr )
		{
			m_Root = nullptr;
			m_Length = 0;
		}

		m_Length = 1;

		Node * cur;
		
		cur = a_SingleNode;

		if ( a_Flags & LL::FIND_ABOVE )
		{
			while ( cur->m_Prev )
			{
				cur = cur->m_Prev;
				++m_Length;
			}
		}

		// Currently, cur holds the root node
		m_Root = cur;

		// If we're ignoring the root node's former parent, then isolate it
		if ( a_Flags ^ LL::FIND_ABOVE )
		{
			m_Root->m_Prev->m_Next = nullptr;
			m_Root->m_Prev = nullptr;
		}

		// If we're ignoring the root node's former child, then isolate it
		if ( a_Flags ^ LL::FIND_BELOW )
		{
			m_Root->m_Next = nullptr;
		}

		if ( a_Flags & LL::FIND_BELOW )
		{
			cur = a_SingleNode;
			while ( cur->m_Next )
			{
				cur = cur->m_Next;
				++m_Length;
			}
		}

		
	}

	LinkedList( const T * a_Data, uint32_t a_Count )
	{
		if ( a_Data && a_Count > 0 )
		{
			m_Length = a_Count;

			Node * newest = new Node();
			m_Root = newest;
			m_Root->m_Data = a_Data[ 0 ];

			for ( uint32_t t = 1; t < a_Count; ++t )
			{
				Node * n = new Node();
				n->m_Data = a_Data[ t ];
				n->m_Prev = newest;
				newest->m_Next = n;
				newest = n;
			}
		}
	}

	LinkedList( const LinkedList< T > & a_Other )
	{
		*this = a_Other;
	}

	~LinkedList()
	{
		delete m_Root;
		m_Length = 0;
	}

	LinkedList< T > & operator=( const LinkedList< T > & a_Other )
	{
	}

	LinkedList< T > & operator+=( const T & a_Value )
	{
	}

	LinkedList< T > & operator--()
	{
	}

	bool operator==( const LinkedList< T > & a_Other ) const
	{
		while ( curr != nullptr )
	}

	bool operator!=( const LinkedList< T > & a_Other ) const
	{
		return !( *this == a_Other );
	}

	Node * Root()
	{
		return m_Root;
	}

	const Node * Root() const
	{
		return m_Root;
	}

	Iterator Begin()
	{
		Iterator it;
		it.m_Node = m_Root;
		return it;
	}

	/*
	LinkedList()
	LinkedList( const T * a_Data, uint32_t a_Count )
	LinkedList( other )
	~LinkedList()
	==
	!=
	=
	+=
	--
	Count()
	Root()
	Find(...)
	Insert( const Node * a_AfterThis, const T & a_Value )
	Erase( const Node * a_AfterThis )
	Node * Isolate( const Node * a_AfterThis )
	*/

private:

	static const Node s_Dummy;
	uint32_t m_Length;
	Node * m_Root;
};

#endif//LINKEDLIST_H