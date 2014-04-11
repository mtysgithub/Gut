#pragma once

#if _GUT_WIN32_OO_
struct Gut3DWnd
{
	Gut3DWnd(){memset(this, 0, sizeof(Gut3DWnd));}

	int index;
	HWND m_hWnd;
	bool m_bMsgLoopActive;
	WNDCLASS m_hWndClass;
	HINSTANCE m_hInstance;
	HANDLE m_ThreadHandle;
	unsigned long m_ThreadID;

	void (*OnSize)(int x, int y);
	void (*OnPaint)(void);
	void (*OnClose)(void);
	void (*OnIdle)(void);

	bool Active()
	{
		// `收到过销毁窗口消息的结构体，调用此函数会返回false来结束执行消息循环.`
		return m_bMsgLoopActive;
	}
};

enum GutWndMode
{
	GutWnd_Normal,
	GutWnd_FullScreen
};

extern HRESULT IGutEx_Create_3DWnd(IN const char *_wndName, IN int _wndPosX, IN int _wndPosY, IN int _wndWidth, IN int _wndHeight, IN bool _bIsFullScreen, OUT Gut3DWnd **_ppGutWnd, OUT int &_wndIdx);
extern HRESULT IGutEx_Destroy_3DWnd(IN int _wndIdx);

#endif