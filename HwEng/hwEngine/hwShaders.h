#pragma once

#include <string>
#include <vector>

#include "hwResources.h"

class HwShaderTechnique;
class HwShaderPass;

//-----------------------------------------------------------------------------
// Name: HwShader
// Desc: --------
//-----------------------------------------------------------------------------

class HwShader : public HwResource
{
private:
	int currentTechnique;
	vector<HwShaderTechnique *> list;
public:
	HwShader(HwResourceManager *m);
	~HwShader();

	HwShaderTechnique *newTechnique();
	HwShaderPass *newPass();

	HwShaderTechnique *getTechnique(int index);
	HwShaderPass *getPass(int index);
	int getPassCount();

	bool isValible();

	void setTechnique(int index);
};

//-----------------------------------------------------------------------------
// Name: HwShaderManager
// Desc: ---------------
//-----------------------------------------------------------------------------

class HwShaderManager : public HwResourceManager
{
public:
	HwShaderManager(HwEngine *eng) : HwResourceManager(eng){}
	HwResource *newRes();
};

//-----------------------------------------------------------------------------
// Name: HwShaderTechnique
// Desc: -----------------
//-----------------------------------------------------------------------------

class HwShaderTechnique
{
private:
	HwShader *pParent;
	vector<HwShaderPass *> list;
public:
	~HwShaderTechnique();
	string name;
	int id;

	HwShaderPass *newPass();
	HwShaderPass *getPass(int index);
	int getCount();

	friend HwShader;
	friend HwShaderPass;
};

class HwVertexProgramm;
class HwPixelProgramm;

//-----------------------------------------------------------------------------
// Name: HwShaderPass
// Desc: ------------
//-----------------------------------------------------------------------------

class HwShaderPass
{
private:
	HwShaderTechnique *pParent;

	HwVertexProgramm *pVP;
	HwPixelProgramm *pPP;
public:
	HwShaderPass();
	~HwShaderPass();

	void setVP(HwVertexProgramm *pV);
	void setPP(HwPixelProgramm *pP);
	HwVertexProgramm *setVP(string name);
	HwPixelProgramm *setPP(string name);
	HwVertexProgramm *setVPFromFile(string name, string fn);
	HwPixelProgramm *setPPFromFile(string name, string fn);

	HwVertexProgramm *vp();
	HwPixelProgramm *pp();

	void Bind();
	void UnBind();

	friend HwShaderTechnique;
};

//-----------------------------------------------------------------------------
// Name: HwProgramm
// Desc: ----------------
//-----------------------------------------------------------------------------

class HwProgramm : public HwResource
{
protected:
	LPD3DXCONSTANTTABLE pConstTable;

	string version;
	string mainFunc;
public:
	HwProgramm(HwResourceManager *m);

	void load();

	virtual void compile() = 0;
	virtual void compile(string s) = 0;

	virtual void bind() = 0;
	virtual void unBind() = 0;

	int getConstIndex(string name);

	bool setValue(string name, void *pData, DWORD bytes);

	bool setConst(string name, bool value);
	bool setConst(string name, bool *pValue, DWORD count);
	bool setConst(string name, int value);
	bool setConst(string name, int *pValue, DWORD count);
	bool setConst(string name, float value);
	bool setConst(string name, float *pValue, DWORD count);
	bool setConst(string name, D3DXVECTOR3 value);
	bool setConst(string name, D3DXVECTOR3 *pValue, DWORD count);
	bool setConst(string name, D3DXVECTOR4 value);
	bool setConst(string name, D3DXVECTOR4 *pValue, DWORD count);
	bool setConst(string name, D3DXMATRIX value);
	bool setConst(string name, D3DXMATRIX *pValue, DWORD count);
};

//-----------------------------------------------------------------------------
// Name: HwVertexProgramm
// Desc: ----------------
//-----------------------------------------------------------------------------

class HwVertexProgramm : public HwProgramm
{
private:
	LPDIRECT3DVERTEXSHADER9 pShader;
	LPD3DXBUFFER pCode;
public:
	HwVertexProgramm(HwResourceManager *m) : HwProgramm(m){}
	void createShader();
	void compile();
	void compile(string s);

	void bind();
	void unBind();	
};

//-----------------------------------------------------------------------------
// Name: HwPixelProgramm
// Desc: ---------------
//-----------------------------------------------------------------------------

class HwPixelProgramm : public HwProgramm
{
private:
	LPDIRECT3DPIXELSHADER9 pShader;
	LPD3DXBUFFER pCode;
public:
	HwPixelProgramm(HwResourceManager *m) : HwProgramm(m){}
	void createShader();
	void compile();
	void compile(string s);

	void bind();
	void unBind();	
};

//-----------------------------------------------------------------------------
// Name: HwVertexProgrammManager
// Desc: -----------------------
//-----------------------------------------------------------------------------

class HwVertexProgrammManager : public HwResourceManager
{
public:
	HwVertexProgrammManager(HwEngine *eng) : HwResourceManager(eng){}
	HwResource *newRes();
};
//-----------------------------------------------------------------------------
// Name: HwPixelProgrammManager
// Desc: -----------------------
//-----------------------------------------------------------------------------

class HwPixelProgrammManager : public HwResourceManager
{
public:
	HwPixelProgrammManager(HwEngine *eng) : HwResourceManager(eng){}
	HwResource *newRes();
};