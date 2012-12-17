#include <Windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <iostream>
#include <vector>

#include "pandaTheGame.h"

#include "..\HwEng\hwEngine\hwEngine.h"
#include "..\HwEng\hwEngine\hwMisc.h"
#include "..\HwEng\hwEngine\hwSkeleton.h"
#include "..\HwEng\hwEngine\hwBuffers.h"
#include "..\HwEng\hwEngine\hwMesh.h"
#include "..\HwEng\hwEngine\hwTerrain.h"
#include "..\HwEng\hwEngine\hwEntities.h"
#include "..\HwEng\hwEngine\hwShaders.h"
HINSTANCE hInstance; HINSTANCE hPrevInst; 
LPWSTR lpszArgs; int nWinMode;


PTGScene *ptg;

//----------------------------------------------------------------------------\\
//------------------------- ------------ -------- ----------------------------\\
//-----------------------------------------------------------------------------\\

void OnSkyRender(HwSceneObject *sender)
{
	ptg->onSkyRender(sender);
}



void OnObjRender(HwSceneObject *sender)
{
	ptg->onObjRender(sender);
}

void OnCreate()
{

	ptg->objRender = OnObjRender;
	ptg->skyRender = OnSkyRender;
	ptg->init();
}

void OnUpdate()
{
	ShowCursor(FALSE);
	HwCamera *camera = ptg->pEng->getScene()->getCamera();
	ptg->update();
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

	UINT key = ( UINT )wParam;
	switch( msg )
	{
		case WM_MOUSEMOVE:
			x = ( short )LOWORD( lParam );
			y = ( short )HIWORD( lParam );
			return 0;
		case WM_DESTROY:
		  //  Cleanup();
			PostQuitMessage( 0 );
			return 0;
		case WM_LBUTTONDOWN:
			ptg->mb.lb = 2;
			return 0;
		case WM_LBUTTONUP:
			ptg->mb.lb = 0;
			return 0;
		case WM_KEYDOWN:
			if (key < 255)
				ptg->keys[key] = 2;
			return 0;
		case WM_KEYUP:
			if (key < 255)
				ptg->keys[key] = 0;
			return 0;
	//	case WM_PAINT:
		//	OnUpdate();
		//	ValidateRect( hWnd, NULL );
	//		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	UNREFERENCED_PARAMETER( hInst );

	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"D3D Engine Test", NULL
	};
	RegisterClassEx( &wc );

	HWND hWnd = CreateWindow( L"D3D Engine Test", L"D3D Engine Test",
							  WS_OVERLAPPEDWINDOW, 25, 25, 1000, 700,
							  NULL, NULL, wc.hInstance, NULL );

//---------------------------
//	eng = new HwEngine(hWnd);
	ptg = new PTGScene(hWnd);
//---------------------------

	OnCreate();
	//if( SUCCEEDED( eng->init() ) )
	//{
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		MSG msg;
		ZeroMemory( &msg, sizeof( msg ) );
		while( msg.message != WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			OnUpdate();
		}
	//}

	UnregisterClass( L"D3D Engine Test", wc.hInstance );
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	wWinMain(hInstance,  hPrevInst, 
		 lpszArgs,  nWinMode);
	return 0;
}



