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

public:

	class ViewPage final : View
	{
		friend class Page;

	private:

		ViewPage( const char * a_Name, Page * a_ContainerPage );
	};

public:

	Page( const char * a_Name, const Pointer< View > & a_RootView );
	Page( const char * a_Name = "" );
	void Tick( float a_DeltaTime );

	inline const char * GetName() const
	{
		return m_Name.Get();
	}

	virtual bool IsBlocking() const
	{
		return true;
	}

	inline Pointer< View > GetRootView() const
	{
		return m_RootView;
	}

protected:

	virtual void OnEnterPage() {}
	virtual void OnExitPage() {}
	virtual void OnTick( float a_DeltaTime ) {}

private:

	Pointer< View > m_RootView;
	CString m_Name;
};

#endif//PAGE_H