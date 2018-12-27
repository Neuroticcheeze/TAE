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

protected:
	void OnEnabled() override {}
	void OnDisabled() override {}

private:

	void OnDrop( ViewDraggable * a_Draggable );
};

#endif//VIEWDROPTARGET_H