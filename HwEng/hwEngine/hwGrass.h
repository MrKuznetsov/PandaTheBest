#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwScene.h"
#include "hwBuffers.h"
#include "hwTexture.h"

using namespace std;

struct HwGrassRenderVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
};

//-----------------------------------------------------------------------------
// Name: HwGrassQuad
// Desc: -------------
//-----------------------------------------------------------------------------

class HwGrassField;
class HwTerrain;

class HwGrassQuad
{
private:
	HwGrassField *pField;

	HwVertexBuffer *pVB;
	int width, height;
	int x, y;

	int grassCount;

	D3DXVECTOR3 min; // AABB  min, max corners
	D3DXVECTOR3 max;
public:
	HwGrassQuad(HwGrassField *filed); 
	~HwGrassQuad();

	void init();
	void reInit();
	void render();

	friend HwGrassField;
};

//-----------------------------------------------------------------------------
// Name: HwGrassField
// Desc: -------------
//-----------------------------------------------------------------------------

class HwGrassField : public HwSceneObject
{
private:
	char *gMap; // 0..1 map[s*s] 0 - no grass 1 - grass

	bool attachedToTerrain;
	HwTerrain *pTerrain;

	int width, height;
	int qWidth, qHeight;

	HwGrassQuad **pQuads;
	DWORD qCount;

	HwIndexBuffer *pIB;

	vector<HwGrassQuad *> rendered;
public:
	HwGrassField(HwEngine *eng);
	~HwGrassField();

	void init();
	void reInit();
	void draw();

	bool isAttcahed();
	HwTerrain *getTerrain();

	char &Map(int x, int y);
	HwGrassQuad *getQuad(int x, int y);

	int getWidth();
	int getHeight();

	void addToRender(int x, int y);
	void clearRender();

	void attachTo(HwTerrain *terr);

	friend HwGrassQuad;
};