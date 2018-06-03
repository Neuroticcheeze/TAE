#pragma once
#ifndef VIEW_H
#define VIEW_H

#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/Math/Linear/Vector2.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/Containers/Set.hpp>
#include <Framework/Input/InputManager.hpp>
#include <Framework/Math/Colour/Colour.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
class Page;
class ViewButton;
class ViewTickBox;
class ViewTextField;
class ViewSlider;

//=====================================================================================
class View : public InputManager::IMouseEventListener
{
	friend class Page;

public:

	class IActionListener abstract
	{
	public:

		virtual void OnButtonPress( ViewButton & a_ViewButton ) {}
		virtual void OnButtonDisabled( ViewButton & a_ViewButton ) {}
		virtual void OnButtonEnabled( ViewButton & a_ViewButton ) {}
		virtual void OnTextFieldDisabled( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldEnabled( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldFocus( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldFocusLost( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldSubmit( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue ) {}
		virtual void OnTickBoxValueChanged( ViewTickBox & a_ViewTickBox, bool a_Flag ) {}
		virtual void OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue ) {}
	};

	void AddActionListener( IActionListener * a_ActionListener, bool a_SendInitialEvents = true );
	void RemoveActionListener( IActionListener * a_ActionListener );

	void OnButtonPress( ViewButton & a_ViewButton );
	void OnButtonDisabled( ViewButton & a_ViewButton );
	void OnButtonEnabled( ViewButton & a_ViewButton );
	void OnTextFieldDisabled( ViewTextField & a_ViewTextField );
	void OnTextFieldEnabled( ViewTextField & a_ViewTextField );
	void OnTextFieldFocus( ViewTextField & a_ViewTextField );
	void OnTextFieldFocusLost( ViewTextField & a_ViewTextField );
	void OnTextFieldSubmit( ViewTextField & a_ViewTextField );
	void OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue );
	void OnTickBoxValueChanged( ViewTickBox & a_ViewTickBox, bool a_Flag );
	void OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue );

protected:

	virtual void RequestInitialEvents( IActionListener * a_ActionListener ) {}

private:

	Set< IActionListener * > m_ActionListeners;

public:

	View( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr, bool a_CanFocus = false );
	~View();
	virtual void Destruct() {}

	void SetZOrder( int16_t a_ZOrder );
	int16_t GetZOrder() const;
	void Tick( float a_DeltaTime );

	ENUMCLASS( Alignment, uint8_t )
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
	END_ENUMCLASS( Alignment, uint8_t )

	void SetBorder( Alignment a_Alignment, float a_Amount );
	float GetBorder( Alignment a_Alignment );

	inline void SetSize( const Vector2 & a_Size )
	{
		m_TrueSize = a_Size;
	}

	inline void UseExplicitSize( bool a_Flag = true )
	{
		m_ExplicitSize = a_Flag;
	}
	
	inline bool IsUsingExplicitSize() const
	{
		return m_ExplicitSize;
	}
	
	inline const Vector2 & GetPosition() const
	{
		return m_TrueOffset;
	}

	inline const Vector2 & GetSize() const
	{
		return m_TrueSize;
	}

	inline Vector2 GetCenter() const
	{
		return m_TrueOffset + m_TrueSize * 0.5F;
	}

	inline bool AddChild( View * a_ChildView )
	{
		if ( !a_ChildView || m_ChildrenViews.IndexOf( a_ChildView ) != -1 )
		{
			return false;
		}

		m_ChildrenViews.Append( a_ChildView );
		( *m_ChildrenViews.Last() )->m_ParentView = this;

		a_ChildView->SetZOrder( a_ChildView->GetZOrder() + 1 );
		a_ChildView->SetZOrder( a_ChildView->GetZOrder() - 1 );

		return true;
	}

	inline bool DetatchChild( View * a_ChildView )
	{
		int32_t child = -1;
		if ( !a_ChildView || ( child = m_ChildrenViews.IndexOf( a_ChildView ) ) == -1 )
		{
			return false;
		}

		m_ChildrenViews[ ( uint32_t )child ]->m_ParentView = nullptr;
		m_ChildrenViews.RemoveAt( ( uint32_t )child );
		return true;
	}

	void Reset();

	void SetTint( const Colour & a_Tint )
	{
		m_Tint = a_Tint;
	}
	
	const Colour & GetTint() const
	{
		return m_Tint;
	}

	inline const CString & GetName() const
	{
		return m_Name;
	}

	bool operator==( const View & a_Other ) const
	{
		return WSID( m_Name.Get() ) == WSID( a_Other.m_Name.Get() );
	}

	bool operator!=( const View & a_Other ) const
	{
		return !( *this == a_Other );
	}

protected:

	virtual void OnReset() {}
	virtual void OnTick( float a_DeltaTime ) {}
	virtual void OnMouseEnter( const Vector2 & m_MousePosition ) {}
	virtual void OnMouseLeave( const Vector2 & m_MousePosition ) {}
	virtual void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) {}
	virtual void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) {}
	virtual void OnGainFocus() {}
	virtual void OnLoseFocus() {}

	bool HasFocus() const;
	View * GetViewWithFocus() const;

	inline View * GetParent()
	{
		return m_ParentView;
	}

	inline const View * GetParent() const
	{
		return m_ParentView;
	}

	inline bool IsBeingGrabbed() const
	{
		return m_IsGrabbed;
	}

protected:

	virtual void OnRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize ) {}
	
private:

	void OnMousePressed( InputManager::MouseButton a_MouseButton );
	void OnMouseReleased( InputManager::MouseButton a_MouseButton );
	void OnChildZOrderChanged( const View * a_Child, uint32_t a_ZOrder );
	void OnParentRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize, const View * a_Parent );

	void RecalculateTrueRectangle();
	CString m_Name;
	float m_Borders[ AlignmentCount ];
	bool m_ExplicitSize;
	Vector2 m_TrueOffset;
	Vector2 m_TrueSize;
	Array< View * > m_ChildrenViews;
	View * m_ParentView;
	Page * m_ContainerPage;
	int16_t m_ZOrder;
	bool m_PrevMouseEntered;
	Colour m_Tint;
	bool m_CanFocus;
	bool m_IsGrabbed;

	static Comparer< View * const & > ZSorter;
};

#endif//VIEW_H