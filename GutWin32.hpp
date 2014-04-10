#pragma once

#include "stdafx.h"
#include "Gut.h"
#include "GutWin32def.h"
#include "IExWin32.h"
#include "IGutWin32.h"

#if _GUT_WIN32_OP_
HWND GutGetWindowHandleWin32(void);
void GutSetWindowHandleWin32(HWND hWnd);
void GutCloseWindow(void);
HINSTANCE GutGetWindowInstanceWin32(void);
void GutGetWindowSize(int &w, int &h);
#endif


#if _GUT_WIN32_OO_

#define GUT_WND_MAXNUM (1 << 8)

class Gut3DWndMgrWin32 : public IGut3DWndMgr
{
public:
	static Gut3DWndMgrWin32 *Instance();

#pragma region IGut3DWndMgr

	virtual HRESULT CreateWnd(IN const char *pWndName, IN int _wndPosX, IN int _wndPosY, IN int _wndWidth, IN int _wndHeight, IN GutWndMode wndMod, OUT Gut3DWnd **ppGutWnd, OUT int &wndIdx)
	{
		WNDCLASS window_class;
		memset(&window_class, 0, sizeof(WNDCLASS));

		window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = __StaticWndMsgProc;
		window_class.hInstance = GetModuleHandle(NULL);
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

		/*http://bbs.csdn.net/topics/390631479?page=1*/
		window_class.lpszClassName = _TEXT(pWndName);

		RegisterClass(&window_class);

		DWORD window_style = WS_OVERLAPPEDWINDOW;

		RECT window_rect;
		if (0 <= _wndPosX && 0 <= _wndPosY &&  0 <= _wndWidth && 0 <= _wndHeight)
		{
			m_DefaultWndPosX = _wndPosX;
			m_DefaultWndPosY = _wndPosY;
			m_DefaultWndWidth = _wndWidth;
			m_DefaultWndHeight = _wndHeight;
		}
		SetRect(&window_rect, m_DefaultWndPosX, m_DefaultWndPosY, m_DefaultWndPosX + m_DefaultWndWidth, m_DefaultWndPosY + m_DefaultWndHeight);
		AdjustWindowRect(&window_rect, window_style, FALSE);

		/*http://bbs.csdn.net/topics/100033541*/
		HWND window_handle = CreateWindowEx(
			WS_EX_APPWINDOW,
			_TEXT(pWndName),
			_TEXT(pWndName),
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

		if(!window_handle)
		{
			int erroCodoc = GetLastError();
			printf("CreateWindowEx => %d\n", erroCodoc);
			return E_FAIL;
		}

		wndIdx = m_WndCount;
		Gut3DWnd *pTmpGutWnd = new Gut3DWnd();

		pTmpGutWnd->m_hWnd = window_handle;
		pTmpGutWnd->m_hWndClass = window_class;
		pTmpGutWnd->m_hInstance = window_class.hInstance;
		pTmpGutWnd->m_bMsgLoopActive = true;
		m_ppWndBuff[m_WndCount++] = pTmpGutWnd;

		*ppGutWnd = pTmpGutWnd;

		ShowWindow(window_handle, SW_SHOWNORMAL);
		SetActiveWindow(window_handle);

		return S_OK;
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

protected:

private:

	Gut3DWndMgrWin32();
	virtual ~Gut3DWndMgrWin32();

	static LRESULT WINAPI __StaticWndMsgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
	switch (message)
	{
	case WM_DESTROY:
		{

			break;
		}

	case WM_SIZE:
		{

			break;
		}

	case WM_PAINT:
		{

			break;
		}

	default:
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}
	}

	return 0;
	}

private:
	Gut3DWnd **m_ppWndBuff;
	int m_WndCount;
	
	int m_DefaultWndPosX;
	int m_DefaultWndPosY;

	int m_DefaultWndWidth;
	int m_DefaultWndHeight;
};

extern Gut3DWndMgrWin32 *pGut3DWndMgrWin32;

#endif