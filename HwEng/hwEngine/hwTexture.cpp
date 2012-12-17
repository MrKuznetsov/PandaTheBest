#include "hwTexture.h"
#include "hwEngine.h"
#include "hwRender.h"

#include <iostream>

//-----------------------------------------------------------------------------
// Name: HwTexture
// Desc: --------
//-----------------------------------------------------------------------------

HwResource *HwTextureManager::newRes()
{
	return new HwTexture(this);
}

HwTexture::HwTexture(HwResourceManager *m) : HwResource(m)
{
	format = D3DFMT_A8R8G8B8;
	levels = 0;//D3DX_DEFAULT;
	texType = HWTEX_2D;

	int width = 512;
	int height = 512;

	p2DTexture = NULL;
	pCubeTexture = NULL;
}

int HwTexture::getWidth()
{
	return width;
}

int HwTexture::getHeight()
{
	return height;
}

int HwTexture::getDepth()
{
	return depth;
}

void HwTexture::load2D()
{
	D3DPOOL pool = D3DPOOL_MANAGED;
	usage = 0;
	unsigned int size = getSize();
	char *buffer = new char[size];
	f->seekg(0, ios::beg);
	f->read(buffer, size);
	LPDIRECT3DDEVICE9 pD3DDevice = getManager()->getEngine()->getRender()->getDevice();
	D3DXCreateTextureFromFileInMemoryEx(pD3DDevice, buffer, size, 0, 0, levels, usage, format, pool, D3DX_DEFAULT,
		D3DX_DEFAULT, transparentColor, NULL, NULL, &p2DTexture);

	D3DSURFACE_DESC desc;
	p2DTexture->GetLevelDesc(0, &desc);

	width = desc.Width;
	height = desc.Height;
	delete[] buffer;

}

void  HwTexture::loadCube()
{
	D3DPOOL pool = D3DPOOL_MANAGED;
	unsigned int size = getSize();
	char *buffer = new char[size];
	f->seekg(0, ios::beg);
	f->read(buffer, size);
	LPDIRECT3DDEVICE9 pD3DDevice = getManager()->getEngine()->getRender()->getDevice();
	D3DXCreateCubeTexture(pD3DDevice, width, levels, usage, format, pool, &pCubeTexture);

	delete[] buffer;
}

void HwTexture::copyToTexture(HwTexture *tex)
{
	//----------------//
}

void HwTexture::load()
{
	if (texType == HWTEX_2D)
		load2D();
	else if (texType == HWTEX_CUBE)
		loadCube();
}

void HwTexture::createNewTexture(int w, int h)
{
	texType = HWTEX_2D;
	width = w;
	height = h;

	D3DPOOL pool = D3DPOOL_MANAGED;
	LPDIRECT3DDEVICE9 pD3DDevice = getManager()->getEngine()->getRender()->getDevice();
	D3DXCreateTexture(pD3DDevice, width, height, levels, usage, format, pool, &p2DTexture);
}

void HwTexture::setToDevice(DWORD stage)
{
	if (texType == HWTEX_2D)
		getManager()->getEngine()->getRender()->getDevice()->SetTexture(stage, p2DTexture);
	else if (texType == HWTEX_CUBE)
		getManager()->getEngine()->getRender()->getDevice()->SetTexture(stage, pCubeTexture);
}

D3DCOLORVALUE HwTexture::getPixelColor(int x, int y)
{
	D3DCOLORVALUE color; 
	if  (texType != HWTEX_2D)
		return color;

	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT rect;

	p2DTexture->GetLevelDesc(0, &desc);
	p2DTexture->LockRect(0, &rect, NULL, 0);

	BYTE *pb;
	BYTE r,g,b,a;
	WORD tmp, t1, t2;

	switch (desc.Format)
	{
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 4);
		b = *pb; pb++;
		g = *pb; pb++;
		r = *pb; pb++;
		a = *pb;
		break;
	case D3DFMT_R8G8B8:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 3);
		b = *pb; pb++;
		g = *pb; pb++;
		r = *pb;
		break;
	case D3DFMT_A4R4G4B4:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 2);
		t2 = *pb; pb++;
		t1 = *pb;
		tmp = t1 << 8 + t2;

		b = (tmp & 0x0F) << 4;
		g = ((tmp >> 4) & 0x0F) << 4;
		r = ((tmp >> 8) & 0x0F) << 4;
		a = ((tmp >> 12) & 0x0F) << 4;
		break;
	case D3DFMT_R5G6B5:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 2);
		t2 = *pb; pb++;
		t1 = *pb;
		tmp = t1 << 8 + t2;

		b = (tmp & 0x1F) << 3;
		g = ((tmp >> 5) & 0x3F) << 2;
		r = ((tmp >> 11) & 0x1F) << 3;
		break;
	}
	p2DTexture->UnlockRect(0);

	color.a = a / 255.0f;
	color.r = r / 255.0f;
	color.g = g / 255.0f;
	color.b = b / 255.0f;
	return color;
}
void HwTexture::setPixelColor(int x, int y, D3DCOLORVALUE color)
{
	if  (texType != HWTEX_2D)
		return;

	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT rect;

	p2DTexture->GetLevelDesc(0, &desc);
	p2DTexture->LockRect(0, &rect, NULL, 0);

	BYTE *pb;
	BYTE r,g,b,a;
	WORD tmp;
	BYTE low;
	BYTE high;

	r = color.r * 255;
	g = color.g * 255;
	b = color.b * 255;
	a = color.a * 255;

	switch (desc.Format)
	{
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8R8G8B8:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 4);
		*pb = b; pb++;
		*pb = g; pb++;
		*pb = r; pb++;
		*pb = a;
		break;
	case D3DFMT_R8G8B8:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 3);
		*pb = b; pb++;
		*pb = g; pb++;
		*pb = r;
		break;
	case D3DFMT_A4R4G4B4:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 2);
		tmp = (a >> 4) << 12 + (r >> 4) << 8 + (g >> 4) << 4 + b >> 4;
		low = tmp & 0x00FF;
		high = (tmp >> 8) & 0x00FF;
		*pb = low; pb++;
		*pb = high;
		break;
	case D3DFMT_R5G6B5:
		pb = (BYTE *)(long(rect.pBits) + y * rect.Pitch + x * 2);
		tmp = (r >> 3) << 11 + (g >> 2) << 5 + (b >> 3);
		low = tmp & 0x00FF;
		high = (tmp >> 8) & 0x00FF;
		*pb = low; pb++;
		*pb = high;
		break;
	}
	p2DTexture->UnlockRect(0);
}

void HwTexture::fill(D3DCOLORVALUE color)
{
	for (int i = 0; i < width; i ++)
		for (int j = 0; j < height; j++)
			setPixelColor(i, j, color);
}