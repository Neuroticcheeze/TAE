#pragma once
#ifndef MEMORYEXT_H
#define MEMORYEXT_H

#include <Framework/Containers/Dictionary.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Utils/Tuple.hpp>
#include <Framework/Utils/Function.hpp>

//=====================================================================================
class GlobalPtrTable final : public Singleton< GlobalPtrTable >
{
private:

	typedef Array< void* > WeakEvents;
	typedef Tuple2< WeakEvents, uint32_t > PtrMeta;

	void * Add( void * a_Pointer )
	{
		PROFILE;

		PtrMeta * meta = m_Refs[ a_Pointer ];

		if ( meta )
		{
			++( meta->Value1 );
		}

		else
		{
			WeakEvents weaks;

			PtrMeta newMeta( weaks, 1 );
			m_Refs.Put( a_Pointer, newMeta );
		}

		return a_Pointer;
	}

	void * AddWeak( void * a_Pointer, void * a_Weak )
	{
		PROFILE;

		PtrMeta * meta = m_Refs[ a_Pointer ];

		if ( meta && a_Weak )
		{	
			meta->Value0.Append( a_Weak );
			return a_Pointer;
		}

		return nullptr;
	}

	template< typename U >
	void * Rem( void * a_Pointer );

	void RemWeak( void * a_UnderlyingPtr, void * a_Weak );

	Dictionary< void *, PtrMeta > m_Refs;

public:

	bool HasRef( void * a_Ptr )
	{
		return m_Refs[ a_Ptr ] != nullptr;
	}

	template< typename T >
	class WeakPointer;

	template< typename T >
	class Pointer
	{
		friend class GlobalPtrTable;

	public:

		typedef T PtrType;
	
		operator bool() const
		{
			return this != nullptr && m_Value != nullptr;
		}

		Pointer< T > & operator=( T * a_Value )
		{
			if ( a_Value )
			{
				*this = Pointer< T >( &a_Value );
			}

			else
			{
				*this = Pointer< T >();
			}
		}

		T * Ptr() const
		{
			return static_cast< T* >( m_Value );
		}
		
		T * operator-> () const
		{
			return static_cast< T* >( m_Value );
		}

		bool operator==( const WeakPointer< T > & a_Other ) const
		{
			return m_Value == a_Other.Ptr();
		}

		bool operator!=( const WeakPointer< T > & a_Other ) const
		{
			return m_Value != a_Other.Ptr();
		}

		Pointer< T > & operator=( const Pointer< T > & a_Other )
		{
			if ( m_Value )
			{
				m_Value = GlobalPtrTable::Instance().Rem< T >( m_Value );
			}

			if ( a_Other )
			{
				m_Value = GlobalPtrTable::Instance().Add( a_Other.m_Value );
			}
	
			else
			{
				m_Value = nullptr;
			}

			return *this;
		}

		Pointer( const Pointer< T > & a_Other )
		{
			*this = a_Other;
		}
	
		explicit Pointer( T a_Value )
		{
			m_Value = GlobalPtrTable::Instance().Add( new T( a_Value ) );
		}

		explicit Pointer( T* a_Value )
		{
			m_Value = GlobalPtrTable::Instance().Add( a_Value );
		}

		Pointer()
			: m_Value( nullptr )
		{

		}
	
		~Pointer()
		{
			m_Value = GlobalPtrTable::Instance().Rem< T >( m_Value );
		}
	
	private:
	
		void * m_Value;
	};

	template< typename T >
	class WeakPointer
	{
	public:

		friend class GlobalPtrTable;
		friend class Pointer< T >;
	
		typedef T PtrType;

		operator bool() const
		{
			return m_Value != nullptr;
		}
		
		T * operator-> () const
		{
			return static_cast< T* >( m_Value );
		}

		WeakPointer( std::nullptr_t )
		{
			*this = WeakPointer< T >();
		}

		WeakPointer< T > & operator=( std::nullptr_t )
		{
			*this = WeakPointer< T >();
			return *this;
		}

		bool operator==( const WeakPointer< T > & a_Other ) const
		{
			return m_Value == a_Other.m_Value;
		}

		bool operator!=( const WeakPointer< T > & a_Other ) const
		{
			return m_Value != a_Other.m_Value;
		}

		bool operator==( const Pointer< T > & a_Other ) const
		{
			return m_Value == a_Other.m_Value;
		}

		bool operator!=( const Pointer< T > & a_Other ) const
		{
			return m_Value != a_Other.m_Value;
		}
	
		explicit WeakPointer( const Pointer< T > & a_Parent )
		{
			if ( a_Parent )
			{
				m_Value = GlobalPtrTable::Instance().AddWeak( (T*)a_Parent.Ptr(), this );
			}

			else
			{
				m_Value = nullptr;
			}
		}

		static WeakPointer< T > Grab( T * a_ParentPtr )
		{
			WeakPointer< T > wp;
			wp.m_Value = GlobalPtrTable::Instance().AddWeak( a_ParentPtr, &wp );
			return wp;
		}

		WeakPointer< T > & operator=( const WeakPointer< T > & a_Other )
		{
			if ( a_Other )
			{
				m_Value = GlobalPtrTable::Instance().AddWeak( a_Other.m_Value, this );
			}
	
			else
			{
				m_Value = nullptr;
			}

			return *this;
		}

		WeakPointer( const WeakPointer< T > & a_Other )
		{
			if ( a_Other )
			{
				m_Value = GlobalPtrTable::Instance().AddWeak( a_Other.m_Value, this );
			}
	
			else
			{
				m_Value = nullptr;
			}
		}

		WeakPointer()
			: m_Value( nullptr )
		{
		}

		~WeakPointer()
		{
			if ( *this )
			{
				GlobalPtrTable::Instance().RemWeak( m_Value, this );
			}
		}

		T * Ptr() const
		{
			return static_cast< T* >( m_Value );
		}
	
	private:

		void OnRem()
		{
			m_Value = nullptr;
		}

		void * m_Value;
	};
};

template< typename U >
void * GlobalPtrTable::Rem( void * a_Pointer )
{
	PROFILE;

	PtrMeta * meta = m_Refs[ a_Pointer ];

	if ( meta )
	{
		if ( meta->Value1 > 1 )
		{
			--meta->Value1;
		}

		else
		{
			meta->Value1 = 0;

			for ( uint32_t i = 0; i < meta->Value0.Count(); ++i )
			{
				static_cast< GlobalPtrTable::WeakPointer< U >* >( meta->Value0[ i ] )->OnRem();
			}
			
			m_Refs.Remove( a_Pointer );

			delete a_Pointer;

			return nullptr;
		}
	}

	return a_Pointer;
}

//=====================================================================================
template < typename P >
using Pointer = GlobalPtrTable::Pointer< P >;

//=====================================================================================
template < typename P >
using WeakPointer = GlobalPtrTable::WeakPointer< P >;

#endif MEMORYEXT_H