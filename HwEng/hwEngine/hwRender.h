#pragma once

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Name: HwRender
// Desc: --------
//-----------------------------------------------------------------------------

class HwRender
{
private:
	LPDIRECT3D9 pD3d; // Used to create the D3DDevice  
	LPDIRECT3DDEVICE9 pD3dDevice; // Our rendering device
	D3DPRESENT_PARAMETERS pD3dpp;
	HWND hw;
public:
	HRESULT initD3D(HWND wnd);

	D3DVIEWPORT9 getViewPort();
	LPDIRECT3DDEVICE9 getDevice();

	void setPerspectiveFovLH(D3DXMATRIX *pOut, float fovy, float Aspect, float zn, float zf);
	void setLookAtLH(D3DXMATRIX *pOut, const D3DXVECTOR3 *pEye, const D3DXVECTOR3 *pAt, const D3DXVECTOR3 *pUp);
	void setTransform(D3DTRANSFORMSTATETYPE State, const D3DXMATRIX *matrix);
	void setTextureTransform(int stage, const D3DXMATRIX *matrix);
	void setNULLTexture(int stage);

	void setRenderState(D3DRENDERSTATETYPE state, DWORD value);
	void setSamplerState(int stage, D3DSAMPLERSTATETYPE state, DWORD value);
	void setTextureStageState(int stage, D3DTEXTURESTAGESTATETYPE state, DWORD value);

	void setMaterial(D3DMATERIAL9 *mtrl);

	void drawPrimitives(D3DPRIMITIVETYPE dType, DWORD stratV, DWORD primCount);
	void drawIndexedPrimitives(D3DPRIMITIVETYPE dType, int baseVIndex, DWORD minVIndex, 
		DWORD numVertices, DWORD stratIndex, DWORD primCount);

	void begin();
	void end();
};