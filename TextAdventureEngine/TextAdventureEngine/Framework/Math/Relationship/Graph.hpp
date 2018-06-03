#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <Framework/Thread/Mutex.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Utils/Tuple.hpp>
#include <Framework/Utils/TypeTraits.hpp>

//=====================================================================================
ENUMCLASS( EdgeType, uint8_t )
	ONE_WAY,
	TWO_WAY,
END_ENUM;

//=====================================================================================
template< typename T >
class IWeightComparator
{
public:

	virtual operator bool() const abstract;
	virtual bool operator>( const T & a_Other ) const abstract;
	
	bool operator<( const T & a_Other ) const
	{
		return a_Other > *this;
	}
	
	bool operator==( const T & a_Other ) const
	{
		return ( ( *this && a_Other ) || ( !( *this ) && !( a_Other ) ) ) && 
			   ! ( *this > a_Other ) && !( *this < a_Other );
	}
	
	bool operator!=( const T & a_Other ) const
	{
		return !( *this == a_Other );
	}

	bool operator<=( const T & a_Other ) const
	{
		return *this < a_Other || *this == a_Other;
	}

	bool operator>=( const T & a_Other ) const
	{
		return *this > a_Other || *this == a_Other;
	}
};

//=====================================================================================
template< typename NODE, typename WEIGHT, typename NodeUserData, typename EdgeUserData >
class Graph final
{
	STATIC_ASSERT( IS_BASE_OF( IWeightComparator< WEIGHT >, WEIGHT ), "Type of WEIGHT is not derived from IWeightComparator" );

public:

	struct Edge final
	{
		friend class Graph< NODE, WEIGHT, NodeUserData, EdgeUserData >;

		EdgeUserData UserData;
		EdgeType Type;
		WEIGHT ForwardWeight;
		WEIGHT BackwardWeight;

		inline const NODE & GetTailNode() const
		{
			return m_TailNode;
		}

		inline const NODE & GetHeadNode() const
		{
			return m_HeadNode;
		}

	private:

		NODE m_TailNode;
		NODE m_HeadNode;
		Graph< NODE, WEIGHT, NodeUserData, EdgeUserData > * m_Owner;
	};
	
	struct Node final
	{
		friend class Graph< NODE, WEIGHT, NodeUserData, EdgeUserData >;

		NodeUserData UserData;
		WEIGHT VisitorWeight;

	private:

		bool m_Visited;
		Graph< NODE, WEIGHT, NodeUserData, EdgeUserData > * m_Owner;
	};

public:


	bool AddNode( NODE a_Identifier, NodeUserData a_UserData = NodeUserData(), WEIGHT a_VisitorWeight = WEIGHT() )
	{
		ScopedLock lock = m_TraverseLock.Lock();

		if ( !m_Nodes.Contains( a_Identifier ) )
		{
			Node node;
			node.UserData = a_UserData;
			node.VisitorWeight = a_VisitorWeight;
			node.m_Visited = false;
			node.m_Owner = this;
			m_Nodes.Put( a_Identifier, node );
			return true;
		}

		return false;
	}

	Node & GetNode( NODE a_Identifier )
	{
		static const Node NULL_NODE = Node();
		auto node = m_Nodes[ a_Identifier ];
		return node ? *node : NULL_NODE;
	}
	
	const Node & GetNode( NODE a_Identifier ) const
	{
		static const Node NULL_NODE = Node();
		auto node = m_Nodes[ a_Identifier ];
		return node ? *node : NULL_NODE;
	}

	inline bool HasNode( NODE a_Identifier ) const
	{
		return m_Nodes.Contains( a_Identifier );
	}

	bool RemoveNode( NODE a_Identifier )
	{
		ScopedLock lock = m_TraverseLock.Lock();

		ClearEdges( a_Identifier );
		return m_Nodes.Remove( a_Identifier );
	}
	
	bool AddEdge( EdgeType a_Type, NODE a_Tail, NODE a_Head, WEIGHT a_ForwardWeight, WEIGHT a_BackwardWeight, EdgeUserData a_UserData = EdgeUserData() )
	{
		ScopedLock lock = m_TraverseLock.Lock();

		typedef Tuple2< NODE, NODE > EdgeNodes;

		Edge * edge = m_Edges.Find< const EdgeNodes & >( []( const EdgeNodes & a_EPs, const Edge & a_Edge ) 
		{
			return ( a_Edge.GetHeadNode() == a_EPs.Value0 && a_Edge.GetTailNode() == a_EPs.Value1 ) || 
				   ( a_Edge.GetHeadNode() == a_EPs.Value1 && a_Edge.GetTailNode() == a_EPs.Value0 );
		}, 
			EdgeNodes( a_Head, a_Tail ) );

		if ( !edge )
		{
			Edge edge2;
			edge2.UserData = a_UserData;
			edge2.Type = a_Type;
			edge2.ForwardWeight = a_ForwardWeight;
			edge2.BackwardWeight = a_BackwardWeight;
			edge2.m_TailNode = a_Tail;
			edge2.m_HeadNode = a_Head;
			edge2.m_Owner = this;
			m_Edges.Append( edge2 );
			return true;
		}

		return false;
	}
	
	Edge & GetEdge( NODE a_Identifier1, NODE a_Identifier2 )
	{
		typedef Tuple2< NODE, NODE > EdgeNodes;

		Edge * edge = m_Edges.Find< const EdgeNodes & >( []( const EdgeNodes & a_EPs, const Edge & a_Edge ) 
		{
			return ( a_Edge.GetHeadNode() == a_EPs.Value0 && a_Edge.GetTailNode() == a_EPs.Value1 ) || 
				   ( a_Edge.GetHeadNode() == a_EPs.Value1 && a_Edge.GetTailNode() == a_EPs.Value0 );
		}, 
			EdgeNodes( a_Head, a_Tail ) );

		static const Edge NULL_EDGE = Edge();
		return edge ? *edge : NULL_EDGE;
	}
	
	const Edge & GetEdge( NODE a_Identifier1, NODE a_Identifier2 ) const
	{
		typedef Tuple2< NODE, NODE > EdgeNodes;

		Edge * edge = m_Edges.Find< const EdgeNodes & >( []( const EdgeNodes & a_EPs, const Edge & a_Edge ) 
		{
			return ( a_Edge.GetHeadNode() == a_EPs.Value0 && a_Edge.GetTailNode() == a_EPs.Value1 ) || 
				   ( a_Edge.GetHeadNode() == a_EPs.Value1 && a_Edge.GetTailNode() == a_EPs.Value0 );
		}, 
			EdgeNodes( a_Head, a_Tail ) );

		static const Edge NULL_EDGE = Edge();
		return edge ? *edge : NULL_EDGE;
	}
	
	bool RemoveEdge( NODE a_Identifier1, NODE a_Identifier2 )
	{
	}
	
	bool GetEdges( NODE a_Identifier, Array< Edge > & o_Edges, bool a_DepartingEdges = true, bool a_ArrivingEdges = true ) const
	{
		if ( !a_DepartingEdges && !a_ArrivingEdges )
		{
			return false;
		}

		bool r = false;

		auto it = m_Edges.First();
		const auto end = m_Edges.End();
		while ( it != end )
		{
			const bool headIs = it->GetHeadNode() == a_Identifier;
			const bool tailIs = it->GetTailNode() == a_Identifier;
			const bool twoWay = it->Type == EdgeType::TWO_WAY;

			if ( headIs || tailIs )
			{
				if ( ( twoWay ) || 
					 ( a_DepartingEdges && tailIs ) ||
					 ( a_ArrivingEdges && headIs ) )
				{
					o_Edges.Append( *it );
					r = true;
				}
			}

			++it;
		}

		return r;
	}
	
	void ClearEdges( NODE a_Identifier, bool a_DepartingEdges = true, bool a_ArrivingEdges = true )
	{
		ScopedLock lock = m_TraverseLock.Lock();

		Array< Edge > edges;
		if ( GetEdges( a_Identifier, edges, a_DepartingEdges, a_ArrivingEdges ) )
		{
			auto it = edges.First();
			const auto end = edges.End();
			while ( it != end )
			{
				m_Edges.Remove< Edge >( *it++ );
			}
		}
	}
	
	void ClearEdges()
	{
		ScopedLock lock = m_TraverseLock.Lock();
		m_Edges.Clear();
	}
	
	void Clear()
	{
		ScopedLock lock = m_TraverseLock.Lock();
		m_Edges.Clear();
		m_Nodes.GetEntries().Clear();
	}

private:

	Dictionary< NODE, Node > m_Nodes;
	Array< Edge >			 m_Edges;

	Mutex m_TraverseLock;
};

#endif//GRAPH_H