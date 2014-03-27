//全局导出函数在此定义
#include <windows.h>

/*
	3D窗体管理函数
*/
HRESULT IGutEx_Create_3DWnd(IN int _wndWidth, IN int _wndHeight, OUT HWND *_pWnd);
HRESULT IGutEx_Destroy_3DWnd(IN HWND _wnd);