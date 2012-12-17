#include "hwScene.h"
#include "hwEngine.h"

//-----------------------------------------------------------------------------
// Name: HwSceneObject
// Desc: -------------
//-----------------------------------------------------------------------------

HwSceneObject::HwSceneObject(HwEngine *eng) : HwRenderObject(eng)
{
//	++id;
//name = "object" + to_string(_Longlong(id));
	pos = D3DXVECTOR3(0, 0, 0);
	ang = D3DXVECTOR3(0, 0, 0);
	size = D3DXVECTOR3(1, 1, 1);
	angChanged = true;

	pMaterial = (HwMaterial *)eng->getScene()->materials()->getByName("Default");
	eng->getScene()->addObject(this);

	func = NULL;
}

// TO DO MATH

inline float sqr(float a)
{
	return a * a;
}

float HwSceneObject::distantioTo(HwSceneObject *obj)
{
	return sqrt(sqr(pos.x - obj->pos.x) + sqr(pos.y - obj->pos.y) + sqr(pos.z - obj->pos.z));
}

float HwSceneObject::distantioTo(D3DXVECTOR3 v)
{
	return sqrt(sqr(pos.x - v.x) + sqr(pos.y - v.y) + sqr(pos.z - v.z));
}

void HwSceneObject::turnTo(HwSceneObject *obj)
{
	turnTo(obj->getPos());
}

void HwSceneObject::turnTo(D3DXVECTOR3 v)
{
	float ax = 0;
	float ay = 0;
	float az = 0;

	float lxz = sqrt((v.x - pos.x) * (v.x - pos.x) + (v.z - pos.z) * (v.z - pos.z));
	ax = atan((v.y - pos.y) / lxz);
	ay = atan2(v.x - pos.x, v.z - pos.z);

	ang.x = ax;
	ang.y = ay;
	//ang.z = ax;

	angChanged = true;
}

void HwSceneObject::moveForward(float dist)
{
	float dx = dist * cos(ang.x) * sin(ang.y);
	float dz = dist * cos(ang.x) * cos(ang.y);
	float dy = -dist * sin(ang.x);
	pos.x += dx;
	pos.y += dy;
	pos.z += dz;
	posChanged = true;
}

void HwSceneObject::setPos(D3DXVECTOR3 v)
{
	pos = v;
	posChanged = true;
}

void HwSceneObject::setAng(D3DXVECTOR3 v)
{
	ang = v;
	angChanged = true;
}

void HwSceneObject::setDirection(D3DXVECTOR3 v)
{
	ang.y = 
	angChanged = true;
}

void HwSceneObject::setSize(D3DXVECTOR3 v)
{
	size = v;
	sizeChanged = true;
}

D3DXVECTOR3 &HwSceneObject::getPos()
{
	return pos;
}

D3DXVECTOR3 &HwSceneObject::getAng()
{
	return ang;
}

D3DXVECTOR3 &HwSceneObject::getSize()
{
	return size;
}

D3DXMATRIX &HwSceneObject::worldMatrix()
{
	reCalcWorldMatrix();
	return worldTransform;
}

void HwSceneObject::reCalcWorldMatrix()
{
	if (angChanged)
	{
		D3DXMATRIX rotx, roty, rotz;
		//D3DXMatrixIdentity(&worldTransform);
		//D3DXMatrixRotationX(&rotx,  ang.x);
		//D3DXMatrixRotationY(&roty,  ang.y);
		//D3DXMatrixRotationZ(&rotz,  ang.z);
	//	worldTransform = rotx * roty * rotz;
		D3DXMatrixRotationYawPitchRoll(&worldTransform, ang.y, ang.x, ang.z);
		posChanged = true;
		sizeChanged = true;
	}
	if (sizeChanged)
	{
		// TO DO.....
	}
	if (posChanged)
	{
		worldTransform._41 = pos.x;
		worldTransform._42 = pos.y;
		worldTransform._43 = pos.z;
	}
	posChanged = false;
	sizeChanged = false;
	angChanged = false;
}

void HwSceneObject::setMaterial(string name)
{
	pMaterial = (HwMaterial *)getEngine()->getScene()->materials()->getByName(name);
	if (pMaterial == NULL)
		pMaterial = (HwMaterial *)getEngine()->getScene()->materials()->getByName("Default");
}

void HwSceneObject::setMaterial(HwMaterial *m)
{
	pMaterial = m;
	if (pMaterial == NULL)
		pMaterial = (HwMaterial *)getEngine()->getScene()->materials()->getByName("Default");
}

HwMaterial *HwSceneObject::getMaterial()
{
	return pMaterial;
}

void HwSceneObject::setRenderFunc(SENDERFUNC f)
{
	func = f;
}

void HwSceneObject::render()
{
	reCalcWorldMatrix();
	getEngine()->getRender()->setTransform(D3DTS_WORLD, &worldTransform);

	if (func)
		func(this);
	
	for (int i = 0; i < pMaterial->getPassCount(); i++)
	{
		HwPass *pass = pMaterial->getPass(i);
		pass->Bind();
		if (pass->shaderValible())
			for (int j = 0; j < pass->getShader()->getPassCount(); j++)
			{
				HwShaderPass *spass = pass->getShader()->getPass(j);
				spass->Bind();
				draw();
				spass->UnBind();
			}
		else
			draw();

	}
}
void HwSceneObject::draw()
{

}
//-----------------------------------------------------------------------------
// Name: HwScene
// Desc: -------
//-----------------------------------------------------------------------------
HwScene::HwScene(HwEngine *eng) : HwObject(eng)
{
	pCamera = new HwCamera(eng);
	pMatManger = new HwMaterialManager(eng);
	pTexManager = new HwTextureManager(eng);
	pMeshManager = new HwMeshManager(eng);

	pShaderManager = new HwShaderManager(eng);
	pVProgrammManger = new HwVertexProgrammManager(eng);
	pPProgrammManger = new HwPixelProgrammManager(eng);
}

HwScene::~HwScene()
{
	delete pCamera;
	delete pMatManger;
	delete pTexManager;
	delete pMeshManager;
}
void HwScene::init()
{
	pCamera->init();
}

void HwScene::render()
{
	pCamera->update();

	for (int i = 0; i < sObjects.size(); i++)
		if (sObjects[i]->getVisible())
			sObjects[i]->render();
}
void HwScene::addObject(HwSceneObject *obj)
{
	sObjects.push_back(obj);
}

HwCamera *HwScene::getCamera()
{
	return pCamera;
}

void HwScene::setCamera(HwCamera *cam)
{
	pCamera = cam;
}

HwMaterialManager *HwScene::materials()
{
	return pMatManger;
}
HwTextureManager *HwScene::textures()
{
	return pTexManager;
}

HwMeshManager *HwScene::meshes()
{
	return pMeshManager;
}

HwShaderManager *HwScene::shaders()
{
	return pShaderManager;
}

HwVertexProgrammManager *HwScene::vps()
{
	return pVProgrammManger;
}

HwPixelProgrammManager *HwScene::pps()
{
	return pPProgrammManger;
}