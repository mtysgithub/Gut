#pragma once
#include "stdafx.h"

#if _GUT_WIN32_OO_

enum GutWndMode
{
	GutWnd_Normal,
	GutWnd_FullScreen
};

_GUT_INTERFACE_ IGut3DWndMgr
{
	virtual HRESULT CreateWnd(IN const char *pWndName, IN GutWndMode wndMod, OUT HWND *pWnd, OUT int &wndIdx) = 0;
	virtual HRESULT CloseWnd(IN int wndIdx) = 0;
	virtual HRESULT ProcMsg(IN int wndIdx) = 0;
};

#endif