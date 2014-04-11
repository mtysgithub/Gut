#pragma once
#include "stdafx.h"
#include "GutWin32def.h"

#if _GUT_WIN32_OO_

_GUT_INTERFACE_ IGut3DWndMgr
{
	virtual HRESULT CreateWnd(IN const char *pWndName, IN int _wndPosX, IN int _wndPosY, IN int _wndWidth, IN int _wndHeight, IN GutWndMode wndMod, OUT Gut3DWnd **ppGutWnd, OUT int &wndIdx) = 0;
	virtual HRESULT ReleaseWnd(IN int wndIdx) = 0;
};

#endif