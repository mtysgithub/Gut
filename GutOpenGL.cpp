#include <windows.h>
#include <stdio.h>

#include "glew.h"
#include "wglew.h"
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutOpenGL.h"
#include "GutFileUtility.h"

static HDC g_hDC = NULL;
static HGLRC g_hGLRC = NULL;

// make my own glProgramLocalParameters4fvEXT
void APIENTRY _my_glProgramLocalParameters4fvEXT(GLenum target, GLuint index, GLsizei count, const GLfloat* params)
{
	for ( int i=0; i<count; i++ )
	{
		glProgramLocalParameter4fvARB(target, index+i, params + 4*i);
	}
}

static void FixupGLExt(void)
{
	if ( glProgramLocalParameters4fvEXT==NULL && glProgramLocalParameter4fvARB )
	{
		glProgramLocalParameters4fvEXT = _my_glProgramLocalParameters4fvEXT;
	}

	/*
	if ( glBindBuffer==NULL && glBindBufferARB)
	{
		glBindBuffer = glBindBufferARB;
		glBufferData = glBufferDataARB;
		glBufferSubData = glBufferSubDataARB;
		glDeleteBuffers = glDeleteBuffersARB;
		glGenBuffers = glGenBuffersARB;
		glGetBufferParameteriv = glGetBufferParameterivARB;
		glGetBufferPointerv = glGetBufferPointervARB;
		glGetBufferSubData = glGetBufferSubDataARB;
		glIsBuffer = glIsBufferARB;
		glMapBuffer = glMapBufferARB;
		glUnmapBuffer = glUnmapBufferARB;
	}
	*/

	/*
	if ( glAttachShader==NULL )
	{
		glAttachShader = glAttachObjectARB;
		glCompileShader = glCompileShaderARB;
		glCreateProgram = glCreateProgramObjectARB;
		glCreateShader = glCreateShaderObjectARB;
		glDeleteProgram = glDeleteObjectARB;
		glDetachShader = glDetachObjectARB;
		glGetActiveUniform = glGetActiveUniformARB;
		glGetAttachedShaders = glGetAttachedObjectsARB;

		//glGetHandleARB
		glGetShaderInfoLog = glGetInfoLogARB;
		//glGetObjectParameterfvARB
		//glGetObjectParameterivARB 
		
		glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) glGetShaderSourceARB;
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glGetUniformLocation;
		glGetUniformfv = glGetUniformfvARB;
		glGetUniformiv = glGetUniformivARB;
		glLinkProgram = glLinkProgramARB;
		glShaderSource = glShaderSourceARB;
		glUniform1f = glUniform1fARB;
		glUniform1fv = glUniform1fvARB;
		glUniform1i = glUniform1iARB;
		glUniform1iv= glUniform1ivARB;
		glUniform2f= glUniform2fARB;
		glUniform2fv = glUniform2fvARB;
		glUniform2i= glUniform2iARB;
		glUniform2iv = glUniform2ivARB;
		glUniform3f = glUniform3fARB;
		glUniform3fv = glUniform3fvARB;
		glUniform3i = glUniform3iARB;
		glUniform3iv = glUniform3ivARB;
		glUniform4f = glUniform4fARB;
		glUniform4fv = glUniform4fvARB;
		glUniform4i = glUniform4iARB;
		glUniform4iv = glUniform4ivARB;
		glUniformMatrix2fv = glUniformMatrix2fvARB;
		glUniformMatrix3fv=  glUniformMatrix3fvARB; 
		glUniformMatrix4fv = glUniformMatrix4fvARB;
		glUseProgram = glUseProgramObjectARB;
		glValidateProgram = glValidateProgramARB; 
	}
	*/
}

static bool SetPixelformat(void)
{
	HWND hWnd = GutGetWindowHandleWin32();
	g_hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 24; // 24 bits zbuffer
	pfd.cStencilBits = 8; // 8 bits stencil buffer
	pfd.iLayerType = PFD_MAIN_PLANE; // main layer

	int pixelformat = ChoosePixelFormat(g_hDC, &pfd);

    if ( pixelformat == 0 ) 
    { 
        return false; 
    } 

    if ( SetPixelFormat(g_hDC, pixelformat, &pfd) == FALSE) 
    { 
		ReleaseDC(hWnd, g_hDC);
        return false; 
    }

	g_hGLRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hGLRC);

    return true; 
}

void ErrorMessage(LPTSTR lpszFunction) 
{ 
    TCHAR szBuf[80]; 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    printf("%s failed with error %d: %s", 
        lpszFunction, dw, lpMsgBuf); 
 
    LocalFree(lpMsgBuf);
}

// ���� OpenGL ��, ���ܵ���������չ���������� multi-sampling ����.
// ����Ҫ���´��µĴ�����ʹ�� multi-sampling ģʽ, ����ʹ��ԭ���Ĵ���.
static bool SetPixelformatEX(GutDeviceSpec *pSpec)
{
	HWND hWnd = GutGetWindowHandleWin32();
	g_hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cDepthBits = 24; // 24 bits zbuffer
	pfd.cStencilBits = 8; // 8 bits stencil buffer
	pfd.iLayerType = PFD_MAIN_PLANE; // main layer

	int pixelformat = ChoosePixelFormat(g_hDC, &pfd);

    if ( pixelformat == 0 ) 
    { 
        return false; 
    } 

    if ( SetPixelFormat(g_hDC, pixelformat, &pfd) == FALSE) 
    { 
		ReleaseDC(hWnd, g_hDC);
        return false; 
    }

	g_hGLRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hGLRC);

	if (!wglChoosePixelFormatARB)
	{
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	}

	if ( !wglChoosePixelFormatARB )
	{
		// We Didn't Find Support For Multisampling, Set Our Flag And Exit Out.
		printf("OpenGL Display driver does not support wglChoosePixelFormatARB function\n");
		return false;
	}

	int iAttributes[] = { 
		WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,24,
		WGL_STENCIL_BITS_ARB,8,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
		WGL_SAMPLES_ARB, pSpec->m_iMultiSamples, // Multi-Sampling�Ĳ�������
		0,0};

	float fAttributes[] = {0,0};

	UINT numFormats = 0;

	// `����Ƿ�֧����Ҫ��Ļ���ģʽ, ��Ҫ���ڿ��Ƿ�֧�� multisampling.`
	BOOL valid = wglChoosePixelFormatARB(g_hDC, iAttributes, fAttributes,1, &pixelformat, &numFormats);

	if (!valid && numFormats==0)
	{
		return false;
	}

	DescribePixelFormat(g_hDC, pixelformat, sizeof(pfd), &pfd); 

	if ( g_hGLRC && g_hDC )
	{
		wglMakeCurrent(g_hDC, NULL);
		wglDeleteContext(g_hGLRC);
		g_hGLRC = NULL;

		ReleaseDC(hWnd, g_hDC);
		g_hDC = NULL;

		// �ر�ԭʼ�Ĵ���
		DestroyWindow(hWnd);
		// ��׽WM_QUIT ��Ϣ
		MSG msg;
		while(GetMessage(&msg, NULL, 0, 0)!=0) {} 
	}

	// ͬһ������ֻ�ܵ��� SetPixelFormat һ��, ����Ҫ���´��µĴ���.
	GutCreateWindow(-1, -1, -1, -1, "OpenGL");

	hWnd = GutGetWindowHandleWin32();
	g_hDC = GetDC(hWnd);

	if ( SetPixelFormat(g_hDC, pixelformat, &pfd)==FALSE) 
	{ 
		ErrorMessage("SetPixelFormatEX");
		ReleaseDC(hWnd, g_hDC);
		return false;
	}

	g_hGLRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hGLRC);

	return true;
}

bool GutInitGraphicsDeviceOpenGL(GutDeviceSpec *pSpec)
{
	int multisamples = 0;

	if ( pSpec )
		multisamples = pSpec->m_iMultiSamples;

	// `�򿪴���ʱ���ѻ��������������ڵ�ָ��, ֱ������ʹ��.`
	HWND hWnd = GutGetWindowHandleWin32();
	if ( hWnd==NULL )
		return false;

	if ( multisamples )
	{
		if ( !SetPixelformatEX(pSpec) )
		{
			return false;
		}
	}
	else
	{
		if ( !SetPixelformat() )
		{
			return false;
		}
	}

	glewInit();
	FixupGLExt();

	return true;
}

bool GutReleaseGraphicsDeviceOpenGL(void)
{
	HWND hWnd = GutGetWindowHandleWin32();

	wglMakeCurrent(g_hDC, NULL);
	wglDeleteContext(g_hGLRC);
	g_hGLRC = NULL;

	ReleaseDC(hWnd, g_hDC);
	g_hDC = NULL;

	return true;
}

void GutSwapBuffersOpenGL(void)
{
	// `��ʾ������`
	SwapBuffers(g_hDC);
}

// OpenGL ARB Vertex Program�淶���������
GLuint GutLoadVertexProgramOpenGL_ASM(const char *filename)
{
	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	unsigned int size = 0;
	// ���뵵��
	unsigned char *buffer = (unsigned char *) GutLoadBinaryStream(filename_fullpath, &size);
	if ( buffer==NULL )
	{
		return 0;
	}

	GLuint shader_id = 0;
	// ����һ��shader����
	glGenProgramsARB(1, &shader_id);
	// ʹ���µ�shader����, ���������ó�vertex program����
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, shader_id);
  	// ���������ָ������shader������
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, size, buffer);
	// ���Shader�г������κδ���
	if ( GL_INVALID_OPERATION == glGetError() )
	{
		// Find the error position
		GLint errPos;
		glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
		// errors and warnings string.
		GLubyte *errString = (GLubyte *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		fprintf( stderr, "error at position: %d\n%s\n", errPos, errString );
		// ��ȡʧ��, �ͷ�shader����
		glDeleteProgramsARB(1, &shader_id);
		shader_id = 0;
	}
	// �����뵵�����ڴ��ͷ�
	GutReleaseBinaryStream(buffer);
	// shader_id�����µ�shader����
	return shader_id;
}

void GutReleaseVertexProgramOpenGL(GLuint shader_id)
{
	glDeleteProgramsARB( 1, &shader_id );
}

// OpenGL ARB Fragment Program�淶���������
GLuint GutLoadFragmentProgramOpenGL_ASM(const char *filename)
{
	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	unsigned int size = 0;
	// ���뵵��
	unsigned char *buffer = (unsigned char *) GutLoadBinaryStream(filename_fullpath, &size);
	if ( buffer==NULL )
	{
		return 0;
	}

	GLuint shader_id = 0;
	// ����һ��shader����
	glGenProgramsARB(1, &shader_id);
	// ʹ���µ�shader����, ���������ó�fragment program����
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, shader_id );
	// ���������ָ������shader������
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, size, buffer);
	// ���Shader�г������κδ���
	if ( GL_INVALID_OPERATION == glGetError() )
	{
		// Find the error position
		GLint errPos;
		glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
		// errors and warnings string.
		GLubyte *errString = (GLubyte *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		fprintf( stderr, "error at position: %d\n%s\n", errPos, errString );
		// ��ȡʧ��, �ͷ�shader����
		glDeleteProgramsARB(1, &shader_id);
		shader_id = 0;
	}
	// �����뵵�����ڴ��ͷ�
	GutReleaseBinaryStream(buffer);
	// shader_id�����µ�shader����
	return shader_id;
}

void GutReleaseFragmentProgramOpenGL(GLuint shader_id)
{
	glDeleteProgramsARB( 1, &shader_id );
}

GLuint _LoadGLSLShader(const char *filename, GLenum type)
{
	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	GLuint shader = glCreateShader(type);

	unsigned int len = 0;
	const GLchar *code = (const GLchar *)GutLoadFileStream(filename, &len);
	GLint size = len;
	glShaderSource(shader, 1, &code, &size);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if ( GL_TRUE!=result )
	{
		GLchar pMessage[2048];
		GLsizei msgLen = 1023;

		glGetShaderInfoLog(shader, size, &msgLen, pMessage);

		printf("%s compile error\n", filename);
		printf("%s\n", pMessage);

		glDeleteShader(shader);
		shader = 0;
	}

	GutReleaseFileStream(code);

	return shader;
}

GLuint GutLoadVertexShaderOpenGL_GLSL(const char *filename)
{
	return _LoadGLSLShader(filename, GL_VERTEX_SHADER);
}

GLuint GutLoadFragmentShaderOpenGL_GLSL(const char *filename)
{
	return _LoadGLSLShader(filename, GL_FRAGMENT_SHADER);
}

GLuint GutCreateProgram(GLuint vs, GLuint fs)
{
	GLuint p = glCreateProgram();
	
	glAttachShader(p, vs);
	glAttachShader(p, fs);
	glLinkProgram(p);

	return p;
}

bool GutCreateRenderTargetOpenGL(int w, int h, GLuint color_fmt, GLuint *pFramebuffer, GLuint *pTexture)
{
	GLuint framebuffer, texture;
	
	*pFramebuffer = 0;
	*pTexture = 0;

	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	// ����һ����ͼ�ռ��framebuffer object��ͼʹ�� 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// ����filter
	if ( color_fmt==GL_RGBA32F_ARB || color_fmt==GL_RGBA16F_ARB )
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// ������ͼ��С����ʽ
	glTexImage2D(GL_TEXTURE_2D, 0, color_fmt,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	// framebuffer��RGBA��ͼ
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);

	// ���framebuffer object�Ƿ����ɹ�
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	*pFramebuffer = framebuffer;
	*pTexture = texture;

	return true;
}

bool GutCreateRenderTargetOpenGL(int w, int h, GLuint *pFrameBuffer,
								 GLuint color_fmt, GLuint *pFrameTexture, int num_mrts,
								 GLuint depth_fmt, GLuint *pDepthTexture)
{
	GLuint framebuffer =  0;
	GLuint frametexture = 0;
	GLuint depthtexture = 0;

	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	if ( pFrameTexture )
	{
		for ( int i=0; i<num_mrts; i++ )
		{
			// ������ͼ�ռ� 
			glGenTextures(1, &frametexture);
			glBindTexture(GL_TEXTURE_2D, frametexture);
			// ʹ��һ��Ԥ���filter
			//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// ����filter
			if ( color_fmt==GL_RGBA32F_ARB || color_fmt==GL_RGBA16F_ARB )
			{
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			else
			{
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			// ������ͼ��С����ʽ
			glTexImage2D(GL_TEXTURE_2D, 0, color_fmt,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// framebuffer��RGBA��ͼ
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, frametexture, 0);
			
			pFrameTexture[i] = frametexture;
		}
	}
	else
	{
		glDrawBuffer(FALSE);
		glReadBuffer(FALSE);
	}

	if ( pDepthTexture )
	{
		// ������ͼ�ռ� 
		glGenTextures(1, &depthtexture);
		glBindTexture(GL_TEXTURE_2D, depthtexture);
		// ʹ��һ��Ԥ���filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// ������ͼ��С����ʽ
		glTexImage2D(GL_TEXTURE_2D, 0, depth_fmt,  w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		// framebuffer��ZBuffer����
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthtexture, 0);

		*pDepthTexture = depthtexture;
	}

	/*
	 ���framebuffer object�Ƿ����ɹ�
	*/

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	*pFrameBuffer = framebuffer;

	return true;
}

bool GutCreateRenderTargetOpenGL(int w, int h, GLuint *pFrameBuffer, 
								 GLuint color_fmt, GLuint *pFrameTexture,
								 GLuint depth_fmt, GLuint *pDepthTexture)
{
	return GutCreateRenderTargetOpenGL(w, h, pFrameBuffer, color_fmt, pFrameTexture, 1, depth_fmt, pDepthTexture);
}

/*
bool GutCreateRenderTargetOpenGL(int w, int h, GLuint *pFrameBuffer, 
								 GLuint color_fmt, GLuint *pFrameTexture,
								 GLuint depth_fmt, GLuint *pDepthTexture)
{
	GLuint framebuffer =  0;
	GLuint frametexture = 0;
	GLuint depthtexture = 0;

	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

	if ( pFrameTexture )
	{
		// ������ͼ�ռ� 
		glGenTextures(1, &frametexture);
		glBindTexture(GL_TEXTURE_2D, frametexture);
		// ʹ��һ��Ԥ���filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// ������ͼ��С����ʽ
		glTexImage2D(GL_TEXTURE_2D, 0, color_fmt,  w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// framebuffer��RGBA��ͼ
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, frametexture, 0);
		
		*pFrameTexture = frametexture;
	}
	else
	{
		glDrawBuffer(FALSE);
		glReadBuffer(FALSE);
	}

	if ( pDepthTexture )
	{
		// ������ͼ�ռ� 
		glGenTextures(1, &depthtexture);
		glBindTexture(GL_TEXTURE_2D, depthtexture);
		// ʹ��һ��Ԥ���filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// ������ͼ��С����ʽ
		glTexImage2D(GL_TEXTURE_2D, 0, depth_fmt,  w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		// framebuffer��ZBuffer����
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthtexture, 0);

		*pDepthTexture = depthtexture;
	}

	// ���framebuffer object�Ƿ����ɹ�
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	*pFrameBuffer = framebuffer;

	return true;
}
*/

void GutSetupLightOpenGL(int index, sGutLight &light)
{
	GLuint LightID = GL_LIGHT0 + index;

	if ( light.m_bEnabled )
	{
		glEnable(LightID);
	}
	else
	{
		glDisable(LightID);
	}

	if ( light.m_iLightType==LIGHT_POINT )
	{
		glLightfv(LightID, GL_POSITION, &light.m_vPosition[0]);
	}
	else
	{
		Vector4 pos = -light.m_vDirection;
		pos[3] = 0.0f;
		glLightfv(LightID, GL_POSITION, &pos[0]);
	}

	glLightfv(LightID, GL_AMBIENT,  &light.m_vAmbient[0]);
	glLightfv(LightID, GL_DIFFUSE,  &light.m_vDiffuse[0]);
	glLightfv(LightID, GL_SPECULAR, &light.m_vSpecular[0]);

	glLightf(LightID, GL_CONSTANT_ATTENUATION,	light.m_vAttenuation[0]);
	glLightf(LightID, GL_LINEAR_ATTENUATION,	light.m_vAttenuation[1]);
	glLightf(LightID, GL_QUADRATIC_ATTENUATION, light.m_vAttenuation[2]);
}
