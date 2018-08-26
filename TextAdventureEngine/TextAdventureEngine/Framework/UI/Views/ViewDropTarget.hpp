#pragma once
#ifndef VIEWDROPTARGET_H
#define VIEWDROPTARGET_H

#include <Framework/UI/View.hpp>

//=====================================================================================
class ViewDropTarget final : public View
{
	friend class ViewDraggable;

public:

	ViewDropTarget( const char * a_Name, Page * a_ContainerPage, View * a_Parent = nullptr );

	void SetEnabled( bool a_Enabled );

	inline bool GetEnabled() const
	{
		return m_Enabled;
	}

private:

	void OnDrop( ViewDraggable * a_Draggable );

	bool m_Enabled;
};

#endif//VIEWDROPTARGET_H