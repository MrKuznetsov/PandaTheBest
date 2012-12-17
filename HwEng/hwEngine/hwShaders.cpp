#include "hwShaders.h"
#include "hwEngine.h"

//-----------------------------------------------------------------------------
// Name: HwShader
// Desc: --------
//-----------------------------------------------------------------------------

HwShader::HwShader(HwResourceManager *m) : HwResource(m)
{
	currentTechnique = 0;
};

HwShaderTechnique *HwShader::newTechnique()
{
	HwShaderTechnique *t = new HwShaderTechnique;
	list.push_back(t);
	t->id = list.size() - 1;
	t->pParent = this;

	return t;
}

HwShaderPass *HwShader::newPass()
{
	return list[currentTechnique]->newPass();
}

HwShaderPass *HwShader::getPass(int index)
{
	return list[currentTechnique]->getPass(index);
}

int HwShader::getPassCount()
{
	return list[currentTechnique]->getCount();
}

HwShaderTechnique *HwShader::getTechnique(int index)
{
	if (index < 0 && index > list.size() - 1)
		return 0;
	return list[index];
}

void HwShader::setTechnique(int index)
{
	if (index < 0 && index > list.size() - 1)
		return;
	currentTechnique = index;
}

HwShader::~HwShader()
{
	for (int i = 0; i < list.size(); i++)
		delete list[i];
}

bool HwShader::isValible()
{
	if (list.size() > 0)
		if (getPassCount() > 0)
			return true;
	return false;
}

//-----------------------------------------------------------------------------
// Name: HwShaderManager
// Desc: ---------------
//-----------------------------------------------------------------------------

HwResource *HwShaderManager::newRes()
{
	return new HwShader(this);
}

//-----------------------------------------------------------------------------
// Name: HwShaderTechnique
// Desc: -----------------
//-----------------------------------------------------------------------------

HwShaderTechnique::~HwShaderTechnique()
{
	for (int i = 0; i < list.size(); i++)
		delete list[i];
}

HwShaderPass *HwShaderTechnique::newPass()
{
	HwShaderPass *p = new HwShaderPass;
	list.push_back(p);
	p->pParent = this;

	return p;
}

HwShaderPass *HwShaderTechnique::getPass(int index)
{
	if (index < 0 && index > list.size() - 1)
		return 0;
	return list[index];
}

int HwShaderTechnique::getCount()
{
	return list.size();
}

//-----------------------------------------------------------------------------
// Name: HwShaderPass
// Desc: ------------
//-----------------------------------------------------------------------------

HwShaderPass::HwShaderPass()
{
	pPP = NULL;
	pVP = NULL;
}

HwShaderPass::~HwShaderPass()
{

}

void HwShaderPass::setVP(HwVertexProgramm *pV)
{
	pVP = pV;
}

void HwShaderPass::setPP(HwPixelProgramm *pP)
{
	pPP = pP;
}

HwVertexProgramm *HwShaderPass::setVP(string name)
{
	pVP = (HwVertexProgramm *)pParent->pParent->getManager()->getScene()->vps()->getByName(name);
	return pVP;
}

HwPixelProgramm *HwShaderPass::setPP(string name)
{
	pPP = (HwPixelProgramm *)pParent->pParent->getManager()->getScene()->pps()->getByName(name);
	return pPP;
}

HwVertexProgramm *HwShaderPass::setVPFromFile(string name, string fn)
{
	pVP = (HwVertexProgramm *)pParent->pParent->getManager()->getScene()->vps()->CreateOrRetrieveRes(name, fn);
	return pVP;
}

HwPixelProgramm *HwShaderPass::setPPFromFile(string name, string fn)
{
	pPP = (HwPixelProgramm *)pParent->pParent->getManager()->getScene()->pps()->CreateOrRetrieveRes(name, fn);
	return pPP;
}

HwVertexProgramm *HwShaderPass::vp()
{
	return pVP;
}

HwPixelProgramm *HwShaderPass::pp()
{
	return pPP;
}

void HwShaderPass::Bind()
{
	if (pVP)
		pVP->bind();
	if (pPP)
		pPP->bind();
}

void HwShaderPass::UnBind()
{
	if (pVP)
		pVP->unBind();
	if (pPP)
		pPP->unBind();
}

//-----------------------------------------------------------------------------
// Name: HwProgramm
// Desc: ----------------
//-----------------------------------------------------------------------------

HwProgramm::HwProgramm(HwResourceManager *m) : HwResource(m)
{
	version = "2_0";
	mainFunc = "main";
}

void HwProgramm::load()
{
	compile();
}

int HwProgramm::getConstIndex(string name)
{
	return -1;
}

bool HwProgramm::setValue(string name, void *pData, DWORD bytes)
{
	pConstTable->SetValue(getManager()->getRender()->getDevice(), name.c_str(), pData, bytes);

	return true;
}

bool HwProgramm::setConst(string name, bool value)
{
	pConstTable->SetBool(getManager()->getRender()->getDevice(), name.c_str(), value);

	return true;
}

bool HwProgramm::setConst(string name, bool *pValue, DWORD count)
{
	pConstTable->SetBoolArray(getManager()->getRender()->getDevice(), name.c_str(), (BOOL *)(pValue), count);

	return true;
}

bool HwProgramm::setConst(string name, int value)
{
	pConstTable->SetInt(getManager()->getRender()->getDevice(), name.c_str(), value);

	return true;
}

bool HwProgramm::setConst(string name, int *pValue, DWORD count)
{
	pConstTable->SetIntArray(getManager()->getRender()->getDevice(), name.c_str(), pValue, count);

	return true;
}

bool HwProgramm::setConst(string name, float value)
{
	pConstTable->SetFloat(getManager()->getRender()->getDevice(), name.c_str(), value);

	return true;
}

bool HwProgramm::setConst(string name, float *pValue, DWORD count)
{
	pConstTable->SetFloatArray(getManager()->getRender()->getDevice(), name.c_str(), pValue, count);

	return true;
}

bool HwProgramm::setConst(string name, D3DXVECTOR3 value)
{
	pConstTable->SetVector(getManager()->getRender()->getDevice(), name.c_str(), &D3DXVECTOR4(value, 0));

	return true;
	//return setValue(name, &value, sizeof(D3DXVECTOR3));
}

bool HwProgramm::setConst(string name, D3DXVECTOR3 *pValue, DWORD count)
{
	return setValue(name, pValue, sizeof(D3DXVECTOR3) * count);
}

bool HwProgramm::setConst(string name, D3DXVECTOR4 value)
{
	pConstTable->SetVector(getManager()->getRender()->getDevice(), name.c_str(), &value);

	return true;
}

bool HwProgramm::setConst(string name, D3DXVECTOR4 *pValue, DWORD count)
{
	pConstTable->SetVectorArray(getManager()->getRender()->getDevice(), name.c_str(), pValue, count);

	return true;
}

bool HwProgramm::setConst(string name, D3DXMATRIX value)
{
	pConstTable->SetMatrix(getManager()->getRender()->getDevice(), name.c_str(), &value);

	return true;
}

bool HwProgramm::setConst(string name, D3DXMATRIX *pValue, DWORD count)
{
	pConstTable->SetMatrixArray(getManager()->getRender()->getDevice(), name.c_str(), pValue, count);

	return true;
}

//-----------------------------------------------------------------------------
// Name: HwVertexProgramm
// Desc: ----------------
//-----------------------------------------------------------------------------

void HwVertexProgramm::compile()
{
	unsigned int size = getSize();
	char *buffer = new char[size];
	f->seekg(0, ios::beg);
	f->read(buffer, size);

	string ver = "vs_" + version;

	D3DXCompileShader(buffer, size, NULL, NULL, mainFunc.c_str(), ver.c_str(), 0, &pCode, NULL, &pConstTable);
	createShader();
}

void HwVertexProgramm::compile(string s)
{
	string ver = "vs_" + version;

	D3DXCompileShader(s.c_str(), s.size(), NULL, NULL, mainFunc.c_str(), ver.c_str(), 0, &pCode, NULL, &pConstTable);
	createShader();
}

void HwVertexProgramm::createShader()
{
	getManager()->getRender()->getDevice()->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &pShader);
}

void HwVertexProgramm::bind()
{
	getManager()->getRender()->getDevice()->SetVertexShader(pShader);
}

void HwVertexProgramm::unBind()
{
	getManager()->getRender()->getDevice()->SetVertexShader(NULL);
}

//-----------------------------------------------------------------------------
// Name: HwPixelProgramm
// Desc: ----------------
//-----------------------------------------------------------------------------

void HwPixelProgramm::compile()
{
	unsigned int size = getSize();
	char *buffer = new char[size];
	f->seekg(0, ios::beg);
	f->read(buffer, size);

	string ver = "ps_" + version;

	D3DXCompileShader(buffer, size, NULL, NULL, mainFunc.c_str(), ver.c_str(), 0, &pCode, NULL, &pConstTable);
	createShader();
}

void HwPixelProgramm::compile(string s)
{
	string ver = "ps_" + version;

	D3DXCompileShader(s.c_str(), s.size(), NULL, NULL, mainFunc.c_str(), ver.c_str(), 0, &pCode, NULL, &pConstTable);
	createShader();
}

void HwPixelProgramm::createShader()
{
	getManager()->getRender()->getDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pShader);
}

void HwPixelProgramm::bind()
{
	getManager()->getRender()->getDevice()->SetPixelShader(pShader);
}

void HwPixelProgramm::unBind()
{
	getManager()->getRender()->getDevice()->SetPixelShader(NULL);
}

//-----------------------------------------------------------------------------
// Name: HwVertexProgrammManager
// Desc: -----------------------
//-----------------------------------------------------------------------------

HwResource *HwVertexProgrammManager::newRes()
{
	return new HwVertexProgramm(this);
}

//-----------------------------------------------------------------------------
// Name: HwPixelProgrammManager
// Desc: -----------------------
//-----------------------------------------------------------------------------

HwResource *HwPixelProgrammManager::newRes()
{
	return new HwPixelProgramm(this);
}