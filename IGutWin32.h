#pragma once
#include "windows.h"

#define GUT_WIN32_OO 1
#define GUT_WIN32_OP 1 

#if GUT_WIN32_OO

enum GutWndMode
{
	GutWnd_Normal,
	GutWnd_FullScreen
};

__interface IGut3DWndMgr
{
	virtual HRESULT CreateWnd(IN const char *pWndName, IN GutWndMode wndMod, OUT HWND **ppWnd, OUT int &wndIdx) = 0; 
	virtual HRESULT CloseWnd(IN int wndIdx) = 0;
	virtual HRESULT ProcMsg(IN int wndIdx) = 0;
};

#endif