#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwScene.h"
#include "hwBuffers.h"
#include "hwTexture.h"
#include "hwGrass.h"

using namespace std;

#define QUADSPERROW 2 //TO DO TYPES 2*2
#define QUADSTONODE QUADSPERROW * QUADSPERROW
//-----------------------------------------------------------------------------
// Name: HwTerrainQuad
// Desc: -------------
//-----------------------------------------------------------------------------

class HwTerrain;

struct HwTerrainVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXVECTOR2 tex;
};

class HwTerrainQuad
{
private:
	HwTerrain *pTerrain;

	HwVertexBuffer *pVB;
	int width, height;
	int x, y;

	D3DXVECTOR3 min; // AABB  min, max corners
	D3DXVECTOR3 max;
public:
	HwTerrainQuad(HwTerrain *terrain); 
	~HwTerrainQuad();

	void init();
	void reInit();
	void render();

	friend HwTerrain;
};

//-----------------------------------------------------------------------------
// Name: HwTerrain
// Desc: ---------
//-----------------------------------------------------------------------------

struct HwTerrainNode
{
	D3DXVECTOR3 min; 
	D3DXVECTOR3 max;
	HwTerrainNode *childs[4];

	virtual bool isLeaf() 
	{ 
		return false;
	}
};

struct HwTerrainLeaf : public HwTerrainNode
{
	HwTerrainQuad **pQuads;
	DWORD qCount;
	bool isLeaf()
	{
		return true;
	}
};

//-----------------------------------------------------------------------------
// Name: HwTerrain
// Desc: ---------
//-----------------------------------------------------------------------------

class HwTerrain : public HwSceneObject
{
private:
public:
	HwIndexBuffer *pIB;

	int width, height;
	int qWidth, qHeight;

	float *hMap;
	HwTerrainQuad **pQuads;
	DWORD qCount;

	HwTerrainNode *pTree;

	HwGrassField *pField;
protected:
	void bTree(HwTerrainNode *t, int minx, int miny, int maxx, int maxy);
	void drawTree(HwTerrainNode *t);
	void recalcTreeHeight(HwTerrainNode *t);
public:
	HwTerrain(HwEngine *eng);
	~HwTerrain();

	void init();
	void reInit();
	void draw();
	void buildTree();

	void setGrassField(HwGrassField *field);
	void setHeightFromTexture(HwTexture *pTexture, float maxAltitude);

	float &heightMap(int x, int y);
	float getHeight(float x, float z);

	HwTerrainQuad *getQuad(int x, int y);

	D3DXVECTOR3 getNormal(int x, int y);

	friend HwTerrainQuad;
};