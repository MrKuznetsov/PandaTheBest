#pragma once

#include <d3dx9.h>
#include <vector>
#include <string>

#include "hwMisc.h"
#include "hwMaterials.h"
#include "hwTexture.h"
#include "hwMesh.h"
#include "hwShaders.h"

using namespace std;

class HwEngine;
class HwSceneObject;

typedef void (*SENDERFUNC)(HwSceneObject *sender);

//-----------------------------------------------------------------------------
// Name: HwRenderObject
// Desc: --------------
//-----------------------------------------------------------------------------
class HwRenderObject : public HwObject
{
private:
	//HwEngine *pEngine;
	bool visible;
public:
	HwRenderObject(HwEngine *eng):HwObject(eng), visible(true) {}
	~HwRenderObject(){};

	void setVisible(bool v) { visible = v;}
	bool getVisible() { return visible;}

	virtual void render() = 0;
	virtual void draw() = 0;
};

//-----------------------------------------------------------------------------
// Name: HwSceneObject
// Desc: --------------
//-----------------------------------------------------------------------------
class HwSceneObject : public HwRenderObject
{
private:
   // static int id;
	string name;

	D3DXVECTOR3 pos;
	D3DXVECTOR3 ang;
	D3DXVECTOR3 size;

	D3DXMATRIX worldTransform;

	bool sizeChanged;
	bool posChanged;
	bool angChanged;

	HwMaterial *pMaterial;

	SENDERFUNC func;
public:
	void reCalcWorldMatrix();
public:
	HwSceneObject(HwEngine *eng);

	float distantioTo(HwSceneObject *obj);
	float distantioTo(D3DXVECTOR3 v);
	void turnTo(HwSceneObject *obj);
	void turnTo(D3DXVECTOR3 v);
	void moveForward(float dist);

	void setPos(D3DXVECTOR3 v);
	void setAng(D3DXVECTOR3 v);
	void setDirection(D3DXVECTOR3 v);
	void setSize(D3DXVECTOR3 v);
	D3DXVECTOR3 &getPos();
	D3DXVECTOR3 &getAng();
	D3DXVECTOR3 &getSize();
	D3DXMATRIX &worldMatrix();

	void setMaterial(string name);
	void setMaterial(HwMaterial *m);
	HwMaterial *getMaterial();

	void setRenderFunc(SENDERFUNC f);

	void render();
	void draw();
};


//-----------------------------------------------------------------------------
// Name: HwScene
// Desc: -------
//-----------------------------------------------------------------------------
class HwScene : public HwObject
{
private:
	HwCamera *pCamera;

	HwMaterialManager *pMatManger;
	HwTextureManager *pTexManager;
	HwMeshManager *pMeshManager;

	HwShaderManager *pShaderManager;
	HwVertexProgrammManager *pVProgrammManger;
	HwPixelProgrammManager *pPProgrammManger;

	vector<HwSceneObject *> sObjects;
public:
	HwScene(HwEngine *eng);
	~HwScene();

	void init();
	void render();

	void addObject(HwSceneObject *obj);

	HwMaterialManager *materials();
	HwTextureManager *textures();
	HwMeshManager *meshes();
	HwShaderManager *shaders();
	HwVertexProgrammManager *vps();
	HwPixelProgrammManager *pps();

	HwCamera *getCamera();
	void setCamera(HwCamera *cam);
};

