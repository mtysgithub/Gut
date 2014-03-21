#include <stdio.h>
#include "Gut.h"

void RenderFrameDX9(void);
void RenderFrameOpenGL(void);

void main(int argc, char *argv[])
{
	// Ĭ��ʹ��DirectX 9����ͼ
	char *device = "dx9";

	if ( argc > 1 )
	{
		// ��������в���ָ����OpenGL, �͸���OpenGL.
		if ( stricmp(argv[1], "opengl")==0 )
		{
			device = "opengl";
		}
	}

	// ��(100,100)��λ�ô�һ����СΪ(512x512)�Ĵ���
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��ʼ��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	// ��ѭ��
	while( GutProcessMessage() )
	{
		if ( !strcmp(device, "dx9") )
		{
			RenderFrameDX9();
		}
		else
		{
			RenderFrameOpenGL();
		}
	}

	// �ر�OpenGL/DirectX��ͼ�豸
	GutReleaseGraphicsDevice();
}

// һ��3D�����(x,y,z)��λ��
float vertex[] = {0.0f, 0.0f, 0.0f};

// ʹ��DirectX 9�ĺ���
void RenderFrameDX9(void)
{
	// ���D3D�豸.
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �رչ��յļ���
    device->SetRenderState( D3DRS_LIGHTING, FALSE );
	// �ѻ�����Ϊ��ɫ
    device->Clear( 
		0, NULL,��// ����������� 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // �����ɫ��Z buffer 
		D3DCOLOR_ARGB(0, 0, 0, 0),  // ����Ҫ����ɫ��ɺ�ɫ
		1.0f, // ����Ҫ��Zֵ��Ϊ1, Ҳ�����뾵ͷ��Զ
		0 // ����Ҫ��Stencil buffer��Ϊ0, �����Ƿ�����û������.
	);

	// ��ʼ�»�ͼָ��
	device->BeginScene(); 
	// �������ݸ�ʽ
	device->SetFVF(D3DFVF_XYZ); 
	// ����һ����
	device->DrawPrimitiveUP(D3DPT_POINTLIST, 1, vertex, 12); 
	// �������еĻ�ͼָ�������
	device->EndScene();		
	
	// �ѱ���backbuffer�Ļ�����ʾ����
    device->Present( NULL, NULL, NULL, NULL );
}

// ʹ��OpenGL�ĺ���
void RenderFrameOpenGL(void)
{
	// �������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ��һ����
	glBegin(GL_POINTS);
	glVertex3fv(vertex);
	glEnd();

	// �ѱ���backbuffer�Ļ�����ʾ����
	GutSwapBuffersOpenGL();
}
