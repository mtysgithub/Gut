// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "TCHAR.H"
#include <GL/gl.h>
#include <d3d9.h>
#include <string>
#include <iostream>
using namespace std;

// �궨��
#ifdef WIN32
	#define _GUT_INTERFACE_ __interface
#else
	#define _GUT_INTERFACE_ class
#endif

#define _GUT_WIN32_OO_ 1
#define _GUT_WIN32_OP_ 1 