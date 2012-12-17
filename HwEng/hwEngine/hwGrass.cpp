#include "hwGrass.h"
#include "hwEngine.h"
#include "hwTerrain.h"

#include <iostream>

//-----------------------------------------------------------------------------
// Name: HwGrassQuad
// Desc: -------------
//-----------------------------------------------------------------------------

HwGrassQuad::HwGrassQuad(HwGrassField *filed)
{
	pField = filed;
	x = 0;
	y = 0;
	width = 32;
	height = 32;
	grassCount = 0;

	pVB = new HwVertexBuffer(pField->getEngine()->getRender());
}

HwGrassQuad::~HwGrassQuad()
{
	delete pVB;
}

void HwGrassQuad::init()
{
	pVB->bufferSize() = 12 * width * height;
	pVB->itemSize() = sizeof(HwGrassRenderVertex);

	pVB->getDecl()->addElement(0, 0, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0);
	pVB->getDecl()->addElement(0, 4 * 3, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0);
	pVB->getDecl()->addElement(0, 4 * 6, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 0);

	pVB->init();

	reInit();
}

HwGrassRenderVertex setGrassVertex(float px, float py, float pz, D3DXVECTOR3 n, float tu, float tv)
{
	HwGrassRenderVertex v;
	v.pos = D3DXVECTOR3(px, py, pz);
	v.normal = n;
	v.tex = D3DXVECTOR2(tu, tv);
	return v;
}

void HwGrassQuad::reInit()
{
	float size = 2.0f;

	grassCount = 0;
	int index = 0;
	int w = pField->getWidth();
	int h = pField->getHeight();
	HwGrassRenderVertex *v = (HwGrassRenderVertex *)pVB->lock();

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			if (pField->Map(x * width + i, y * height + j))
			{
				D3DXVECTOR3 n = D3DXVECTOR3(0, 1, 0);

				float py = 0;
				float px = -w / 2.0f + x * width + i + 0.5;
				float pz =  h / 2.0f - y * height - j - 0.5;

				if (pField->isAttcahed())
					py = pField->getTerrain()->getHeight(px + 0.5f, pz - 0.5f);

				float ang = (rand() % 6283) / 1000.0f;//rotate for random angle

				float ax = - 0.5 * size;
				float az = 0.5 * size;
				ax = ax * cosf(ang) + az * sinf(ang);
				az = az * cosf(ang) - ax * sinf(ang);

				v[index++] = setGrassVertex(px + ax, py, pz + az, n, 0, 1);
				v[index++] = setGrassVertex(px + ax, py + 1 * size, pz + az, n, 0, 0);
				v[index++] = setGrassVertex(px - ax, py + 1 * size, pz - az, n, 1, 0);
				v[index++] = setGrassVertex(px - ax, py, pz - az, n, 1, 1);
				
				v[index++] = setGrassVertex(px + az, py, pz - ax, n, 0, 1);
				v[index++] = setGrassVertex(px + az, py + 1 * size, pz - ax, n, 0, 0);
				v[index++] = setGrassVertex(px - az, py + 1 * size, pz + ax, n, 1, 0);
				v[index++] = setGrassVertex(px - az, py, pz + ax, n, 1, 1);

				ax = ax * cosf(ang);
				az = - ax * sinf(ang);

				v[index++] = setGrassVertex(px + ax, py, pz + az, n, 0, 1);
				v[index++] = setGrassVertex(px + ax, py + 1 * size, pz + az, n, 0, 0);
				v[index++] = setGrassVertex(px - ax, py + 1 * size, pz - az, n, 1, 0);
				v[index++] = setGrassVertex(px - ax, py, pz - az, n, 1, 1);
				

				grassCount++;
			}
	pVB->unlock();
}

void HwGrassQuad::render()
{

	if (grassCount < 1)
		return;
	pVB->setStreamSource(0);
	pField->pIB->setIndices();
	pField->getEngine()->getRender()->drawIndexedPrimitives(D3DPT_TRIANGLELIST, 0, 0, grassCount * 12, 0, grassCount * 6);
}

//-----------------------------------------------------------------------------
// Name: HwGrassField
// Desc: -------------
//-----------------------------------------------------------------------------

HwGrassField::HwGrassField(HwEngine *eng) : HwSceneObject(eng)
{
	pIB = new HwIndexBuffer(eng->getRender());
	width = 256;
	height = 256;
	qWidth = 32;
	qHeight = 32;

	qCount = 0;
}

HwGrassField::~HwGrassField()
{
	for (int i = 0; i < qCount; i++)
		delete pQuads[i];
	delete[] pQuads;
}

void HwGrassField::init()
{
	DWORD w = width / qWidth;
	DWORD d = height / qHeight;
	qCount = w * d;

	pQuads = new HwGrassQuad *[qCount];
	gMap = new char[width * height];
	for (int i = 0; i < width * height; i++)
		gMap[i] = 0;

	for (int i = 0; i < d; i++)
		for (int j = 0; j < w; j++)
		{
			pQuads[i * w + j] = new HwGrassQuad(this);
			pQuads[i * w + j]->x = j;
			pQuads[i * w + j]->y = i;
			pQuads[i * w + j]->width = qWidth;
			pQuads[i * w + j]->height = qHeight;
			pQuads[i * w + j]->init();
		}
	pIB->bufferSize() =  qWidth * qHeight * 6 * 3;
	pIB->itemSize() = sizeof(DWORD);
	pIB->iFormat() = D3DFMT_INDEX32;
	pIB->init();

	DWORD *ind = (DWORD *)pIB->lock();
	int index = 0;
	int i = 0;
	while ( index < pIB->bufferSize())
	{
			ind[index] = i;       
			ind[index + 1] = i + 1;     
			ind[index + 2] = i + 2; 

			ind[index + 3] = i + 2; 
			ind[index + 4] = i + 3;   
			ind[index + 5] = i;       
			i += 4;
			index += 6;
		}
		
	pIB->unlock();
}
void HwGrassField::reInit()
{
	for (int i = 0; i < qCount; i++)
		pQuads[i]->reInit();
}

void HwGrassField::draw()
{
	
	if (attachedToTerrain)
		for (int i = 0; i < rendered.size(); i++)
			rendered[i]->render();
}

bool HwGrassField::isAttcahed()
{
	return attachedToTerrain;
}
HwTerrain *HwGrassField::getTerrain()
{
	return pTerrain;
}

char &HwGrassField::Map(int x, int y)
{
	return gMap[y * width + x];
}

HwGrassQuad *HwGrassField::getQuad(int x, int y)
{
	int w = width / qWidth;
	return pQuads[y * w + x];
}

int HwGrassField::getWidth()
{
	return width;
}

int HwGrassField::getHeight()
{
	return height;
}

void HwGrassField::attachTo(HwTerrain *terr)
{
	pTerrain = terr;
	attachedToTerrain = true;
};

void HwGrassField::addToRender(int x, int y)
{
	rendered.push_back(getQuad(x, y));
}

void HwGrassField::clearRender()
{
	rendered.resize(0);
}