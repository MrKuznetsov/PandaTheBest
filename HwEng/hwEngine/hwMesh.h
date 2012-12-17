#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwResources.h"
#include "hwMisc.h"
#include "hwBuffers.h"
#include "hwTypes.h"
#include "hwSkeleton.h"

using namespace std;

//-----------------------------------------------------------------------------
// Name: HwSurface
// Desc: ---------
//-----------------------------------------------------------------------------

class HwMesh;
class HwShader;

struct HwRendringVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
};

struct HwHardRendringVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
	byte indicies[4];
	float weights[4];
};

struct HwMeshVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
	D3DXVECTOR3 normal;
	DWORD boneCount;
	DWORD *bones;
	float* weights;
};

class HwSurface
{
public:
	HwMesh *pParent;
	//int id;

	HwMeshVertex *vertices;
	DWORD vSize;
	DWORD *indices;
	DWORD iSize;

	HwVertexBuffer *pVB;
	HwIndexBuffer *pIB;

	HWANIMATIONTYPE aType;
	HWANIMRENDER rendType;
protected:
	void setSize(DWORD vS, DWORD iS);
	void setBoneSize(int index, DWORD size);
public:
	HwSurface(HwMesh *m);

	void init();
	void renderStatic();
	void renderAnimSoftware(float time = 0);
	void renderAnimHardware(float time = 0);

	void render(float time = 0);

	friend HwMesh;
};

//-----------------------------------------------------------------------------
// Name: HwMeshMaager
// Desc: ------------
//-----------------------------------------------------------------------------

class HwMeshManager : public HwResourceManager
{
public:
	HwMeshManager(HwEngine *eng) : HwResourceManager(eng){}
	HwResource *newRes();
};

//-----------------------------------------------------------------------------
// Name: HwMesh
// Desc: ------
//-----------------------------------------------------------------------------

#define HWMT_SUB 16
#define HWMT_ANIM 32
#define HWMT_END 13

class HwMesh : public HwResource
{
public:
	HwSkeleton *pSkelet;
	vector<HwSurface *> list;

	bool isAnimated;
	HWANIMRENDER rendType;
private:
	void readAnimSub();
	void readAnim();
	void readSub();
	void loadSkin();
	void loadStatic();
public:
	HwMesh(HwResourceManager *m);

	void init();
	void render(float time = 0, int animID = 0, HwShader *pShd = NULL);

	void load();

	void setAnimated(bool is);

	HwSkeleton *getSkeleton();

	HwSurface *newSurface(string name);

	friend HwSurface;
};