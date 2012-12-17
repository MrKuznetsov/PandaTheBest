#include "hwTerrain.h"
#include "hwEngine.h"

#include <iostream>

//-----------------------------------------------------------------------------
// Name: HwTerrainQuad
// Desc: --------
//-----------------------------------------------------------------------------

HwTerrainQuad::HwTerrainQuad(HwTerrain *terrain)
{
	pTerrain = terrain;
	width = 32;
	height = 32;

	pVB = new HwVertexBuffer(pTerrain->getEngine()->getRender());
}

HwTerrainQuad::~HwTerrainQuad()
{
	delete pVB;
}

void HwTerrainQuad::init()
{
	pVB->bufferSize() = (width + 1) * (height + 1);
	pVB->itemSize() = sizeof(HwTerrainVertex);

	pVB->getDecl()->addElement(0, 0, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_POSITION, 0);
	pVB->getDecl()->addElement(0, 4 * 3, D3DDECLTYPE_FLOAT3, D3DDECLUSAGE_NORMAL, 0);
	pVB->getDecl()->addElement(0, 4 * 6, D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD, 0);

	pVB->init();

	reInit();
}

void HwTerrainQuad::reInit()
{
	int w = pTerrain->width;
	int d = pTerrain->height;

	float tu = 1.0f / w;
	float tv = 1.0f / d;
	float uw = float(width) / float(w);
	float vw = float(height) / float(d);

	D3DXVECTOR3 pos = D3DXVECTOR3(-w / 2 + width / 2 + x * width, 0, d / 2 - height / 2 - y * height);
	min = D3DXVECTOR3(pos.x - width / 2 ,  INT_MAX, pos.z - height / 2 );
	max = D3DXVECTOR3(pos.x + width / 2 , -INT_MAX, pos.z + height / 2 );

	HwTerrainVertex *v = (HwTerrainVertex *)pVB->lock();
	for (int i = 0; i <= height; i++)
		for (int j = 0; j <= width; j++)
		{
			int index = i * (width + 1) + j;
			float h = pTerrain->heightMap(x * width + j, y * height + i);
			v[index].pos = D3DXVECTOR3(pos.x - width / 2 + j, h, pos.z + height / 2 - i );	
			v[index].norm = pTerrain->getNormal(x * width + j, y * height + i);
			v[index].tex = D3DXVECTOR2(j * tu + uw * x, 1 - (i * tv + vw * y));
			if (h > max.y)
				max.y = h;
			if (h < min.y)
				min.y = h;
		}
	pVB->unlock();

}

void HwTerrainQuad::render()
{
	pVB->setStreamSource(0);
	pTerrain->pIB->setIndices();
	pTerrain->getEngine()->getRender()->drawIndexedPrimitives(D3DPT_TRIANGLELIST, 0, 0, pVB->bufferSize(), 0, pTerrain->pIB->bufferSize() / 3);
}

//-----------------------------------------------------------------------------
// Name: HwTerrain
// Desc: ---------
//-----------------------------------------------------------------------------

HwTerrain::HwTerrain(HwEngine *eng) : HwSceneObject(eng)
{
	pIB = new HwIndexBuffer(eng->getRender());
	width = 256;
	height = 256;
	qWidth = 32;
	qHeight = 32;

	pField = NULL;
}

HwTerrain::~HwTerrain()
{
	for (int i = 0; i < qCount; i++)
		delete pQuads[i];
	delete[] pQuads;
}

void HwTerrain::bTree(HwTerrainNode *t, int minx, int miny, int maxx, int maxy)
{
	
 /*max
	*-----*   0 1
	|     |   2 3
	|     |
	*-----*
		  min*/
	bool isLeaf = abs(minx - maxx) == 1;
	if (!isLeaf)
	{
		for (int i = 0; i < 4; i++)
			if  (abs(minx - maxx) > 2)
				t->childs[i] = new HwTerrainNode;
			else
				t->childs[i] = new HwTerrainLeaf;
		int cx = maxx + (minx - maxx) / 2;
		int cy = miny + (maxy - miny) / 2;
		bTree(t->childs[0], cx, cy, maxx, maxy);
		bTree(t->childs[1], minx, cy, cx, maxy);
		bTree(t->childs[2], cx, miny, maxx, cy);
		bTree(t->childs[3], minx, miny, cx, cy);
		t->min = t->childs[2]->min;
		t->max = t->childs[1]->max;
	}
	else
	{
		HwTerrainLeaf *p = (HwTerrainLeaf *)t;
		p->qCount = QUADSTONODE;
		p->pQuads = new HwTerrainQuad *[QUADSTONODE];
		DWORD w = (width / qWidth);
		int x = maxx * QUADSPERROW;
		int y = abs(maxy) * QUADSPERROW;
		for (int i = 0; i < QUADSPERROW; i++)
			for (int j = 0; j < QUADSPERROW; j++)
				p->pQuads[j * QUADSPERROW + i] = pQuads[(y + j) * w + x + i];
		p->min = p->pQuads[2]->min;
		p->max = p->pQuads[1]->max;	
	}
}

void HwTerrain::recalcTreeHeight(HwTerrainNode *t)
{
	if (t->isLeaf())
	{
		HwTerrainLeaf *p = (HwTerrainLeaf *)t;
		float miny = p->pQuads[0]->min.y;
		float maxy = p->pQuads[0]->max.y;
		for (int i = 1; i < p->qCount; i++)
		{
			miny = min(miny, p->pQuads[i]->min.y);
			maxy = max(maxy, p->pQuads[i]->max.y);
		}
		p->min.y = miny;
		p->max.y = maxy;
	}
	else
	{
		for (int i = 0; i < 4; i++)
			recalcTreeHeight(t->childs[i]);
		float miny = t->childs[0]->min.y;
		float maxy = t->childs[0]->max.y;
		for (int i = 1; i < 4; i++)
		{
			miny = min(miny, t->childs[i]->min.y);
			maxy = max(maxy, t->childs[i]->max.y);
		}
		t->min.y = miny;
		t->max.y = maxy;
	}
}

void HwTerrain::buildTree()
{
	pTree = new HwTerrainNode;
	bTree(pTree, (width / qWidth) / QUADSPERROW, -(height / qHeight) / QUADSPERROW, 0, 0);
	recalcTreeHeight(pTree);
	cout << pTree->min.x << " " << pTree->min.y << " " << pTree->min.z << '\n';
	cout << pTree->max.x << " " << pTree->max.y << " " << pTree->max.z << '\n';
}

void HwTerrain::drawTree(HwTerrainNode *t)
{
	if (!getEngine()->getScene()->getCamera()->boxInFrustum(t->min, t->max))
		return;
	if (!t->isLeaf())
		for (int i = 0; i < 4; i++)
			drawTree(t->childs[i]);
	else
		for (int i = 0; i < ((HwTerrainLeaf *)t)->qCount; i++)
		{
			((HwTerrainLeaf *)t)->pQuads[i]->render();
			if (pField)
				pField->addToRender(((HwTerrainLeaf *)t)->pQuads[i]->x, ((HwTerrainLeaf *)t)->pQuads[i]->y);
		}
}

void HwTerrain::init()
{
	DWORD w = (width / qWidth);
	DWORD d = (height / qHeight);
	qCount = w * d;

	pQuads = new HwTerrainQuad *[qCount];
	hMap = new float[(width + 1) * (height + 1)];
	for (int i = 0; i < (width + 1) * (height + 1); i++)
		hMap[i] = 0;

	for (int i = 0; i < d; i++)
		for (int j = 0; j < w; j++)
		{
			pQuads[i * w + j] = new HwTerrainQuad(this);
			pQuads[i * w + j]->x = j;
			pQuads[i * w + j]->y = i;
			pQuads[i * w + j]->width = qWidth;
			pQuads[i * w + j]->height = qHeight;
			pQuads[i * w + j]->init();
		}

	pIB->bufferSize() =  qWidth * qHeight * 6;
	pIB->itemSize() = sizeof(DWORD);
	pIB->iFormat() = D3DFMT_INDEX32;
	pIB->init();

	DWORD *ind = (DWORD *)pIB->lock();
	int index = 0;
	for (int i = 0; i < qHeight; i++)
		for (int j = 0; j < qWidth; j++)
		{
			ind[index] = i * (qWidth + 1) + j;           //ind++;
			ind[index + 1] = i * (qWidth + 1) + j + 1;       //ind++;
			ind[index + 2] = (i + 1) * (qWidth + 1) + j + 1; //ind++;

			ind[index + 3] = (i + 1) * (qWidth + 1) + j + 1; //ind++;
			ind[index + 4] = (i + 1) * (qWidth + 1) + j;     //ind++;
			ind[index + 5] = i * (qWidth + 1) + j;           //ind++;
			index += 6;
		}
		
	pIB->unlock();
}

void HwTerrain::reInit()
{
	for (int i = 0; i < qCount; i++)
		pQuads[i]->reInit();
}


// TO DO MATH!!!!!!!!!
D3DXVECTOR3 clackNormal(D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c)
{
	D3DXVECTOR3 out;
	D3DXVECTOR3 v1 = b - a;
	D3DXVECTOR3 v2 = c - a;
	D3DXVec3Cross(&out, &v1, &v2);
	return out;
}

D3DXVECTOR3 HwTerrain::getNormal(int x, int y)
{
	D3DXVECTOR3 v = D3DXVECTOR3(0, heightMap(x, y), 0);
	vector<D3DXVECTOR3> p;
	if (x > 0)
		p.push_back(D3DXVECTOR3(-1, heightMap(x - 1, y), 0));
	if (y > 0)
		p.push_back(D3DXVECTOR3(0, heightMap(x , y - 1), 1));
	if (x < width)
		p.push_back(D3DXVECTOR3(1, heightMap(x + 1, y), 0));
	if (y < height)
		p.push_back(D3DXVECTOR3(0, heightMap(x , y - 1), -1));

	D3DXVECTOR3 n = clackNormal(p[0], p[1], v);
	if (p.size() > 2)
	{
		D3DXVECTOR3 n1 = clackNormal(p[1], p[2], v);
		n = n + n1;
	}
	if (p.size() > 3)
	{
		D3DXVECTOR3 n1 = clackNormal(p[2], p[3], v);
		n = n + n1;
	}
	D3DXVec3Normalize(&n, &n);
	return n;
}

void HwTerrain::draw()
{
	//for (int i = 0; i < qCount; i++)
	//	pQuads[i]->render();
	if (pField)
		pField->clearRender();
	drawTree(pTree);
}

float &HwTerrain::heightMap(int x, int y)
{
	return hMap[y * (width + 1) + x];
}

HwTerrainQuad *HwTerrain::getQuad(int x, int y)
{
	DWORD w = (width / qWidth);

	return  pQuads[y * w + x];
}

void HwTerrain::setHeightFromTexture(HwTexture *pTexture, float maxAltitude)
{
	for (int i = 0; i <= width; i++)
		for (int j = 0; j <= height; j++)
			heightMap(i, j) = pTexture->getPixelColor( int(float(i) / (width + 1) * (pTexture->getWidth() - 1)),
				int(float(j) / (height + 1) * (pTexture->getHeight() - 1))).r * maxAltitude;
	reInit();
}

void HwTerrain::setGrassField(HwGrassField *field)
{
	pField = field;
	pField->attachTo(this);
}

// TO DO MATH
inline float lerp(float a, float b, float t)
{
	return a - (a * t) + (b * t);
}

float HwTerrain::getHeight(float x, float z)
{
	int ww = width + 1;//(width / qWidth) * (qWidth + 1);
	int hh = height + 1;//(height / qHeight) * (qHeight + 1);
	float px = width / 2.0f + x;
	float pz = height / 2.0f - z;
	if (px < 0 || px > width -1 || pz < 0 || pz > height -1)
		return 0;

	float a = hMap[int(pz) * ww + int(px)];
	float b = hMap[int(pz) * ww + int(px) + 1];
	float c = hMap[(int(pz) + 1) * ww + int(px)];
	float d = hMap[(int(pz) + 1) * ww + int(px) + 1];

	float dx = px - int(px);
	float dz = pz - int(pz);

	if (dz < 1 - dx) // top triangle
	{
		float uy = b - a; // a -> b
		float vy = c - a; // a -> c
		return a + lerp(0.0f, uy, dx) + lerp(0.0f, vy, dz);
	}
	else
	{
		float uy = c - d; // d -> c
		float vy = b - d; // d -> b
		return d + lerp(0.0f, uy, 1.0f - dx) + lerp(0.0f, vy, 1.0f - dz);
	}
}