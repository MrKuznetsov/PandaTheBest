
#include "hwRender.h"

//-----------------------------------------------------------------------------
// Name: HwRender
// Desc: --------
//-----------------------------------------------------------------------------

HRESULT HwRender::initD3D(HWND wnd)
{
	hw = wnd;

	if( NULL == ( pD3d = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	ZeroMemory( &pD3dpp, sizeof( pD3dpp ) );

	pD3dpp.Windowed = TRUE;
	pD3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pD3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	pD3dpp.EnableAutoDepthStencil = TRUE;
	pD3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	pD3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pD3dpp.MultiSampleQuality = 0;
	pD3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	pD3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hw,
									  D3DCREATE_HARDWARE_VERTEXPROCESSING,
									  &pD3dpp, &pD3dDevice ) ) )
	{
		return E_FAIL;
	}
	
	pD3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );


		// Turn off culling, so we see the front and back of the triangle
	pD3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	// Turn off D3D lighting, since we are providing our own vertex colors
	pD3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
}

D3DVIEWPORT9 HwRender::getViewPort()
{
	D3DVIEWPORT9 view;
	pD3dDevice->GetViewport(&view);
	return view;
}

LPDIRECT3DDEVICE9 HwRender::getDevice()
{
	return pD3dDevice;
}

void HwRender::setPerspectiveFovLH(D3DXMATRIX *pOut, float fovy, float Aspect, float zn, float zf)
{
	D3DXMatrixPerspectiveFovLH(pOut, fovy, Aspect, zn, zf);
	pD3dDevice->SetTransform(D3DTS_PROJECTION, pOut);
}

void HwRender::setLookAtLH(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt, const D3DXVECTOR3 *pUp)
{
	D3DXMatrixLookAtLH(pOut, pEye, pAt, pUp);
	pD3dDevice->SetTransform(D3DTS_VIEW, pOut);
}

void HwRender::setTransform(D3DTRANSFORMSTATETYPE State, const D3DXMATRIX *matrix)
{
	pD3dDevice->SetTransform(State, matrix);
}

void HwRender::setTextureTransform(int stage, const D3DXMATRIX *matrix)
{
	setTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	setTransform(D3DTRANSFORMSTATETYPE(DWORD(D3DTS_TEXTURE0) + stage), matrix);
}

void HwRender::setNULLTexture(int stage)
{
	pD3dDevice->SetTexture(stage, NULL);
}

void HwRender::setRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
	DWORD vc;
	pD3dDevice->GetRenderState(state, &vc);
	if (vc != value)
		pD3dDevice->SetRenderState(state, value);
}

void HwRender::setSamplerState(int stage, D3DSAMPLERSTATETYPE state, DWORD value)
{
	DWORD vc;
	pD3dDevice->GetSamplerState(stage, state, &vc);
	if (vc != value)
		pD3dDevice->SetSamplerState(stage, state, value);
}

void HwRender::setTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE state, DWORD value)
{
	DWORD vc;
	pD3dDevice->GetTextureStageState(stage, state, &vc);
	if (vc != value)
		pD3dDevice->SetTextureStageState(stage, state, value);
}

void HwRender::setMaterial(D3DMATERIAL9 *mtrl)
{
	pD3dDevice->SetMaterial(mtrl);
}

void HwRender::drawPrimitives(D3DPRIMITIVETYPE dType, DWORD stratV, DWORD primCount)
{
	pD3dDevice->DrawPrimitive(dType, stratV, primCount);
}

void HwRender::drawIndexedPrimitives(D3DPRIMITIVETYPE dType, int baseVIndex, DWORD minVIndex, 
		DWORD numVertices, DWORD stratIndex, DWORD primCount)
{
	pD3dDevice->DrawIndexedPrimitive(dType, baseVIndex, minVIndex, numVertices, stratIndex, primCount);
}

void HwRender::begin()
{
	pD3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

	// Begin the scene
	pD3dDevice->BeginScene();
}

void HwRender::end()
{
	pD3dDevice->EndScene();

	pD3dDevice->Present( NULL, NULL, NULL, NULL );
}