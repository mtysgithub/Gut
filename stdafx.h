// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// 在此处引用程序需要的其他头文件
#include "TCHAR.H"
#include <GL/gl.h>
#include <d3d9.h>
#include <string>
#include <iostream>
using namespace std;

// 宏定义
#ifdef WIN32
	#define _GUT_INTERFACE_ __interface
#else
	#define _GUT_INTERFACE_ class
#endif

#define _GUT_WIN32_OO_ 1
#define _GUT_WIN32_OP_ 1 