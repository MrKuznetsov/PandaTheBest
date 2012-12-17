#include "hwMaterials.h"
#include "hwEngine.h"
#include "hwRender.h"

#include <iostream>
//-----------------------------------------------------------------------------
// Name: HwMaterial
// Desc: ----------
//-----------------------------------------------------------------------------

HwMaterial::HwMaterial(HwResourceManager *m) : HwResource(m)
{
	currentTechnique = 0;
};

HwTechnique *HwMaterial::newTechnique()
{
	HwTechnique *t = new HwTechnique;
	list.push_back(t);
	t->id = list.size() - 1;
	t->pParent = this;

	return t;
}

HwPass *HwMaterial::newPass()
{
	return list[currentTechnique]->newPass();
}

HwPass *HwMaterial::getPass(int index)
{
	return list[currentTechnique]->getPass(index);
}

int HwMaterial::getPassCount()
{
	return list[currentTechnique]->getCount();
}

int HwMaterial::getCurrTechn()
{
	return currentTechnique;
}


HwTechnique *HwMaterial::getTechnique(int index)
{
	if (index < 0 && index > list.size() - 1)
		return 0;
	return list[index];
}

void HwMaterial::setTechnique(int index)
{
	if (index < 0 && index > list.size() - 1)
		return;
	currentTechnique = index;
}

HwMaterial::~HwMaterial()
{
	for (int i = 0; i < list.size(); i++)
		delete list[i];
}

//-----------------------------------------------------------------------------
// Name: HwMaterialManager
// Desc: -----------------
//-----------------------------------------------------------------------------

HwMaterialManager::HwMaterialManager(HwEngine *eng) : HwResourceManager(eng)
{
	((HwMaterial *)(createImpl("Default")))->newTechnique()->newPass()->newStage();
}

HwResource *HwMaterialManager::newRes()
{
	return new HwMaterial(this);
}

//-----------------------------------------------------------------------------
// Name: HwTechnique
// Desc: -----------
//-----------------------------------------------------------------------------
HwTechnique::~HwTechnique()
{
	for (int i = 0; i < list.size(); i++)
		delete list[i];
}

HwPass *HwTechnique::newPass()
{
	HwPass *p = new HwPass;
	list.push_back(p);
	p->pParent = this;

	return p;
}

HwPass *HwTechnique::getPass(int index)
{
	if (index < 0 && index > list.size() - 1)
		return 0;
	return list[index];
}

int HwTechnique::getCount()
{
	return list.size();
}

//-----------------------------------------------------------------------------
// Name: HwPass
// Desc: --------
//-----------------------------------------------------------------------------

HwPass::HwPass()
{
	setDefault();
	pShader = NULL;
}

HwPass::~HwPass()
{
	for (int i = 0; i < stageList.size(); i++)
		delete stageList[i];
}

HwStage *HwPass::newStage()
{
	HwStage *s = new HwStage;
	stageList.push_back(s);
	s->index = stageList.size() - 1;
	s->pParent = this;

	return s;
}

HwStage *HwPass::getStage(int index)
{
	if (index < 0 && index > stageList.size() - 1)
		return 0;
	return stageList[index];
}

int HwPass::getStageCount()
{
	return stageList.size();
}

void HwPass::setShader(string name)
{
	pShader = (HwShader *)pParent->pParent->getManager()->getScene()->shaders()->getByName(name);
}

void HwPass::setShaderFromFile(string fn)
{
	pShader = (HwShader *)pParent->pParent->getManager()->getScene()->shaders()->CreateOrRetrieveRes(" ", fn);
}

void HwPass::setShader(HwShader *shd)
{
	pShader = shd;
}

HwShader *HwPass::getShader()
{
	return pShader;
}

HwShaderTechnique *HwPass::getShaderPass(int index)
{
	return pShader->getTechnique(index);
}

HwShaderPass *HwPass::getShaderTechnique(int index)
{
	return pShader->getPass(index);
}

int HwPass::getShaderPassCount()
{
	return pShader->getPassCount();
}
bool HwPass::shaderValible()
{
	return pShader ? pShader->isValible() : false;
}

void HwPass::Bind()
{
	HwRender *render = pParent->pParent->getManager()->getEngine()->getRender();
	
	render->setRenderState(D3DRS_CULLMODE, renderOptions.cullMode);
	render->setRenderState(D3DRS_FILLMODE, renderOptions.fillMode);
	render->setRenderState(D3DRS_SHADEMODE, renderOptions.shadeMode);

	render->setMaterial(&matColor);

	//BLENDING
	if (blend.src == D3DBLEND_ONE && blend.dest == D3DBLEND_ZERO)
		render->setRenderState(D3DRS_ALPHABLENDENABLE, 0);
	else
	{
		render->setRenderState(D3DRS_ALPHABLENDENABLE, 1);
		render->setRenderState(D3DRS_SRCBLEND, blend.src);
		render->setRenderState(D3DRS_DESTBLEND, blend.dest);
	}
	render->setRenderState(D3DRS_BLENDOP, blend.opertaion);

	//BLEND FUNC
	if (blend.func != D3DCMP_ALWAYS)
		render->setRenderState(D3DRS_ALPHATESTENABLE, 1);
	else
		render->setRenderState(D3DRS_ALPHATESTENABLE, 0);
	render->setRenderState(D3DRS_ALPHAFUNC, blend.func);
	render->setRenderState(D3DRS_ALPHAREF, blend.alphaRef);
	
	//ZBUFFER
	render->setRenderState(D3DRS_CULLMODE, zBuffer.type);  
	render->setRenderState(D3DRS_ZWRITEENABLE, zBuffer.writeEnable ? 1 : 0);  
	render->setRenderState(D3DRS_ZFUNC, zBuffer.func);  

	//TO DO BIAS


	//STENCIL
	render->setRenderState(D3DRS_STENCILENABLE, stencil.enable ? 1 : 0);

	if (stencil.twoSide)
	{
		render->setRenderState(D3DRS_TWOSIDEDSTENCILMODE, 1);
		render->setRenderState(D3DRS_CCW_STENCILFAIL, stencil.fail);
		render->setRenderState(D3DRS_CCW_STENCILZFAIL, stencil.zFail);
		render->setRenderState(D3DRS_CCW_STENCILPASS, stencil.pass);
	}
	else
		render->setRenderState(D3DRS_TWOSIDEDSTENCILMODE, 0);
	render->setRenderState(D3DRS_STENCILFUNC, stencil.func);
	render->setRenderState(D3DRS_STENCILREF, stencil.ref);
	render->setRenderState(D3DRS_STENCILMASK, stencil.mask);
	render->setRenderState(D3DRS_STENCILFAIL, stencil.fail);
	render->setRenderState(D3DRS_STENCILZFAIL, stencil.zFail);
	render->setRenderState(D3DRS_STENCILPASS, stencil.pass);

	render->setRenderState(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(ambient.r, ambient.g, ambient.b, 1));
	render->setRenderState(D3DRS_LIGHTING, lightEnable ? 1 : 0);

	//FOG
	if (fog.mode == D3DFOG_NONE)
	{
		render->setRenderState(D3DRS_FOGENABLE, 0);
		render->setRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	}
	else
	{
		render->setRenderState(D3DRS_FOGENABLE, 1);
		render->setRenderState(D3DRS_FOGTABLEMODE, fog.mode);
		render->setRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);

		render->setRenderState(D3DRS_FOGCOLOR, D3DCOLOR_COLORVALUE(fog.color.r, fog.color.g, fog.color.b, fog.color.a));
		render->setRenderState(D3DRS_FOGSTART, fog.start);
		render->setRenderState(D3DRS_FOGEND, fog.end);
		render->setRenderState(D3DRS_FOGDENSITY, fog.densitiy);
	}
	for (int i = 0; i < 8; i++)
		render->setNULLTexture(i);

	for (int i = 0; i < stageList.size(); i++)
		stageList[i]->Bind();
	//for stages
}

D3DCOLORVALUE HWCOLORRGBA(float r, float g, float b, float a)
{
	D3DCOLORVALUE out;
	out.a = a;
	out.b = b;
	out.g = g;
	out.r = r;
	return out;
}

void HwPass::setDefault()
{
    renderOptions.cullMode = D3DCULL_NONE;
	renderOptions.fillMode = D3DFILL_SOLID;
	renderOptions.shadeMode = D3DSHADE_GOURAUD;

	blend.opertaion = D3DBLENDOP_ADD;
	blend.dest = D3DBLEND_ZERO;
	blend.src = D3DBLEND_ONE;
	blend.func = D3DCMP_GREATEREQUAL;
	blend.alphaRef = 7;

	zBuffer.type = D3DZB_TRUE;
	zBuffer.writeEnable = true;
	zBuffer.func = D3DCMP_LESSEQUAL;
	zBuffer.bias = 0;
	zBuffer.slopeBias = 0;

	stencil.enable = false;
	stencil.func = D3DCMP_ALWAYS;
	stencil.ref = 0;
	stencil.zFail = D3DSTENCILOP_KEEP;
	stencil.fail = D3DSTENCILOP_KEEP;
	stencil.pass = D3DSTENCILOP_KEEP;
	stencil.mask = 0xffffffff;
	stencil.twoSide = false;

	matColor.Ambient = HWCOLORRGBA(1, 1, 1, 1);
	matColor.Diffuse = HWCOLORRGBA(1, 1, 1, 1);
	matColor.Emissive = HWCOLORRGBA(0, 0, 0, 0);
	matColor.Specular = HWCOLORRGBA(0, 0, 0, 0);
	matColor.Power = 0;

	ambient = HWCOLORRGBA(0.5, 0.5, 0.5, 1);
	lightEnable = false;

	fog.color = HWCOLORRGBA(0.5, 0.5, 0.5, 1);
	fog.densitiy = 10;
	fog.end = 100;
	fog.start = 30;
	fog.mode = D3DFOG_NONE;
}

//-----------------------------------------------------------------------------
// Name: HwStage
// Desc: --------
//-----------------------------------------------------------------------------
HwStage::HwStage()
{
	index = 0;
	texIndex = 0;
	reMat = false;
	setDefault();
	pTexture = NULL;
}

void HwStage::setTexture(HwTexture *tex)
{
	pTexture = tex;
}
HwTexture *HwStage::setTexture(string name)
{
	pTexture = (HwTexture*)pParent->pParent->pParent->getManager()->getScene()->textures()->getByName(name);
	                      //pass    //techn  //material
	return pTexture;

}
HwTexture *HwStage::setTextureFromFile(string name, string fn)
{
	pTexture = (HwTexture*)pParent->pParent->pParent->getManager()->getScene()->textures()->CreateOrRetrieveRes(name, fn);
	                      //pass    //techn  //material
	return pTexture;
}

HwTexture *HwStage::getTexture()
{
	return pTexture;
}

void HwStage::setDefault()
{
	blend.colorBlendType = D3DTOP_MODULATE;
	blend.alphaBlendType = D3DTOP_DISABLE;

	blend.colorArg0 = D3DTA_DIFFUSE;
	blend.colorArg1 = D3DTA_TEXTURE;
	blend.colorArg2 = D3DTA_DIFFUSE;

	blend.alphaArg0 = D3DTA_DIFFUSE;
	blend.alphaArg1 = D3DTA_DIFFUSE;
	blend.alphaArg2 = D3DTA_DIFFUSE;

	srcBlend = D3DBLEND_SRCALPHA;
	destBlend = D3DBLEND_INVSRCALPHA;

	mag = D3DTEXF_LINEAR;
	min = D3DTEXF_LINEAR;
	mip = D3DTEXF_LINEAR;

	addressU = D3DTADDRESS_WRAP;
	addressV = D3DTADDRESS_WRAP;
	addressW = D3DTADDRESS_WRAP;

	scroll[0] = 0;
	scroll[1] = 0;

	scale[0] = 1;
	scale[1] = 1;

	angle = 0;

	recallMatrix();
}

void HwStage::recallMatrix()
{
	D3DXMATRIX xform, xlate, rot;
	D3DXMatrixIdentity(&xform);
	D3DXMatrixIdentity(&xlate);
	D3DXMatrixIdentity(&rot);

	float cosa, sina;
	xform.m[0][0] = 1 / scale[0];
	xform.m[1][1] = 1 / scale[1];
	xform.m[0][3] = (-0.5 * xform.m[0][0]) + 0.5;
	xform.m[1][3] = (-0.5 * xform.m[1][1]) + 0.5;

	xlate.m[0][3] = scroll[0];
	xlate.m[1][3] = scroll[1];

	D3DXMatrixMultiply(&xform, &xlate, &xform);

	cosa = cos(angle);
	sina = sin(angle);

	rot.m[0][0] =  cosa;
	rot.m[0][1] = -sina;
	rot.m[1][0] =  sina;
	rot.m[1][1] =  cosa;
	rot.m[0][3] =  0.5 + ( (-0.5 * cosa) - (-0.5 * sina) );
	rot.m[1][3] =  0.5 + ( (-0.5 * sina) + (-0.5 * cosa) );
	D3DXMatrixMultiply(&xform, &rot, &xform);
  
	matrix = xform;
	reMat = false;
}

void HwStage::Bind()
{
	if (reMat)
		recallMatrix();

	HwRender *render = pParent->pParent->pParent->getManager()->getEngine()->getRender();
	
	
	if (!pTexture)
	{
		render->setNULLTexture(index);
		return;
	}
	
	//cout << 

//	render->setTextureStageState(index, D3DTSS_COLOROP, blend.colorBlendType);
//	render->setTextureStageState(index, D3DTSS_ALPHAOP, blend.alphaBlendType);

	render->setTextureStageState(index, D3DTSS_COLORARG1, blend.colorArg1);
	render->setTextureStageState(index, D3DTSS_COLORARG2, blend.colorArg2);
	render->setTextureStageState(index, D3DTSS_COLORARG0, blend.colorArg0);
/*
	render->setTextureStageState(index, D3DTSS_ALPHAARG1, blend.alphaArg1);
	render->setTextureStageState(index, D3DTSS_ALPHAARG2, blend.alphaArg2);
	render->setTextureStageState(index, D3DTSS_ALPHAARG0, blend.alphaArg0);*/

	//render->setTextureStageState(index ,D3DTSS_TEXCOORDINDEX, texIndex);

	render->setSamplerState(index, D3DSAMP_MAGFILTER, mag);
	render->setSamplerState(index, D3DSAMP_MINFILTER, min);
	render->setSamplerState(index, D3DSAMP_MIPFILTER, mip);
	
	render->setSamplerState(index, D3DSAMP_ADDRESSU, addressU);
	render->setSamplerState(index, D3DSAMP_ADDRESSV, addressV);
	render->setSamplerState(index, D3DSAMP_ADDRESSW, addressW);
	
	render->setTextureTransform(index, &matrix);
	
	pTexture->setToDevice(index);

}
