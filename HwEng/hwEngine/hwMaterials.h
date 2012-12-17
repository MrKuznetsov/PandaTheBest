#pragma once

#include <string>
#include <vector>

#include "hwResources.h"
#include "hwTexture.h"
#include "hwShaders.h"

using namespace std;


class HwTechnique;
class HwPass;
class HwStage;

//-----------------------------------------------------------------------------
// Name: HwMaterial
// Desc: ----------
//-----------------------------------------------------------------------------

class HwMaterial : public HwResource
{
private:
	int currentTechnique;
	vector<HwTechnique *> list;
public:
	HwMaterial(HwResourceManager *m);
	~HwMaterial();

	HwTechnique *newTechnique();
	HwPass *newPass();

	HwTechnique *getTechnique(int index);
	HwPass *getPass(int index);
	int getPassCount();
	int getCurrTechn();

	void setTechnique(int index);
};

//-----------------------------------------------------------------------------
// Name: HwMaterialManager
// Desc: -----------------
//-----------------------------------------------------------------------------

class HwMaterialManager : public HwResourceManager
{
public:
	HwMaterialManager(HwEngine *eng);
	HwResource *newRes();
};

//-----------------------------------------------------------------------------
// Name: HwTechnique
// Desc: -----------
//-----------------------------------------------------------------------------

class HwTechnique
{
private:
	HwMaterial *pParent;
	vector<HwPass *> list;
public:
	~HwTechnique();
	string name;
	int id;

	HwPass *newPass();
	HwPass *getPass(int index);
	int getCount();

	friend HwMaterial;
	friend HwPass;
	friend HwStage;
};

struct HwRenderOptions
{
	D3DCULL cullMode;
	D3DFILLMODE fillMode;
	D3DSHADEMODE shadeMode;
};

struct HwBlend
{
	D3DBLENDOP opertaion;
	D3DBLEND src;
	D3DBLEND dest;
	D3DCMPFUNC func;
	int alphaRef;
};

struct HwZBuffer
{
	D3DZBUFFERTYPE type;
	bool writeEnable;
	D3DCMPFUNC func;
	float bias;
	float slopeBias;
};

struct HwStencil
{
	bool enable;
	D3DCMPFUNC func;
	int ref;
	D3DSTENCILOP fail;
	D3DSTENCILOP zFail;
	D3DSTENCILOP pass;
	DWORD mask;
	bool twoSide;
};

struct HwFog
{
	D3DFOGMODE mode;
	D3DCOLORVALUE color;
	DWORD start;
	DWORD end;
	DWORD densitiy;
};

//-----------------------------------------------------------------------------
// Name: HwPass
// Desc: --------
//-----------------------------------------------------------------------------

class HwPass
{
private:
	HwTechnique *pParent;
	vector<HwStage *> stageList;
	HwShader *pShader;
public:
	HwPass();
	~HwPass();

	D3DMATERIAL9 matColor;
	HwRenderOptions renderOptions;
	HwBlend blend;
	HwZBuffer zBuffer;
	HwStencil stencil;
	HwFog fog;
	D3DCOLORVALUE ambient;
	bool lightEnable;

	HwStage *newStage();
	HwStage *getStage(int index);
	int getStageCount();


	void setShader(string name);
	void setShaderFromFile(string fn);
	void setShader(HwShader *shd);
	HwShader *getShader();
	HwShaderTechnique *getShaderPass(int index);
	HwShaderPass *getShaderTechnique(int index);
	int getShaderPassCount();
	bool shaderValible();

	void setDefault();
	void Bind();

	friend HwStage;
	friend HwTechnique;
};

//-----------------------------------------------------------------------------
// Name: HwStage
// Desc: --------
//-----------------------------------------------------------------------------

struct HwTexBlend
{
	D3DTEXTUREOP colorBlendType;
	D3DTEXTUREOP alphaBlendType;

	DWORD colorArg1;
	DWORD colorArg2;
	DWORD colorArg0;

	DWORD alphaArg1;
	DWORD alphaArg2;
	DWORD alphaArg0;

	DWORD texCoordIndex;
};

class HwStage
{
private:
	HwPass *pParent;
	bool reMat;
	int index;
protected:
	void recallMatrix();
public:
	HwStage();

	HwTexture *pTexture;
	int texIndex;

	HwTexBlend blend;
	D3DBLEND srcBlend;
	D3DBLEND destBlend;
	D3DTEXTUREFILTERTYPE mag;
	D3DTEXTUREFILTERTYPE min;
	D3DTEXTUREFILTERTYPE mip;
	D3DTEXTUREADDRESS addressU;
	D3DTEXTUREADDRESS addressV;
	D3DTEXTUREADDRESS addressW;

	float scroll[2];
	float scale[2];
	float angle;

	D3DXMATRIX matrix;

	void Bind();
	void setDefault();

	void setTexture(HwTexture *tex);
	HwTexture *setTexture(string name);
	HwTexture *setTextureFromFile(string name, string fn);
	HwTexture *getTexture();

	friend HwPass;
};


