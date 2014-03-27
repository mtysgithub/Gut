#pragma once

#include "Gut.h"
#include "IExWin32.h"
#include "IGutWin32.h"
#include <windows.h>

#if GUT_WIN32_OO

#define GUT_WND_MAXNUM 1 << 8

HRESULT IGutEx_Create_3DWnd(IN int _wndWidth, IN int _wndHeight, IN bool _bIsFullScreen, OUT HWND *_pWnd, OUT int &_wndIdx)
{
	return E_FAIL;
}

HRESULT IGutEx_Destroy_3DWnd(IN int _wndIdx)
{
	return E_FAIL;
}

class Gut3DWndMgrWin32 : public IGut3DWndMgr
{
protected:
	struct Gut3DWnd
	{
		Gut3DWnd(){memset(this, 0, sizeof(Gut3DWnd));}
		bool m_bMsgLoopActive;
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		HANDLE m_ThreadHandle;
		unsigned long m_ThreadID;
	};

public:
	static Gut3DWndMgrWin32 *Instance();

#pragma region IGut3DWndMgr

	virtual HRESULT CreateWnd(IN const char *pWndName, IN GutWndMode wndMod, OUT HWND **ppWnd, OUT int &wndIdx)
	{
		WNDCLASS window_class;
		memset(&window_class, 0, sizeof(WNDCLASS));

		window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = WndMsgProc;
		window_class.hInstance = GetModuleHandle(NULL);
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		window_class.lpszClassName = pWndName;
		RegisterClass(&window_class);

		DWORD window_style;
		if ( g_GutCallBack.OnSize )
		{
			window_style = WS_OVERLAPPEDWINDOW;
		}
		else
		{
			window_style = WS_BORDER | WS_SYSMENU;
		}

		RECT window_rect;
		SetRect(&window_rect, m_DefaultWndPosX, m_DefaultWndPosY, m_DefaultWndPosX + m_DefaultWndWidth, m_DefaultWndPosY + m__DefaultWndHeight);
		AdjustWindowRect(&window_rect, window_style, FALSE);
		HWND window_handle = CreateWindowEx(
			WS_EX_APPWINDOW,
			pWndName,
			pWndName,
			window_style,
			window_rect.left,
			window_rect.top,
			window_rect.right - window_rect.left,
			window_rect.bottom - window_rect.top,
			NULL,
			NULL,
			window_class.hInstance,
			NULL
			);

		if(!window_handle) return E_FAIL;

		Gut3DWnd *pGutWnd = new Gut3DWnd();
		pGutWnd->m_hWnd = window_handle;
		pGutWnd->m_hInstance = window_class.hInstance;
		pGutWnd->m_bMsgLoopActive = true;
		m_ppWndBuff[m_WndCount++] = pGutWnd;

		ShowWindow(window_handle, SW_SHOWNORMAL);
		SetActiveWindow(window_handle);
	}

	virtual HRESULT CloseWnd(IN int wndIdx)
	{
		//TODO.
		return S_FALSE;
	}

	virtual HRESULT ProcMsg(IN int wndIdx)
	{
		//TODO.
		return S_FALSE;
	}

#pragma endregion

	static LRESULT WINAPI WndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		//TODO.
		return 0L;
	}

protected:

private:

	Gut3DWndMgrWin32();
	virtual ~Gut3DWndMgrWin32();

private:
	Gut3DWnd **m_ppWndBuff;
	int m_WndCount;
	
	static const int m_DefaultWndPosX = 0;
	static const int m_DefaultWndPosY = 0;

	static const int m_DefaultWndWidth = 800;
	static const int m__DefaultWndHeight = 600;
};

extern Gut3DWndMgrWin32 *pGut3DWndMgrWin32 = NULL;

#endif

#if GUT_WIN32_OP

HWND GutGetWindowHandleWin32(void);
void GutSetWindowHandleWin32(HWND hWnd);
void GutCloseWindow(void);
HINSTANCE GutGetWindowInstanceWin32(void);
void GutGetWindowSize(int &w, int &h);

#endif
