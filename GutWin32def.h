#pragma once

#if _GUT_WIN32_OO_
struct Gut3DWnd
{
	Gut3DWnd(){memset(this, 0, sizeof(Gut3DWnd));}

	bool m_bMsgLoopActive;
	HWND m_hWnd;
	WNDCLASS m_hWndClass;
	HINSTANCE m_hInstance;
	HANDLE m_ThreadHandle;
	unsigned long m_ThreadID;

	void (*OnSize)(int x, int y);
	void (*OnPaint)(void);
	void (*OnIdle)(void);
	void (*OnClose)(void);
};

enum GutWndMode
{
	GutWnd_Normal,
	GutWnd_FullScreen
};

extern HRESULT IGutEx_Create_3DWnd(IN const char *_wndName, IN int _wndPosX, IN int _wndPosY, IN int _wndWidth, IN int _wndHeight, IN bool _bIsFullScreen, OUT Gut3DWnd **_ppGutWnd, OUT int &_wndIdx);
extern HRESULT IGutEx_Destroy_3DWnd(IN int _wndIdx);

#endif