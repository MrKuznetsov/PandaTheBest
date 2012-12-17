#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwScene.h"
#include "hwMesh.h"
#include "hwTypes.h"

using namespace std;

//-----------------------------------------------------------------------------
// Name: HwEntitie
// Desc: ---------
//-----------------------------------------------------------------------------

class HwEntitie : public HwSceneObject
{
public:
	HwMesh *pMesh;

	int curentFrame;
	float updateTime; // 0.0 to 1.0
	int curentAnimation;
	HWANIMATIONMODE aMode; // TO DO MODES
	D3DXMATRIX *updates;

	// TO DO RENDER TYPE
public:
	HwEntitie(HwEngine *eng);

	HwMesh *loadMesh(string name, string fn);
	void setMesh(string name);
	void setMesh(HwMesh *mesh);

	void setAnimation(string name);
	void setAnimation(int id);

	float &animTime();

	void draw();
};