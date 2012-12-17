#pragma once

#include "hwResources.h"

//-----------------------------------------------------------------------------
// Name: HwTextureManager
// Desc: ----------------
//-----------------------------------------------------------------------------

class HwTextureManager : public HwResourceManager
{
public:
	HwTextureManager(HwEngine *eng) : HwResourceManager(eng){}
	HwResource *newRes();
};

//-----------------------------------------------------------------------------
// Name: HwTexture
// Desc: --------
//-----------------------------------------------------------------------------
class HwTexture : public HwResource
{
public:
	LPDIRECT3DTEXTURE9 p2DTexture;
	LPDIRECT3DCUBETEXTURE9 pCubeTexture;

	DWORD transparentColor;
	D3DFORMAT format;
	int levels;
	HWTEXTURETYPE texType;
	int width;
	int height;
	int depth;
	DWORD usage;
protected:
	void load2D();
	void loadCube();
public:
	HwTexture(HwResourceManager *m);

	int getWidth();
	int getHeight();
	int getDepth();

	//property

	void copyToTexture(HwTexture *tex);
	void load();
	void createNewTexture(int w, int h);
	void setToDevice(DWORD stage);

	D3DCOLORVALUE getPixelColor(int x, int y); //for 2D only
	void setPixelColor(int x, int y, D3DCOLORVALUE color);
	void fill(D3DCOLORVALUE color);
};


