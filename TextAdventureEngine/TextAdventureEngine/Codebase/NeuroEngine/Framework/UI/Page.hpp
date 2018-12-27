#pragma once
#ifndef PAGE_H
#define PAGE_H

#include <Framework/Utils/MemoryExt.hpp>
#include <Framework/Containers/CString.hpp>
#include "View.hpp"

//=====================================================================================
class Page
{
	friend class PageManager;
	friend class View;

public:

	class ViewPage final : View
	{
		friend class Page;

	private:

		ViewPage( const char * a_Name, Page * a_ContainerPage );

	protected:
		void OnEnabled() override {}
		void OnDisabled() override {}
	};

public:

	Page( const char * a_Name = "" );
	~Page();

	void Tick( float a_DeltaTime );

	inline const char * GetName() const
	{
		return m_Name.Get();
	}

	virtual bool IsBlocking() const
	{
		return true;
	}

	inline View * GetRootView() const
	{
		return m_RootView;
	}

	inline bool IsBeingBlocked() const
	{
		return m_IsBeingBlocked;
	}

protected:

	virtual void OnEnterPage() {}
	virtual void OnExitPage() {}
	virtual void OnTick( float a_DeltaTime ) {}
	virtual void OnTickPost( float a_DeltaTime ) {}

private:

	bool m_IsBeingBlocked = false;
	ViewPage * m_RootView;
	View * m_FocusedView;
	CString m_Name;

	Array< View* > m_AllViews;
};

#endif//PAGE_H