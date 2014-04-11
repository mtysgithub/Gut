#include "stdafx.h"
#include "Gut.h"
#include "GutWin32def.h"
#include "GutWin32.hpp"

#if _GUT_WIN32_OO_

HRESULT IGutEx_Create_3DWnd(IN const char *_wndName, IN int _wndPosX, IN int _wndPosY, IN int _wndWidth, IN int _wndHeight, IN bool _bIsFullScreen, OUT Gut3DWnd **_ppGutWnd, OUT int &_wndIdx)
{
	if (!_bIsFullScreen)
	{
		return (Gut3DWndMgrWin32::Instance())->CreateWnd(_wndName, _wndPosX, _wndPosY, _wndWidth, _wndHeight, GutWndMode::GutWnd_Normal, _ppGutWnd, _wndIdx);
	}else
	{
		return (Gut3DWndMgrWin32::Instance())->CreateWnd(_wndName, _wndPosX, _wndPosY, _wndWidth, _wndHeight, GutWndMode::GutWnd_FullScreen, _ppGutWnd, _wndIdx);
	}
}

HRESULT IGutEx_Destroy_3DWnd(IN int _wndIdx)
{
	return (Gut3DWndMgrWin32::Instance())->ReleaseWnd(_wndIdx);
}

Gut3DWndMgrWin32 *Gut3DWndMgrWin32::Instance()
{
	if(!pGut3DWndMgrWin32)
	{
		pGut3DWndMgrWin32 = new Gut3DWndMgrWin32();
	}
	return pGut3DWndMgrWin32;
}

Gut3DWndMgrWin32::Gut3DWndMgrWin32()
{
	m_DefaultWndPosX = 100;
	m_DefaultWndPosY = 100;
	m_DefaultWndWidth = 800;
	m_DefaultWndHeight = 600;

	m_WndCount = 0;
	m_ppWndBuff = new Gut3DWnd *[GUT_WND_MAXNUM];
	memset(m_ppWndBuff, 0, sizeof(m_ppWndBuff[0]));
}

Gut3DWndMgrWin32::~Gut3DWndMgrWin32()
{
	if(m_ppWndBuff) for(int i = 0; i < m_WndCount; ++i) if(m_ppWndBuff[i]) delete m_ppWndBuff[i];
	delete m_ppWndBuff;
}

Gut3DWnd *Gut3DWndMgrWin32::GetGutWnd(int wndIdx)
{
	return m_ppWndBuff[wndIdx];
}
Gut3DWnd *Gut3DWndMgrWin32::GetGutWnd(HWND hWnd)
{
	map<HWND, int>::iterator it = m_mapWnd2Index.find(hWnd);
	if (m_mapWnd2Index.end() != it)
	{
		return m_ppWndBuff[it->second];
	}else
	{
		return NULL;
	}
}

Gut3DWndMgrWin32 *pGut3DWndMgrWin32 = NULL;

#endif

#if _GUT_WIN32_OP_

static bool g_bMsgLoopActive = false;
static HWND g_hWnd = NULL;
static HINSTANCE g_hInstance = NULL;
static HANDLE g_ThreadHandle = NULL;
static unsigned long g_ThreadID = 0;

static int g_iWindowPosX = 100;
static int g_iWindowPosY = 100;
static int g_iWindowWidth = 512;
static int g_iWindowHeight = 512;

void GutSetWindowHandleWin32(HWND hWnd)
{
	g_hWnd = hWnd;
}

HWND GutGetWindowHandleWin32(void)
{
	return g_hWnd;
}

HINSTANCE GutGetWindowInstanceWin32(void)
{
	return g_hInstance;
}

void GutGetWindowSize(int &w, int &h)
{
	w = g_iWindowWidth;
	h = g_iWindowHeight;
}

void GutGetMouseState(int &x, int &y, int button[3])
{
	POINT p;

	GetCursorPos(&p);
	ScreenToClient(g_hWnd, &p);

	x = p.x;
	y = p.y;

	button[0] = GetKeyState(VK_LBUTTON) & 0x80 ? 1 : 0;
	button[1] = GetKeyState(VK_MBUTTON) & 0x80 ? 1 : 0;
	button[2] = GetKeyState(VK_RBUTTON) & 0x80 ? 1 : 0;
}

// `������Ϣ������`
static LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// `�����Ϣ����`
	switch (message)
	{
	case WM_DESTROY:
		{
			/*
			���ǵ�ĳЩOpenGL������ܱ���ΪĳЩͼ���������Ҳ�����̴�ʱ�������PostQuitMessage��Ϣ����ζ���ͷ��������̿ռ䣬
			��������������.   by _mTy 2014.03.21
			*/
			//PostQuitMessage(0);

			g_bMsgLoopActive = false;
			break;
		}

	case WM_SIZE: // `���ڴ�С���ı�`
		{
			int w = LOWORD(lParam);
			int h = HIWORD(lParam);
			g_iWindowWidth = w;
			g_iWindowHeight = h;
			if ( g_GutCallBack.OnSize && GutGetGraphicsDeviceType()!= GUT_UNKNOWN ) 
			{
				// `�����ô��ڴ�С�ı�Ĵ������Ļ�, ��ȥ������.`
				g_GutCallBack.OnSize(w, h);
			}
			break;
		}

	case WM_PAINT: // `������Ҫ�ػ�`
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			if ( g_GutCallBack.OnPaint )
			{
				// `�������ػ���Ϣ�������Ļ�, ��ȥ������.`
				g_GutCallBack.OnPaint();
			}
			break;
		}

	default: // `������Ϣ��ֱ�ӽ������õ���Ϣ������ȥ��`
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}
	}

	return 0;
} // WndProc

void GutCloseWindow(void)
{
	/*
	���ǵ�ĳЩOpenGL������ܱ���ΪĳЩͼ���������Ҳ�����̴�ʱ�������PostQuitMessage��Ϣ����ζ���ͷ��������̿ռ䣬
	��������������.   by _mTy 2014.03.21
	*/
	//PostQuitMessage(0);

	if (g_hWnd && IsWindow(g_hWnd))
	{
		DestroyWindow(g_hWnd);
	}
}

// `���Windows����ϵͳ��������Ϣ`
bool GutProcessMessage(void)
{
	if (false == g_bMsgLoopActive)
	{
		// `�յ��رմ��ڵ���Ϣ�᷵��false������ѭ��.`
		return false;
	}

	MSG	msg;
	// `�����Ƿ��κ���Ϣ`
	if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		// `����Ϣ�Ļ�, �Ͱ����͸���Ϣ������.`
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

bool GutFullScreen(int width, int height)
{
	char *title = "fullscreen";

	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
		return false;
	}


	WNDCLASS	window_class;

	memset(&window_class, 0, sizeof(WNDCLASS));

	// `���ô�������`
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	// `������Ϣ������ΪWndProc`
	window_class.lpfnWndProc = WndProc;
	// `���Ŀǰ���еĳ���`
	window_class.hInstance = GetModuleHandle(NULL);
	// `ʹ��Ĭ�ϵ����`
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	// `������Ϊ��ɫ, ��ʵ�Ƿ�����û������.`
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// `����һ��ע�ᴰ��ʱʹ�õ�����`
	window_class.lpszClassName = title;
	// `�����ϵͳע��һ���µĴ���`
	if (RegisterClass(&window_class) == 0)
		return 0;

	DWORD dwExStyle=WS_EX_APPWINDOW;	// Window Extended Style
	DWORD dwStyle=WS_POPUP;				// Windows Style
	ShowCursor(FALSE);					// Hide Mouse Pointer

	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	g_iWindowPosX = g_iWindowPosY = 0;
	g_iWindowWidth = width;
	g_iWindowHeight = height;

	HWND window_handle = CreateWindowEx(	
		dwExStyle,
		"fullscreen",
		title,
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		0, 0,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL,
		NULL,
		window_class.hInstance,
		NULL
		);

	if (window_handle == NULL)
		return false;

	// `��¼�����������ں�Ŀǰ���г���Ķ���, �Ժ�������õ�.`
	g_hWnd = window_handle;
	g_hInstance = window_class.hInstance;
	// `��ʾ����`
	ShowWindow(window_handle, SW_SHOWNORMAL);
	SetActiveWindow(window_handle);
	SetForegroundWindow(window_handle);	
	SetFocus(window_handle);

	g_bMsgLoopActive = true;
	return true;
}


// `�򿪴���`
// `x, y = �������Ͻǵ�λ��`
// `width, height = ���ڴ�С`
// `title = ���ڱ���������ʾ������`
bool GutCreateWindow(int x, int y, int width, int height, const char *title)
{
	static bool registered = false;

	WNDCLASS window_class;

	memset(&window_class, 0, sizeof(WNDCLASS));

	// `���ô�������`
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	// `������Ϣ������ΪWndProc`
	window_class.lpfnWndProc = WndProc;
	// `���Ŀǰ���еĳ���`
	window_class.hInstance = GetModuleHandle(NULL);
	// `ʹ��Ĭ�ϵ����`
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	// `������Ϊ��ɫ, ��ʵ�Ƿ�����û������.`
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// `����һ��ע�ᴰ��ʱʹ�õ�����`
	window_class.lpszClassName = title;
	// `�����ϵͳע��һ���µĴ���`
	if ( !registered )
	{
		if (RegisterClass(&window_class) == 0)
			return false;
		registered = true;
	}

	DWORD window_style;

	if ( g_GutCallBack.OnSize )
	{
		// `����д��������ŵ���Ϣ, ���������ı䴰�ڴ�С.`
		window_style = WS_OVERLAPPEDWINDOW;
	}
	else
	{
		// `����Ƿ��������ŵ���Ϣ, ���������ı䴰�ڴ�С.`
		window_style = WS_BORDER | WS_SYSMENU;
	}

	if ( x<0 || y<0 )
	{
		// ʹ��Ĭ��ֵ
		x = g_iWindowPosX;
		y = g_iWindowPosY;
	}

	if ( width<0 || height < 0 )
	{
		// ʹ��Ĭ��ֵ
		width = g_iWindowWidth;
		height = g_iWindowHeight;
	}

	// `���㴰�ڵı��߻�ռ���ĵ���, ��ҪʱҪ���Ӵ��ڴ�С.`
	RECT window_rect;
	SetRect(&window_rect, x, y, x+width, y+height);
	AdjustWindowRect(&window_rect, window_style, FALSE);

	g_iWindowPosX = x;
	g_iWindowPosY = y;
	g_iWindowWidth = width;
	g_iWindowHeight = height;

	// `׼���������, ��һ������.`
	HWND window_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		title,
		title,
		window_style,
		window_rect.left,	// x
		window_rect.top,	// y
		window_rect.right - window_rect.left,	// width
		window_rect.bottom - window_rect.top,	// height
		NULL,
		NULL,
		window_class.hInstance,
		NULL
		);

	if (window_handle == NULL)
		return false;

	// `��¼�����������ں�Ŀǰ���г���Ķ���, �Ժ�������õ�.`
	g_hWnd = window_handle;
	g_hInstance = window_class.hInstance;
	// `��ʾ����`
	ShowWindow(window_handle, SW_SHOWNORMAL);
	SetActiveWindow(window_handle);

	g_bMsgLoopActive = true;
	return true;
}
#endif