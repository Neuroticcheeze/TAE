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
class ViewListSelector;
class ViewDropTarget;
class ViewDraggable;

//=====================================================================================
class View : public InputManager::IMouseEventListener
{
	friend class Page;

public:

	class IActionListener abstract
	{
	public:
		
		virtual void OnEnabled( View * a_View ) {}
		virtual void OnDisabled( View * a_View ) {}
		virtual void OnButtonPress( ViewButton & a_ViewButton ) {}
		virtual void OnTextFieldFocus( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldFocusLost( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldSubmit( ViewTextField & a_ViewTextField ) {}
		virtual void OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue ) {}
		virtual void OnTickBoxValueChanged( ViewTickBox & a_ViewTickBox, bool a_Flag ) {}
		virtual void OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue ) {}
		virtual void OnListSelectionChanged( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex ) {}
		virtual void OnListSelectionConfirmed( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex ) {}
		virtual void OnDragBegin( ViewDraggable & a_ViewDraggable ) {}
		virtual void OnDragEnd( ViewDraggable & a_ViewDraggable ) {}
		virtual void OnDrop( ViewDropTarget & a_ViewDropTarget, ViewDraggable & a_ViewDraggable ) {}
	};

	void AddActionListener( IActionListener * a_ActionListener, bool a_SendInitialEvents = true );
	void RemoveActionListener( IActionListener * a_ActionListener );
	
	void OnEnabled( View * a_View );
	void OnDisabled( View * a_View );
	void OnButtonPress( ViewButton & a_ViewButton );
	void OnTextFieldFocus( ViewTextField & a_ViewTextField );
	void OnTextFieldFocusLost( ViewTextField & a_ViewTextField );
	void OnTextFieldSubmit( ViewTextField & a_ViewTextField );
	void OnTextFieldValueChanged( ViewTextField & a_ViewTextField, const CString & a_StringValue );
	void OnTickBoxValueChanged( ViewTickBox & a_ViewTickBox, bool a_Flag );
	void OnSliderValueChanged( ViewSlider & a_ViewSlider, float a_PreviousValue, float a_NewValue );
	void OnListSelectionChanged( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex );
	void OnListSelectionConfirmed( ViewListSelector & a_ViewListSelector, int32_t a_SelectionIndex );
	void OnDragBegin( ViewDraggable & a_ViewDraggable );
	void OnDragEnd( ViewDraggable & a_ViewDraggable );
	void OnDrop( ViewDropTarget & a_ViewDropTarget, ViewDraggable & a_ViewDraggable );

protected:

	virtual void OnEnabled() {}
	virtual void OnDisabled() {}
	virtual void RequestInitialEvents( IActionListener * a_ActionListener );

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
		UNSPECIFIED,
	END_ENUM_C2( Alignment, uint8_t, 4 )

	void SetBorder( Alignment a_Alignment, float a_Amount );
	void SetBorderPx( Alignment a_Alignment, float a_Amount ); // Set borders by locking their offset from the opposite border in px
	float GetBorder( Alignment a_Alignment );

	void SetBordersFromSizeAndOffset( const Vector2 & a_Size, const Vector2 & a_Offset = Vector2::ZERO );

	inline const Vector2 & GetPosition() const
	{
		return m_Offset;
	}

	inline const Vector2 & GetSize() const
	{
		return m_Size;
	}

	inline Vector2 GetCenter() const
	{
		return m_Offset + m_Size * 0.5F;
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
		a_ChildView->OnParentRectangleChanged( m_Offset, m_Size, this );
		
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

	void SetIsInteractible( bool a_Flag ) { m_Interactible = a_Flag; }
	bool GetIsInteractible() const { return m_Interactible; }

	void SetEnabled( bool a_Flag = true );
	bool GetEnabled() const;

	bool GetMouseOver() const;

protected:

	void SendMessageUpward( uint32_t a_Message );
	virtual bool OnMessage( View * a_Child, uint32_t a_Message ) { return true; }

	virtual void OnReset() {}
	virtual void OnTick( float a_DeltaTime ) {}
	virtual void OnTickPost( float a_DeltaTime ) {}
	virtual void OnMouseEnter( const Vector2 & m_MousePosition ) {}
	virtual void OnMouseLeave( const Vector2 & m_MousePosition ) {}
	virtual void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) {}
	virtual void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) {}
	virtual void OnGainFocus() {}
	virtual void OnLoseFocus() {}

	virtual void OnRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize ) {}
	
	Page * GetContainerPage() { return m_ContainerPage; }
	const Page * GetContainerPage() const { return m_ContainerPage; }

	virtual bool CheckInput( bool a_ReleaseOnly );
	
private:

	bool m_Hover;
	bool m_Interactible;

	void AppendToFrameCacheList();

	void OnMousePressed( InputManager::MouseButton a_MouseButton );
	void OnMouseReleased( InputManager::MouseButton a_MouseButton );
	void OnChildZOrderChanged( const View * a_Child, uint32_t a_ZOrder );
	void OnParentRectangleChanged( const Vector2 & a_TrueOffset, const Vector2 & a_TrueSize, const View * a_Parent );

	void RecalculateTrueRectangle();
	
	bool m_Enabled;
	
	CString m_Name;
	float m_Borders[ AlignmentCount ];
	float m_BorderPxs[ AlignmentCount ];
	Vector2 m_Offset;
	Vector2 m_Size;
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