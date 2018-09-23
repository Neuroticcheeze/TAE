#pragma once
#ifndef VIEWNODEPLUG_UI
#define VIEWNODEPLUG_UI

#include <Framework/UI/View.hpp>
#include <[Tool] Dialogue Editor/DialogueEditor.hpp>

//=====================================================================================
class ViewNodePlug : public View
{
public:

	ENUMCLASS( Type, uint8_t )
		INPUT,
		OUTPUT,
	END_ENUMCLASS( Type, uint8_t )

	ViewNodePlug( uint32_t a_Id, uint32_t a_Sub, Type a_Type, const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	inline bool IsBeingDragged() const { return m_Drag; }
	inline Type GetPlugType() { return m_Type; }

	void OnTick( float a_DeltaTime ) override;

	ViewNodePlug * GetConnection() { return m_Connection; }
	const ViewNodePlug * GetConnection() const { return m_Connection; }

	uint32_t GetNodeId() const { return m_Id; }
	uint32_t GetSub() const { return m_Sub; }

	Functor< void( const ViewNodePlug & ) > OnConnectEventFunctor;

protected:

	void OnEnabled() override;
	void OnDisabled() override;
	void OnMouseEnter( const Vector2 & m_MousePosition ) override;
	void OnMouseLeave( const Vector2 & m_MousePosition ) override;
	void OnMouseClick( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;
	void OnMouseReleased( const Vector2 & m_MousePosition, InputManager::MouseButton a_MouseButton ) override;

private:

	void OnConnectionChanged();

	uint32_t m_Id, m_Sub;
	Type m_Type;
	bool m_Drag;

	ViewNodePlug * m_Connection;

	static Set< ViewNodePlug* > m_GlobalList;
};

#endif//VIEWNODEPLUG_UI